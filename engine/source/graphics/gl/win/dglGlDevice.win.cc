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

S32 ChooseBestPixelFormat(HDC hDC, PIXELFORMATDESCRIPTOR *pPFD)
{
   PIXELFORMATDESCRIPTOR pfds[MAX_PFDS + 1];
   S32 i;
   S32 bestMatch = 0;

   S32 maxPFD = DescribePixelFormat(hDC, 1, sizeof(PIXELFORMATDESCRIPTOR), &pfds[0]);
   if (maxPFD > MAX_PFDS)
      maxPFD = MAX_PFDS;

   bool accelerated = false;

   for (i = 1; i <= maxPFD; i++)
   {
      DescribePixelFormat(hDC, i, sizeof(PIXELFORMATDESCRIPTOR), &pfds[i]);

      // make sure this has hardware acceleration:
      if ((pfds[i].dwFlags & PFD_GENERIC_FORMAT) != 0)
         continue;

      // verify pixel type
      if (pfds[i].iPixelType != PFD_TYPE_RGBA)
         continue;

      // verify proper flags
      if (((pfds[i].dwFlags & pPFD->dwFlags) & pPFD->dwFlags) != pPFD->dwFlags)
         continue;

      accelerated = !(pfds[i].dwFlags & PFD_GENERIC_FORMAT);

      //
      // selection criteria (in order of priority):
      //
      //  PFD_STEREO
      //  colorBits
      //  depthBits
      //  stencilBits
      //
      if (bestMatch)
      {
         // check stereo
         if ((pfds[i].dwFlags & PFD_STEREO) && (!(pfds[bestMatch].dwFlags & PFD_STEREO)) && (pPFD->dwFlags & PFD_STEREO))
         {
            bestMatch = i;
            continue;
         }

         if (!(pfds[i].dwFlags & PFD_STEREO) && (pfds[bestMatch].dwFlags & PFD_STEREO) && (pPFD->dwFlags & PFD_STEREO))
         {
            bestMatch = i;
            continue;
         }

         // check color
         if (pfds[bestMatch].cColorBits != pPFD->cColorBits)
         {
            // prefer perfect match
            if (pfds[i].cColorBits == pPFD->cColorBits)
            {
               bestMatch = i;
               continue;
            }
            // otherwise if this PFD has more bits than our best, use it
            else if (pfds[i].cColorBits > pfds[bestMatch].cColorBits)
            {
               bestMatch = i;
               continue;
            }
         }

         // check depth
         if (pfds[bestMatch].cDepthBits != pPFD->cDepthBits)
         {
            // prefer perfect match
            if (pfds[i].cDepthBits == pPFD->cDepthBits)
            {
               bestMatch = i;
               continue;
            }
            // otherwise if this PFD has more bits than our best, use it
            else if (pfds[i].cDepthBits > pfds[bestMatch].cDepthBits)
            {
               bestMatch = i;
               continue;
            }
         }

         // check stencil
         if (pfds[bestMatch].cStencilBits != pPFD->cStencilBits)
         {
            // prefer perfect match
            if (pfds[i].cStencilBits == pPFD->cStencilBits)
            {
               bestMatch = i;
               continue;
            }
            // otherwise if this PFD has more bits than our best, use it
            else if ((pfds[i].cStencilBits > pfds[bestMatch].cStencilBits) &&
               (pPFD->cStencilBits > 0))
            {
               bestMatch = i;
               continue;
            }
         }
      }
      else
      {
         bestMatch = i;
      }
   }

   if (!bestMatch)
      return 0;

   else if (pfds[bestMatch].dwFlags & PFD_GENERIC_ACCELERATED)
   {
      // MCD
   }
   else
   {
      // ICD
   }

   *pPFD = pfds[bestMatch];

   return bestMatch;
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

   if (!winState.appWindow)
   {
      Con::printf("Creating a new %swindow...", (fullScreen ? "full-screen " : ""));
      winState.appWindow = CreateOpenGLWindow(width, height, false, true);
      if (!winState.appWindow)
      {
         AssertFatal(false, "Failed to create a new window!");
         return false;
      }
   }

  winState.appDC = GetDC(winState.appWindow);
  if (!winState.appDC)
  {
     AssertFatal(false, "GetDC failed to get a valid device context!");
     return false;
  }

   PIXELFORMATDESCRIPTOR pfd;
   CreatePixelFormat(&pfd, 32, 0, 0); // 32 bit color... We do not need depth or stencil, OpenGL renders into a FBO and then copy the image to window
   S32 chosenFormat = ChooseBestPixelFormat(winState.appDC, &pfd);
   if (!SetPixelFormat(winState.appDC, chosenFormat, &pfd))
   {
      AssertFatal(false, "DGLDevice::init - cannot get the one and only pixel format we check for.");
   }
   Con::printf("  Pixel format set:");
   Con::printf("  %d color bits, %d depth bits, %d stencil bits", pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits);

   int OGL_MAJOR = 3;
   int OGL_MINOR = 1;

#if TORQUE_DEBUG
   int debugFlag = WGL_CONTEXT_DEBUG_BIT_ARB;
#else
   int debugFlag = 0;
#endif

   // Create a temp rendering context, needed a current context to use wglCreateContextAttribsARB
   HGLRC tempGLRC = wglCreateContext(winState.appDC);
   if (!wglMakeCurrent(winState.appDC, tempGLRC))
      AssertFatal(false, "Couldn't make temp GL context.");

   if (gglHasWExtension(ARB_create_context))
   {
      int const create_attribs[] = {
               WGL_CONTEXT_MAJOR_VERSION_ARB, OGL_MAJOR,
               WGL_CONTEXT_MINOR_VERSION_ARB, OGL_MINOR,
               WGL_CONTEXT_FLAGS_ARB, /*WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |*/ debugFlag,
               WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
               //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
               0
      };

      Con::printf("make attrib context");
      mContext = wglCreateContextAttribsARB(winState.appDC, 0, create_attribs);
      if (!mContext)
      {
         AssertFatal(0, "");
      }
   }
   else
      mContext = wglCreateContext(winState.appDC);

   winState.hGLRC = (HGLRC)mContext;
   // Delete temp rendering context
   wglMakeCurrent(NULL, NULL);
   wglDeleteContext(tempGLRC);

   if (!wglMakeCurrent(winState.appDC, (HGLRC)mContext))
      AssertFatal(false, "DGLGLDevice::init - cannot make our context current. Or maybe we can't create it.");

   // Change the window style:
   Con::printf("Changing the window style...");
   S32 windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
   windowStyle |= (WS_OVERLAPPEDWINDOW);

   if (winState.appWindow && !SetWindowLong(winState.appWindow, GWL_STYLE, windowStyle))
      Con::errorf("SetWindowLong failed to change the window style!");

   // Doesn't hurt to do this even it isn't necessary:
   ShowWindow(winState.appWindow, SW_SHOW);
   SetForegroundWindow(winState.appWindow);
   SetFocus(winState.appWindow);

   loadGlCore();
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

void DGLGLDevice::swapBuffers()
{
   SwapBuffers(GetDC(winState.appWindow));
}

DGLDevice* DGLGLDevice::create()
{
   WNDCLASS windowclass;
   dMemset(&windowclass, 0, sizeof(WNDCLASS));

   windowclass.lpszClassName = L"GFX-OpenGL";
   windowclass.style = CS_OWNDC;
   windowclass.lpfnWndProc = DefWindowProc;
   windowclass.hInstance = winState.appInstance;

   if (!RegisterClass(&windowclass))
      AssertFatal(false, "Failed to register the window class for the GL test window.");

   // Now create a window
   HWND hwnd = CreateWindow(L"GFX-OpenGL", L"", WS_POPUP, 0, 0, 640, 480,
      NULL, NULL, winState.appInstance, NULL);
   AssertFatal(hwnd != NULL, "Failed to create the window for the GL test window.");

   // Create a device context
   HDC tempDC = GetDC(hwnd);
   AssertFatal(tempDC != NULL, "Failed to create device context");

   // Create pixel format descriptor...
   PIXELFORMATDESCRIPTOR pfd;
   CreatePixelFormat(&pfd, 32, 0, 0, false);
   if (!SetPixelFormat(tempDC, ChoosePixelFormat(tempDC, &pfd), &pfd))
      AssertFatal(false, "I don't know who's responcible for this, but I want caught...");

   // Create a rendering context!
   HGLRC tempGLRC = wglCreateContext(tempDC);
   if (!wglMakeCurrent(tempDC, tempGLRC))
      AssertFatal(false, "I want them caught and killed.");

   Con::printf("  DGL Device: Test LoadGLAD");
   // Core GL. 
   loadGlCore();
   loadGlExtensions(tempDC);

   // Cleanup.
   wglMakeCurrent(NULL, NULL);
   wglDeleteContext(tempGLRC);
   ReleaseDC(hwnd, tempDC);
   DestroyWindow(hwnd);
   UnregisterClass(L"GFX-OpenGL", winState.appInstance);

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