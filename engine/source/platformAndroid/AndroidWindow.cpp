//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------


#include <platform/platformGL.h>
#include "platformAndroid/platformAndroid.h"
#include "graphics/dgl.h"
#include "graphics/gl/dglglDevice.h"
#include "platformAndroid/AndroidConsole.h"
#include "platform/platformInput.h"
#include "game/gameInterface.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "platformAndroid/AndroidEvents.h"
#include "platform/threads/thread.h"

#include "platformAndroid/AndroidWindow.h"

#include "platformAndroid/platformGL.h"

#include "platform/platformGL.h"

bool getStatusBarHidden();
bool setStatusBarHidden(bool);
void setStatusBarType(S32);

//------------------------------------------------------------------------------
#pragma mark ---- PlatState ----
AndroidPlatState platState;

void PlatformGL::setVSync(const int i)
{
    eglSwapInterval(platState.engine->display, i);
}

AndroidPlatState::AndroidPlatState()
{
    captureDisplay  = true;
    fadeWindows     = true;
    backgrounded    = false;
    minimized       = false;

    quit            = false;

    portrait        = true;//-Mat Android is in portrait mode by default


    // start with something reasonable.
    desktopBitsPixel = ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;
    desktopWidth    = ANDROID_DEFAULT_RESOLUTION_X;
    desktopHeight   = ANDROID_DEFAULT_RESOLUTION_Y;
    fullscreen      = true;

    osVersion       = 0;

    dStrcpy(appWindowTitle, "Android Torque Game Engine");

    // Semaphore for alerts. We put the app in a modal state by blocking the main
    alertSemaphore = Semaphore::createSemaphore(0);

    // directory that contains main.cs . This will help us detect whether we are
    // running with the scripts in the bundle or not.
    mainDotCsDir = NULL;
}


//------------------------------------------------------------------------------
// DGL, the Gui, and TS use this for various purposes.
const Point2I &Platform::getWindowSize()
{
    return platState.windowSize;
}


//------------------------------------------------------------------------------
// save the window size, for DGL's use
void Platform::setWindowSize(U32 newWidth, U32 newHeight)
{
    platState.windowSize.set(newWidth, newHeight);
}

//------------------------------------------------------------------------------
// Issue a minimize event. The standard handler will handle it.
void Platform::minimizeWindow()
{
    //no minimizing on Android
}

void Platform::restoreWindow()
{
    //no minimizing on Android
}

//------------------------------------------------------------------------------
void Platform::setWindowTitle(const char *title)
{
    //no window titles on Android
}



#pragma mark ---- Init funcs  ----
//------------------------------------------------------------------------------
void Platform::init()
{
    Con::setVariable("$platform", "Android");
    
    AndroidConsole::create();

    //if ( !AndroidConsole::isEnabled() )
    Input::init();

    // allow users to specify whether to capture the display or not when going fullscreen
    Con::addVariable("pref::Android::captureDisplay", TypeBool, &platState.captureDisplay);
    Con::addVariable("pref::Android::fadeWindows", TypeBool, &platState.fadeWindows);

    // create the opengl display device
    Con::printf("DGLDevice Init:");
    DGLDevice::init();
    if(!DGLDevice::installDevice(DGLGLDevice::create()))
    {
        Con::printf("Failed to install device");
    }

    Con::printf("");
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
    setMouseLock(false);
    DGLDevice::destroy();
    Input::destroy();
    AndroidConsole::destroy();
}


//Hidden by Default. 1 Black Opaque, 2 Black Translucent
S32 gScreenOrientation = 0;
S32 gStatusBarType = 0;
bool gStatusBarHidden = true;

//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
   // variable is never used
    //S32 resolutionWidth = ANDROID_DEFAULT_RESOLUTION_X;
    //S32 resolutionHeight = ANDROID_DEFAULT_RESOLUTION_Y;

    dSprintf(platState.appWindowTitle, sizeof(platState.appWindowTitle), name);

    platState.windowSize.x = _AndroidGetScreenWidth();
    platState.windowSize.y = _AndroidGetScreenHeight();

    //Default to landscape, and run into portrait if requested.
    S32 orientation = _AndroidGameGetOrientation();
    if (orientation == ACONFIGURATION_ORIENTATION_PORT)
    {
    	gScreenOrientation = 1;
    	platState.portrait = true;
    }
    else
    {
    	gScreenOrientation = 0;
    	platState.portrait = false;
    }

    bool fullScreen;
    U32 bpp = Con::getIntVariable("$pref::Android::ScreenDepth"); //ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;
    if (!bpp)
    {
        Con::printf("Default BPP Chosen , $pref::Android::ScreenDepth was not found.");
        bpp = ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;
    }
    
    fullScreen = true;

    if(!DGL->setDevice("OpenGL",platState.windowSize.x,platState.windowSize.y,bpp,fullScreen))
    {
        Con::printf("Platform:Set Device failed");
    }
}

//--------------------------------------
// run app function: not applicable to Android
//--------------------------------------
// run other apps
int runApp(const char *fileName, const char *batchArgs, bool blocking = false)
{
    return 0;
}

bool appIsRunning(int batchId)
{
    return false;
}

ConsoleFunction(setScreenOrientation, bool, 3, 3, "Sets the orientation of the screen ( portrait/landscape, upside down or right-side up )\n"
        "@(bool portrait, bool upside_down)"){
    adprintf("screen orientation is set via the manifest file on android");
	return false;
}
