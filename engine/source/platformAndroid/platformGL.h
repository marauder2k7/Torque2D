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


#ifndef _PLATFORMGL_H_
#define _PLATFORMGL_H_

/// Using aglMacro.h gives us signifigant performance gains in opengl immediate mode ( glVertex etc )
#if !defined(TORQUE_DEBUG)
//#define USE_AGL_MACRO
#endif

#if defined(USE_AGL_MACRO)
#if !defined(AGLContext)
typedef struct __AGLContextRec       *AGLContext;
#endif
#include <AGL/aglMacro.h>
extern AGLContext agl_ctx;
#endif


#define MIN_RESOLUTION_X				1
#define MIN_RESOLUTION_Y				1
#define MIN_RESOLUTION_BIT_DEPTH		16
#define MIN_RESOLUTION_XY_STRING		"480 320"

#define ANDROID_DEFAULT_RESOLUTION_X    1024
#define ANDROID_DEFAULT_RESOLUTION_Y    768

#define ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH	32

#define ANDROID_SCREEN_PORTRAIT			0
#define ANDROID_SCREEN_LANDSCAPE			1
#define ANDROID_SCREEN_RIGHT_SIDE_UP		3
#define ANDROID_SCREEN_UPSIDE_DOWN		4

#endif // _PLATFORMGL_H_
