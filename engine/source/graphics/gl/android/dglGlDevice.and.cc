#include "graphics/gl/dglglDevice.h"
#include "platformAndroid/platformAndroid.h"
#include "platformAndroid/platformGL.h"
#include "game/gameInterface.h"
#include "platformAndroid/T2DActivity.h"


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
    eglSwapBuffers(platState.engine->display, platState.engine->surface);
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

    adprintf("DGLGLDevice::Activate finished");

    initGLstate();
    return true;
}

DGLDevice* DGLGLDevice::create()
{
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            //EGL_ALPHA_SIZE, 8,
            //EGL_DEPTH_SIZE, 24,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    static const EGLint ctx_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(platState.engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, platState.engine->app->window, NULL);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        adprintf("Unable to eglMakeCurrent");
        return NULL;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    platState.engine->display = display;
    platState.engine->context = context;
    platState.engine->surface = surface;
    platState.engine->width = w;
    platState.engine->height = h;
    platState.engine->state.angle = 0;

    loadGlCore();

    platState.engine->animating = 1;

    DGLGLDevice *newOGLDevice = new DGLGLDevice();
    return (DGLDevice*)newOGLDevice;
}