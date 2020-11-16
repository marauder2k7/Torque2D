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
   mVideoModes.clear();

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
         for (U32 i = 0; i < (U32)mVideoModes.size(); i++)
         {
            if (devMode.dmPelsWidth == mVideoModes[i].w
               && devMode.dmPelsHeight == mVideoModes[i].h
               && devMode.dmBitsPerPel == mVideoModes[i].bpp)
            {
               alreadyInList = true;
               break;
            }
         }

         // If we've not already added this resolution, add it to our resolution list.
         if (!alreadyInList)
         {
            DGLVideoMode newRes(devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel);
            mVideoModes.push_back(newRes);
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

   HWND curtain = NULL;
   DGLVideoMode newRes(width, height, bpp);

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

   if (!winState.appWindow)
   {
      winState.appWindow = CreateOpenGLWindow(width, height, fullScreen, true);
   }

   HDC hdcGL = GetDC(winState.appWindow);

   int OGL_MAJOR = 3;
   int OGL_MINOR = 1;

#if TORQUE_DEBUG
   int debugFlag = WGL_CONTEXT_DEBUG_BIT_ARB;
#else
   int debugFlag = 0;
#endif

   HGLRC tempRC = wglCreateContext(hdcGL);
   if (!wglMakeCurrent(hdcGL, tempRC))
      AssertFatal(false, "couldnt make a temp context!");

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

      mContext = wglCreateContextAttribsARB(hdcGL, 0, create_attribs);
      if (!mContext)
      {
         AssertFatal(0, "");
      }

   }
   else
      mContext = wglCreateContext(hdcGL);

   wglMakeCurrent(NULL, NULL);
   wglDeleteContext(tempRC);

   if (!wglMakeCurrent(hdcGL, (HGLRC)mContext))
      AssertFatal(false, "DGLGLDevice::activate - unable to make context current!");

   loadGlCore();
   loadGlExtensions(hdcGL);

   initGLstate();

   winState.appDC = hdcGL;
   winState.hGLRC = (HGLRC)mContext;

   return true;
}

//------------------------------------------------------------------------------

void DGLGLDevice::create()
{
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
   if (!SetPixelFormat(tempDc, ChooseBestPixelFormat(tempDc, &pfd), &pfd))
   {
      AssertFatal(false, "unable to set pixel format");
   }

   // Create a temp rendering context.
   HGLRC tempGLRC = wglCreateContext(tempDc);
   if (!wglMakeCurrent(tempDc, tempGLRC))
      AssertFatal(false, "Unable to make temp current");

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
      DGLDevice::installDevice(newOGLDevice);
   }
}