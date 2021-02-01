#include "graphics/gl/dglglDevice.h"
#include "platformAndroid/platformAndroid.h"
#include "platformAndroid/platformGL.h"
#include "game/gameInterface.h"

extern void loadGlCore();
extern void loadGlExtensions(void* context);

void DGLGLDevice::enumerateVideoModes()
{
    mVideoModeList.push_back(DGLVideoMode(320, 480, 16));
    mVideoModeList.push_back(DGLVideoMode(480, 320, 16));

    mVideoModeList.push_back(DGLVideoMode(320, 480, 32));
    mVideoModeList.push_back(DGLVideoMode(480, 320, 32));

    mVideoModeList.push_back(DGLVideoMode(640, 960, 16));
    mVideoModeList.push_back(DGLVideoMode(960, 640, 16));

    mVideoModeList.push_back(DGLVideoMode(640, 960, 32));
    mVideoModeList.push_back(DGLVideoMode(960, 640, 32));

    mVideoModeList.push_back(DGLVideoMode(768, 1024, 16));
    mVideoModeList.push_back(DGLVideoMode(1024, 768, 16));

    mVideoModeList.push_back(DGLVideoMode(768, 1024, 32));
    mVideoModeList.push_back(DGLVideoMode(1024, 768, 32));

    mVideoModeList.push_back(DGLVideoMode(1536, 2048, 16));
    mVideoModeList.push_back(DGLVideoMode(2048, 1536, 16));

    mVideoModeList.push_back(DGLVideoMode(1536, 2048, 32));
    mVideoModeList.push_back(DGLVideoMode(2048, 1536, 32));

    mVideoModeList.push_back(DGLVideoMode(640, 1136, 16));
    mVideoModeList.push_back(DGLVideoMode(1136, 640, 16));

    mVideoModeList.push_back(DGLVideoMode(640, 1136, 32));
    mVideoModeList.push_back(DGLVideoMode(1136, 640, 32));
}

//------------------------------------------------------------------------------
void DGLGLDevice::shutdown()
{
    Con::printf("Shutting down the OpenGL display device...");
}

//------------------------------------------------------------------------------
void DGLGLDevice::swapBuffers()
{
    adprintf("swap buffer");
    bool b = eglSwapBuffers(platState.engine->display, platState.engine->surface);
    if(!b)
    {
        EGLint err = eglGetError();
        if(err == EGL_BAD_SURFACE)
        {
            adprintf("swap buffer bad surface");
            EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglInitialize(platState.engine->display,0,0);

            const EGLint attribs[] = { EGL_RENDERABLE_TYPE,
                                       EGL_OPENGL_ES2_BIT, //Request opengl ES2.0
                                       EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8,
                                       EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_NONE };
            EGLint w, h;
            EGLint num_configs;
            EGLConfig config;
            EGLSurface surface;
            eglChooseConfig( platState.engine->display, attribs, &config, 1, &num_configs );
            surface = eglCreateWindowSurface( platState.engine->display, config, platState.engine->app->window, NULL );

            eglQuerySurface(display, surface, EGL_WIDTH, &w);
            eglQuerySurface(display, surface, EGL_HEIGHT, &h);

            platState.engine->display = display;
            platState.engine->surface = surface;
            platState.engine->width = w;
            platState.engine->height = h;
            platState.engine->config = config;

        }
        else if( err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT )
        {
            adprintf("swap buffer bad context");
            const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, //Request opengl ES2.0
                                               EGL_NONE };
            eglMakeCurrent(platState.engine->display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);

            EGLContext context = eglCreateContext( platState.engine->display, platState.engine->config, NULL, context_attribs );

            if( eglMakeCurrent( platState.engine->display, platState.engine->surface, platState.engine->surface, context ) == EGL_FALSE )
            {
                Con::printf("Unable to make swap context current after error!");
            }
        }
    }
}

//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool DGLGLDevice::setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint)
{
    Con::printf(" set screen mode %i x %i x %i, %s, %s, %s", width, height, bpp,
                fullScreen ? "fullscreen" : "windowed",
                forceIt ? "force it" : "dont force it",
                repaint ? "repaint" : "dont repaint"
    );

    // validation, early outs --------------------------------------------------
    // sanity check. some scripts are liable to pass in bad values.
    if (!bpp)
        bpp = platState.desktopBitsPixel;

    DGLVideoMode newRes = DGLVideoMode(width, height, bpp);

    // if no values changing and we're not forcing a change, kick out. prevents thrashing.
    if (!forceIt && smIsFullScreen == fullScreen && smCurrentRes == newRes)
        return (true);

    Con::printf(">> Attempting to change display settings to %s %dx%dx%d...",
                fullScreen ? "fullscreen" : "windowed", newRes.w, newRes.h, newRes.bpp);

    // set torque variables ----------------------------------------------------
    // save window size for dgl
    Platform::setWindowSize(newRes.w, newRes.h);

    // update smIsFullScreen and pref
    smIsFullScreen = fullScreen;

    Con::setBoolVariable("$pref::Video::fullScreen", smIsFullScreen);

    // save resolution
    smCurrentRes = newRes;

    // save resolution to prefs
    char buf[32];

    if (fullScreen)
    {
        dSprintf(buf, sizeof(buf), "%d %d %d", newRes.w, newRes.h, newRes.bpp);
        Con::setVariable("$pref::Video::resolution", buf);
    }
    else
    {
        dSprintf(buf, sizeof(buf), "%d %d", newRes.w, newRes.h);
        Con::setVariable("$pref::Video::windowedRes", buf);
    }

    // Reload the textures gl names
    Con::printf("Resurrecting the texture manager...");
    Game->textureResurrect();

    if (repaint)
        Con::evaluate("resetCanvas();");

    return true;
}

bool DGLGLDevice::activate(U32 width, U32 height, U32 bpp, bool fullScreen)
{

    // validation, early outs --------------------------------------------------
    // sanity check. some scripts are liable to pass in bad values.
    if (!bpp)
        bpp = platState.desktopBitsPixel;

    DGLVideoMode newRes = DGLVideoMode(width, height, bpp);

    Con::printf(">> Attempting to change display settings to %s %dx%dx%d...",
                fullScreen ? "fullscreen" : "windowed", newRes.w, newRes.h, newRes.bpp);

    // set torque variables ----------------------------------------------------
    // save window size for dgl
    Platform::setWindowSize(newRes.w, newRes.h);

    Con::printf("DGLDevice::create Device");

    // update smIsFullScreen and pref
    smIsFullScreen = fullScreen;

    Con::setBoolVariable("$pref::Video::fullScreen", smIsFullScreen);

    // save resolution
    smCurrentRes = newRes;

    // save resolution to prefs
    char buf[32];

    if (fullScreen)
    {
        dSprintf(buf, sizeof(buf), "%d %d %d", newRes.w, newRes.h, newRes.bpp);
        Con::setVariable("$pref::Video::resolution", buf);
    }
    else
    {
        dSprintf(buf, sizeof(buf), "%d %d", newRes.w, newRes.h);
        Con::setVariable("$pref::Video::windowedRes", buf);
    }

    initGLstate();

    adprintf("DGLGLDevice::Activate finished");

    return true;
}

DGLDevice* DGLGLDevice::create()
{
    loadGlCore();

    DGLGLDevice *newOGLDevice = new DGLGLDevice();
    return newOGLDevice;
}