#include "platformWin32/platformWin32.h"
#include "platformWin32/platformGL.h"
#include "graphics/gl/glad/gladWGL.h"
#include "graphics/gl/dglglDevice.h"
#include "game/gameInterface.h"

void CreatePixelFormat(PIXELFORMATDESCRIPTOR *mPFD, S32 colorBits, S32 depthBits, S32 stencilBits)
{
   PIXELFORMATDESCRIPTOR src =
   {
      sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd
      1,                               // version number
      PFD_DRAW_TO_WINDOW |             // support window
      PFD_SUPPORT_OPENGL |             // support OpenGL
      PFD_DOUBLEBUFFER,                // double buffered
      PFD_TYPE_RGBA,                   // RGBA type
      colorBits,                       // color depth
      0, 0, 0, 0, 0, 0,                // color bits ignored
      0,                               // no alpha buffer
      0,                               // shift bit ignored
      0,                               // no accumulation buffer
      0, 0, 0, 0,                      // accum bits ignored
      depthBits,                       // z-buffer
      stencilBits,                     // stencil buffer
      0,                               // no auxiliary buffer
      PFD_MAIN_PLANE,                  // main layer
      0,                               // reserved
      0, 0, 0                          // layer masks ignored
   };

   *mPFD = src;
}

extern void loadGlCore();
extern void loadGlExtensions(void* context);

void DGLGLDevice::enumerateVideoModes()
{
   mVideoModeList.clear();

   // Enumerate all available resolutions:
   DEVMODE devMode;
   U32 modeNum = 0;
   U32 foundDisplayMode = true;

   while (foundDisplayMode)
   {
      // Clear out our display mode settings structure.
      dMemset(&devMode, 0, sizeof(devMode));
      devMode.dmSize = sizeof(devMode);

      // Check for display mode settings
      foundDisplayMode = EnumDisplaySettings(NULL, modeNum++, &devMode);

      if (devMode.dmPelsWidth >= MIN_RESOLUTION_X && devMode.dmPelsHeight >= MIN_RESOLUTION_Y && (devMode.dmBitsPerPel == 16 || devMode.dmBitsPerPel == 32))
      {
         // Only add this resolution if it is not already in the list:
         bool alreadyInList = false;
         for (U32 i = 0; i < (U32)mVideoModeList.size(); i++)
         {
            if (devMode.dmPelsWidth == mVideoModeList[i].w
               && devMode.dmPelsHeight == mVideoModeList[i].h
               && devMode.dmBitsPerPel == mVideoModeList[i].bpp)
            {
               alreadyInList = true;
               break;
            }
         }

         // If we've not already added this resolution, add it to our resolution list.
         if (!alreadyInList)
         {
            DGLVideoMode newRes(devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel);
            mVideoModeList.push_back(newRes);
         }
      }
   }

}

//------------------------------------------------------------------------------

void DGLGLDevice::shutdown()
{
   Con::printf("Shutting down the OpenGL display device...");

   if (winState.hGLRC)
   {
      if (mRestoreGamma)
         SetDeviceGammaRamp(winState.appDC, mOriginalRamp);

      Con::printf("Making the GL rendering context not current...");
      wglMakeCurrent(NULL, NULL);
      if (Con::getBoolVariable("$pref::Video::deleteContext", true))
      {
         Con::printf("Deleting the GL rendering context...");
         wglDeleteContext(winState.hGLRC);
      }
      winState.hGLRC = NULL;
   }

   if (winState.appDC)
   {
      Con::printf("Releasing the device context...");
      ReleaseDC(winState.appWindow, winState.appDC);
      winState.appDC = NULL;
   }

   if (smIsFullScreen)
   {
      Con::printf("Restoring the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel);
      ChangeDisplaySettings(NULL, 0);
   }
}

//------------------------------------------------------------------------------

bool DGLGLDevice::activate(U32 width, U32 height, U32 bpp, bool fullScreen)
{
   Con::printf("Activating the OpenGL display device...");

   bool needResurrect = false;

   // If the rendering context exists, delete it:
   if (winState.hGLRC)
   {
      Con::printf("Killing the texture manager...");
      Game->textureKill();
      needResurrect = true;

      Con::printf("Making the rendering context not current...");
      if (!wglMakeCurrent(NULL, NULL))
      {
         AssertFatal(false, "OpenGLDevice::activate\ndwglMakeCurrent( NULL, NULL ) failed!");
         return false;
      }

      Con::printf("Deleting the rendering context ...");
      if (!wglDeleteContext(winState.hGLRC))
      {
         AssertFatal(false, "OpenGLDevice::activate\ndwglDeleteContext failed!");
         return false;
      }
      winState.hGLRC = NULL;
   }

   // [neo, 5/31/2007 - #3174]
   if (winState.appMenu)
   {
      DestroyMenu(winState.appMenu);

      winState.appMenu = NULL;
   }

   // If the window already exists, kill it so we can start fresh:
   if (winState.appWindow)
   {
      if (winState.appDC)
      {
         Con::printf("Releasing the device context...");
         ReleaseDC(winState.appWindow, winState.appDC);
         winState.appDC = NULL;
      }

      Con::printf("Destroying the window...");
      DestroyWindow(winState.appWindow);
      winState.appWindow = NULL;
   }

   loadGlCore();
   
   // Set the resolution:
   if (!setScreenMode(width, height, bpp, (fullScreen || mFullScreenOnly), true, false))
      return false;

   loadGlExtensions(winState.hGLRC);
   // Output some driver info to the console:
   const char* vendorString = (const char*)glGetString(GL_VENDOR);
   const char* rendererString = (const char*)glGetString(GL_RENDERER);
   const char* versionString = (const char*)glGetString(GL_VERSION);
   Con::printf("OpenGL driver information:");
   if (vendorString)
      Con::printf("  Vendor: %s", vendorString);
   if (rendererString)
      Con::printf("  Renderer: %s", rendererString);
   if (versionString)
      Con::printf("  Version: %s", versionString);

   Con::setVariable("$pref::Video::displayDevice", mDeviceName);

   initGLstate();

   return true;
}

//------------------------------------------------------------------------------

DGLDevice* DGLGLDevice::create()
{

   Con::printf("  DGL Device: Test Create");
   bool fullScreenOnly = false;

   WNDCLASS wc;
   dMemset(&wc, 0, sizeof(wc));
   wc.style          = CS_OWNDC;
   wc.lpfnWndProc    = DefWindowProc;
   wc.hInstance      = winState.appInstance;
   wc.lpszClassName  = dT("OGLTest");
   RegisterClass(&wc);

   //Test window.
   HWND test = CreateWindow(dT("OGLTest"), dT(""), WS_POPUP, 0, 0, MIN_RESOLUTION_X, MIN_RESOLUTION_Y, NULL, NULL, winState.appInstance, NULL);
   AssertFatal(test != NULL, "Failed to create the window for the GL test window.");

   // Test device context.
   HDC tempDc = GetDC(test);
   AssertFatal(tempDc != NULL, "Failed to create device context for test");

   // Pixel format descriptor.
   PIXELFORMATDESCRIPTOR pfd;
   // Not testing for 16bit for the time being. 
   CreatePixelFormat(&pfd, 32, 0, 0, false);
   if (!SetPixelFormat(tempDc, ChoosePixelFormat(tempDc, &pfd), &pfd))
   {
      AssertFatal(false, "unable to set pixel format");
   }
   
   // Create a temp rendering context.
   HGLRC tempGLRC = wglCreateContext(tempDc);
   if (!wglMakeCurrent(tempDc, tempGLRC))
      AssertFatal(false, "Unable to make temp current");

   Con::printf("  DGL Device: Test LoadGLAD");
   // Core GL. 
   loadGlCore();
   loadGlExtensions(tempDc);

   // Cleanup.
   wglMakeCurrent(NULL, NULL);
   wglDeleteContext(tempGLRC);
   ReleaseDC(test, tempDc);
   DestroyWindow(test);
   UnregisterClass(dT("OGLTest"), winState.appInstance);

   DGLGLDevice* newOGLDevice = new DGLGLDevice();
   newOGLDevice->enumerateVideoModes();
   if (newOGLDevice)
   {
      return newOGLDevice;
   }
}

//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool DGLGLDevice::setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint)
{
   HWND curtain = NULL;
   char errorMessage[256];
   DGLVideoMode newRes(width, height, bpp);
   bool newFullScreen = fullScreen;
   bool safeModeOn = Con::getBoolVariable("$pref::Video::safeModeOn");

   if (!newFullScreen && mFullScreenOnly)
   {
      Con::warnf(ConsoleLogEntry::General, "OpenGLDevice::setScreenMode - device or desktop color depth does not allow windowed mode!");
      newFullScreen = true;
   }

   if (!newFullScreen && ((S32)newRes.w >= winState.desktopClientWidth || (S32)newRes.h >= winState.desktopClientHeight))
   {
      Con::warnf(ConsoleLogEntry::General, "OpenGLDevice::setScreenMode -- can't switch to resolution larger than desktop in windowed mode!");
      S32 resIndex = 0;
      S32 bestScore = 0, thisScore = 0;
      for (S32 i = 0; i < mVideoModeList.size(); i++)
      {
         if (newRes == mVideoModeList[i])
         {
            resIndex = i;
            break;
         }
         else if ((S32)newRes.h <= winState.desktopClientHeight && (S32)newRes.w <= winState.desktopClientWidth)
         {
            thisScore = abs(S32(newRes.w) - S32(mVideoModeList[i].w))
               + abs(S32(newRes.h) - S32(mVideoModeList[i].h))
               + (newRes.bpp == mVideoModeList[i].bpp ? 0 : 1);

            if (!bestScore || (thisScore < bestScore))
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }

   }

   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   if (newRes.w < MIN_RESOLUTION_X || newRes.h < MIN_RESOLUTION_Y)
   {
      Con::warnf(ConsoleLogEntry::General, "OpenGLDevice::setScreenMode -- can't go smaller than 640x480!");
      return false;
   }

   if (newFullScreen)
   {
      if (newRes.bpp != 16 && mFullScreenOnly)
         newRes.bpp = 16;

      // Match the new resolution to one in the list:
      U32 resIndex = 0;
      U32 bestScore = 0, thisScore = 0;
      for (int i = 0; i < mVideoModeList.size(); i++)
      {
         if (newRes == mVideoModeList[i])
         {
            resIndex = i;
            break;
         }
         else
         {
            thisScore = abs(S32(newRes.w) - S32(mVideoModeList[i].w))
               + abs(S32(newRes.h) - S32(mVideoModeList[i].h))
               + (newRes.bpp == mVideoModeList[i].bpp ? 0 : 1);

            if (!bestScore || (thisScore < bestScore))
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }

      newRes = mVideoModeList[resIndex];
   }
   else
   {
      // Basically ignore the bit depth parameter:
      newRes.bpp = winState.desktopBitsPixel;
   }

   // Return if already at this resolution:
   if (!forceIt && newRes == smCurrentRes && newFullScreen == smIsFullScreen)
      return true;

   Con::printf("Setting screen mode to %dx%dx%d (%s)...", newRes.w, newRes.h, newRes.bpp, (newFullScreen ? "fs" : "w"));

   bool needResurrect = false;

   if ((newRes.bpp != smCurrentRes.bpp) || (safeModeOn && ((smIsFullScreen != newFullScreen) || newFullScreen)))
   {
      // Delete the rendering context:
      if (winState.hGLRC)
      {
         if (!smNeedResurrect)
         {
            Con::printf("Killing the texture manager...");
            Game->textureKill();
            needResurrect = true;
         }

         Con::printf("Making the rendering context not current...");
         if (!wglMakeCurrent(NULL, NULL))
         {
            AssertFatal(false, "OpenGLDevice::setScreenMode\ndwglMakeCurrent( NULL, NULL ) failed!");
            return false;
         }

         Con::printf("Deleting the rendering context...");
         if (Con::getBoolVariable("$pref::Video::deleteContext", true) &&
            !wglDeleteContext(winState.hGLRC))
         {
            AssertFatal(false, "OpenGLDevice::setScreenMode\ndwglDeleteContext failed!");
            return false;
         }
         winState.hGLRC = NULL;
      }

      // Release the device context:
      if (winState.appDC)
      {
         Con::printf("Releasing the device context...");
         ReleaseDC(winState.appWindow, winState.appDC);
         winState.appDC = NULL;
      }

      // [neo, 5/31/2007 - #3174]
      if (winState.appMenu)
      {
         DestroyMenu(winState.appMenu);

         winState.appMenu = NULL;
      }

      // Destroy the window:
      if (winState.appWindow)
      {
         Con::printf("Destroying the window...");
         DestroyWindow(winState.appWindow);
         winState.appWindow = NULL;
      }
   }
   else if (smIsFullScreen != newFullScreen)
   {
      // Change the window style:
      Con::printf("Changing the window style...");
      S32 windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      if (newFullScreen)
         windowStyle |= (WS_MAXIMIZE | WS_VISIBLE);
      else
         windowStyle |= (WS_OVERLAPPEDWINDOW);

      if (winState.appWindow && !SetWindowLong(winState.appWindow, GWL_STYLE, windowStyle))
         Con::errorf("SetWindowLong failed to change the window style!");
   }

   if (winState.appWindow && !newFullScreen)
      ShowWindow(winState.appWindow, SW_NORMAL);
   else
      ShowWindow(winState.appWindow, SW_MAXIMIZE);


   U32 test;
   if (newFullScreen)
   {
      // Change the display settings:
      DEVMODE devMode;
      dMemset(&devMode, 0, sizeof(devMode));
      devMode.dmSize = sizeof(devMode);
      devMode.dmPelsWidth = newRes.w;
      devMode.dmPelsHeight = newRes.h;
      devMode.dmBitsPerPel = newRes.bpp;
      devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

      Con::printf("Changing the display settings to %dx%dx%d...", newRes.w, newRes.h, newRes.bpp);
      curtain = CreateCurtain(newRes.w, newRes.h);
      test = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
      if (test != DISP_CHANGE_SUCCESSFUL)
      {
         smIsFullScreen = false;
         Con::setBoolVariable("$pref::Video::fullScreen", false);

         ChangeDisplaySettings(NULL, 0);
         Con::errorf(ConsoleLogEntry::General, "OpenGLDevice::setScreenMode - ChangeDisplaySettings failed.");
         switch (test)
         {
         case DISP_CHANGE_RESTART:
            Platform::AlertOK("Display Change Failed", "You must restart your machine to get the specified mode.");
            break;

         case DISP_CHANGE_BADMODE:
            Platform::AlertOK("Display Change Failed", "The specified mode is not supported by this device.");
            break;

         default:
            Platform::AlertOK("Display Change Failed", "Hardware failed to switch to the specified mode.");
            break;
         };
         DestroyWindow(curtain);
         return false;
      }
      else
         smIsFullScreen = true;
   }
   else if (smIsFullScreen)
   {
      Con::printf("Changing to the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel);
      ChangeDisplaySettings(NULL, 0);
      smIsFullScreen = false;
   }

   Con::setBoolVariable("$pref::Video::fullScreen", smIsFullScreen);


   bool newWindow = false;
   if (!winState.appWindow)
   {
      Con::printf("Creating a new %swindow...", (fullScreen ? "full-screen " : ""));
      winState.appWindow = CreateOpenGLWindow(newRes.w, newRes.h, newFullScreen, true);
      if (!winState.appWindow)
      {
         AssertFatal(false, "OpenGLDevice::setScreenMode\nFailed to create a new window!");
         return false;
      }
      newWindow = true;
   }

   // Move the window:
   if (!newFullScreen)
   {
      // Adjust the window rect to compensate for the window style:
      RECT windowRect;
      windowRect.left = windowRect.top = 0;
      windowRect.right = newRes.w;
      windowRect.bottom = newRes.h;

      AdjustWindowRect(&windowRect, GetWindowLong(winState.appWindow, GWL_STYLE), false);
      U32 adjWidth = windowRect.right - windowRect.left;
      U32 adjHeight = windowRect.bottom - windowRect.top;

      // Center the window on the desktop:
      U32 xPos = 0, yPos = 0;

      if (adjWidth < (U32)winState.desktopClientWidth)
         xPos = (winState.desktopClientWidth - adjWidth) / 2;

      if (adjHeight < (U32)winState.desktopClientHeight)
         yPos = (winState.desktopClientHeight - adjHeight) / 2;

      test = SetWindowPos(winState.appWindow, 0, xPos, yPos, adjWidth, adjHeight, SWP_NOZORDER);
      if (!test)
      {
         dSprintf(errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed trying to move the window to (%d,%d) and size it to %dx%d.", xPos, yPos, newRes.w, newRes.h);
         AssertFatal(false, errorMessage);
         return false;
      }

      AnimateWindow(winState.appWindow, 90, AW_BLEND);
   }
   else if (!newWindow)
   {
      // Move and size the window to take up the whole screen:
      if (!SetWindowPos(winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
         || !SetWindowPos(winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER))
      {
         dSprintf(errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h);
         AssertFatal(false, errorMessage);
         return false;
      }
   }

   bool newDeviceContext = false;
   if (!winState.appDC)
   {
      // Get a new device context:
      Con::printf("Acquiring a new device context...");
      winState.appDC = GetDC(winState.appWindow);
      if (!winState.appDC)
      {
         AssertFatal(false, "OpenGLDevice::setScreenMode\nGetDC failed to get a valid device context!");
         return false;
      }
      newDeviceContext = true;
   }

   if (newWindow)
   {
      // Set the pixel format of the new window:
      PIXELFORMATDESCRIPTOR pfd;
      CreatePixelFormat(&pfd, newRes.bpp, 24, 8, false);
      S32 chosenFormat = ChoosePixelFormat(winState.appDC, &pfd);
      if (!chosenFormat)
      {
         AssertFatal(false, "OpenGLDevice::setScreenMode\nNo valid pixel formats found!");
         return false;
      }
      if (!SetPixelFormat(winState.appDC, chosenFormat, &pfd))
      {
         AssertFatal(false, "unable to set pixel format");
      }
      Con::printf("Pixel format set:");
      Con::printf("  %d color bits, %d depth bits, %d stencil bits", pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits);
   }

   if (!winState.hGLRC)
   {
      // Create a new rendering context:
      Con::printf("Creating a new rendering context...");
      winState.hGLRC = wglCreateContext(winState.appDC);
      if (!winState.hGLRC)
      {
         AssertFatal(false, "OpenGLDevice::setScreenMode\ndwglCreateContext failed to create an OpenGL rendering context!");
         return false;
      }

      // Make the new rendering context current:
      Con::printf("Making the new rendering context current...");
      if (!wglMakeCurrent(winState.appDC, winState.hGLRC))
      {
         AssertFatal(false, "OpenGLDevice::setScreenMode\ndwglMakeCurrent failed to make the rendering context current!");
         return false;
      }

      // Just for kicks.  Seems a relatively central place to put this...
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if (needResurrect)
      {
         // Reload the textures:
         Con::printf("Resurrecting the texture manager...");
         Game->textureResurrect();
      }
   }

   // Just for kicks.  Seems a relatively central place to put this...
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   smCurrentRes = newRes;
   Platform::setWindowSize(newRes.w, newRes.h);
   char tempBuf[15];
   dSprintf(tempBuf, sizeof(tempBuf), "%d %d %d", smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp);
   if (fullScreen)
      Con::setVariable("$pref::Video::resolution", tempBuf);
   else
      Con::setVariable("$pref::Video::windowedRes", tempBuf);

   if (curtain)
      DestroyWindow(curtain);

   // Doesn't hurt to do this even it isn't necessary:
   ShowWindow(winState.appWindow, SW_SHOW);
   SetForegroundWindow(winState.appWindow);
   SetFocus(winState.appWindow);

   if (newFullScreen)
   {
      // Doesn't hurt to do this an extra time, and seems to help in some cases
      if (!SetWindowPos(winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
         || !SetWindowPos(winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER))
      {
         dSprintf(errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h);
         AssertFatal(false, errorMessage);
         return false;
      }
   }

   //Luma:	Clear window at first, as it is showing previous gl color buffer stuff.
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   if (repaint)
      Con::evaluate("resetCanvas();");

   return true;
}