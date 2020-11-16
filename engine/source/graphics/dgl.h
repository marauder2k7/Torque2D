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

#ifndef _DGL_H_
#define _DGL_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _PLATFORMGL_H_
#include "platform/platformGL.h"
#endif
#ifndef _DGLENUMS_H_
#include "graphics/dglEnums.h"
#endif

#include "graphics/dglStructs.h"

/*class TextureObject;
class GFont;
class MatrixF;
class RectI;
class ColorI;
class ColorF;
class Point2I;
class Point2F;
class Point3F;*/
 enum GFlipConstants
 {
    GFlip_None = 0,
    GFlip_X = BIT(0),
    GFlip_Y = BIT(1),
    GFlip_XY = GFlip_X | GFlip_Y
 };

 /// Number of full screen anti-aliasting samples
 extern signed int gFSAASamples;

/// @defgroup dgl Cross-Platform OpenGL Support
/// functions used to map Torque2D calls to OpenGL (or OpenGL ES) calls per platform

//------------------------------------------------------------------------------
//-------------------------------------- Bitmap Drawing
//
/// Enumerations used for telling the bitmap drawing how/if to flip a bitmap
/// @see DrawBitmap
/// @see DrawBitmapStretch
/// @see DrawBitmapSR
/// @see DrawBitmapStretchSR
#define DGL DGLDevice::get()

 class DGLDevice
 {
 private:
    friend class TextureObject;
    friend class GFont;
    friend class MatrixF;
    friend class RectI;
    friend class ColorI;
    friend class ColorF;
    friend class Point2I;
    friend class Point2F;
    friend class Point3F;

 public:

    const char* mDeviceName;

    DGLDevice();

    static DGLDevice *get() { return smDGL; }

    /// Video class funcions
    // Creation of device funtions.
    static void init();
    static void destroy();                               // clean up and shut down
    static bool installDevice(DGLDevice *dev);
    static bool setDevice( const char *renderName, 
                           U32 width, U32 height, 
                           U32 bpp, bool fullScreen);    // set the current display device
    virtual bool activate(U32 width, U32 height, U32 bpp, bool fullScreen) = 0;
    static const char* getDriverInfo();                  // get info about the current display device driver
    static const char* getDeviceList();                  // get a tab-separated list of all the installed display devices

 private:
    static DGLDevice             * smDGL;
    static Vector<DGLDevice *>   smDeviceList;
    static DGLDevice*            smCurrentDevice;
    static bool                  smCritical;
    

 protected:
    static DGLVideoMode    smCurrentRes;
    Vector<DGLVideoMode>   mVideoModes;

    static bool            smIsFullScreen;
    bool                   mFullScreenOnly;

    static bool            smDisableVsync;

 public:
    static bool smNeedResurrect;
    static bool smEdgeClamp;

    virtual void shutdown() = 0;

    virtual bool setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true) = 0;
    virtual bool setResolution(U32 width, U32 height, U32 bpp);
    virtual bool toggleFullScreen();
    virtual void swapBuffers() = 0;
    virtual bool getGammaCorrection(F32 &g) = 0;
    virtual bool setGammaCorrection(F32 g) = 0;
    virtual bool getVerticalSync() = 0;
    virtual bool setVerticalSync(bool on) = 0;
    virtual void create() = 0;
    bool prevRes();
    bool nextRes();
    const char* getResolutionList();
    bool isFullScreenOnly() { return(mFullScreenOnly); }

    static DGLVideoMode getResolution();
    static bool       isFullScreen();

    /// @defgroup dgl_bitmap_mod Bitmap Modulation
    /// @ingroup dgl
    /// These functions control a modulation color that is used to modulate all drawn objects
    /// @{

    /// Sets the current color to modulate objects with, similar to glColor3fv()
    /// @see dglGetBitmapModulation
    /// @see dglClearBitmapModulation
    void SetBitmapModulation(const ColorF& in_rColor);
    /// Gets the current color modulation in float format [0,1]
    /// @see dglSetBitmapModulation
    /// @see dglClearBitmapModulation
    void GetBitmapModulation(ColorF* color);
    /// Gets the current color modulation in integer format [0,256)
    /// @see dglSetBitmapModulation
    /// @see dglClearBitmapModulation
    void GetBitmapModulation(ColorI* color);
    /// Sets current color modulation to white, so no modulation takes place
    /// @see dglSetBitmapModulation
    /// @see dglGetBitmapModulation
    void ClearBitmapModulation();

    /// @}


    // Note that you must call this _after_ SetBitmapModulation if the two are different
    //  SetBMod sets the text anchor to the modulation color
    /// Sets the anchor color for text coloring, useful when mixing text colors
    void SetTextAnchorColor(const ColorF&);

    void DrawBlendBox(RectI & bounds, ColorF & c1, ColorF & c2, ColorF & c3, ColorF & c4);

    void DrawBlendRangeBox(RectI & bounds, bool vertical, U8 numColors, ColorI * colors);

    /// @defgroup dgl_bitmap_draw Bitmap Drawing Functions
    /// @ingroup dgl
    /// These functions allow you to draw a bitmap.
    /// Each function will draw the bitmap in a specific way regarding stretching
    /// @{

    /// Draws a bitmap, starting from the lower left corner, going up and right
    /// @param texObject texture object to be drawn
    /// @param in_rAt where to draw the texture in 2d coordinates
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    virtual void DrawBitmap(TextureObject* texObject,
       const Point2I& in_rAt,
       const U32      in_flip = GFlip_None);
    /// Draws a power-of-two bitmap that is tiled. Requires a POT bitmap to draw properly.
    /// @param texObject texture object to be drawn
    /// @param in_rTile rectangle where the texture will be drawn in 2d coordinates
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    virtual void DrawBitmapTile(TextureObject* texObject,
       const RectI&   in_rTile,
       const U32      in_flip = GFlip_None,
       F32			    fSpin = 0.0f,
       bool           bSilhouette = false);
    /// Draws a bitmap that is stretched
    /// @param texObject texture object to be drawn
    /// @param in_rStretch rectangle where the texture will be drawn in 2d coordinates
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    virtual void DrawBitmapStretch(TextureObject* texObject,
       const RectI&   in_rStretch,
       const U32      in_flip = GFlip_None,
       F32			    fSpin = 0.0f,
       bool           bSilhouette = false);
    /// Draws a sub region of a texture
    /// @param texObject texture object to be drawn
    /// @param in_rAt point where the texture is to be drawn
    /// @param in_rSubRegion portion of the texture to be drawn
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    virtual void DrawBitmapSR(TextureObject* texObject,
       const Point2I& in_rAt,
       const RectI&   in_rSubRegion,
       const U32      in_flip = GFlip_None);
    /// Draws a stretched sub region of a texture
    /// @param texObject texture object to be drawn
    /// @param in_rStretch rectangle where the texture object will be drawn
    /// @param in_rSubRegion sub region of the texture that will be applied over the stretch region of the screen
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    virtual void DrawBitmapStretchSR(TextureObject* texObject,
       const RectI&   in_rStretch,
       const RectI&   in_rSubRegion,
       const U32      in_flip = GFlip_None,
       F32			    fSpin = 0.0f,
       bool           bSilhouette = false);
    /// @}
    /// @defgroup dgl_text Text Functions
    /// @ingroup dgl
    /// These functions draw a string on the string with a given font
    /// @{

    /// Draws text at a location in 2d gui coordinates
    /// Also supports color tags to modulate the given text
    /// @returns the number of x-pixels traversed
    /// @param font font to draw with, usually found in the profile
    /// @param ptDraw point where to start drawing text
    /// @param in_string string to be drawn
    /// @param colorTable lookups for the color tags
    /// @param maxColorIndex size of the colorTable
    /// @param rot rotation of text
    U32 DrawText(GFont *font, const Point2I &ptDraw, const UTF16 *in_string, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);
    /// Converts UTF8 text to UTF16, and calls the UTF16 version of dglDrawText
    U32 DrawText(GFont *font, const Point2I &ptDraw, const UTF8 *in_string, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);
    /// Draws "n" number of characters from the string, in_string
    /// @returns the number of x-pixels traversed
    /// @see dglDrawText
    U32 DrawTextN(GFont *font, const Point2I &ptDraw, const UTF16 *in_string, U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);
    /// Converts UTF8 text to UTF16, and calls the UTF16 version of dglDrawTextN
    U32 DrawTextN(GFont *font, const Point2I &ptDraw, const UTF8  *in_string, U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);
    /// @}
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
    // Drawing primitives

    /// @defgroup dgl_primitive Primitive Drawing
    /// @ingroup dgl
    /// Easy functions for drawing lines and (un)textured rectangles in 2d or 3d space
    /// @{

    /// draws a line from x1,y1 to x2,y2 in the specified color
    virtual void DrawLine(S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color);
    /// draws a line from startPt to endPt in specified color
    virtual void DrawLine(const Point2I &startPt, const Point2I &endPt, const ColorI &color);
    /// draws an UNTEXTURED filled triangle with the three points which should be given in counterclockwise order
    virtual void DrawTriangleFill(const Point2I& pt1, const Point2I& pt2, const Point2I& pt3, const ColorI& color);
    /// draws a wireframe rectangle from upperL to lowerR in specified color and optional line width
    virtual void DrawRect(const Point2I &upperL, const Point2I &lowerR, const ColorI &color, const float &lineWidth = 1.0f);
    /// draws a wireframe rectangle in "rect" in specified color and optional line width
    virtual void DrawRect(const RectI &rect, const ColorI &color, const float &lineWidth = 1.0f);
    /// draws an UNTEXTURED filled rectangle from upperL to lowerR in specified color
    virtual void DrawRectFill(const Point2I &upperL, const Point2I &lowerR, const ColorI &color);
    /// draws an UNTEXTURED filled rectangle in "rect" in specified color
    virtual void DrawRectFill(const RectI &rect, const ColorI &color);
    /// draws an UNTEXTURED filled quad in specified color
    virtual void DrawQuadFill(const Point2I &point1, const Point2I &point2, const Point2I &point3, const Point2I &point4, const ColorI &color);
    /// draws a square, with center point "screenPoint", width of "width" on an angle of "spinAngle" in 2d
    virtual void Draw2DSquare(const Point2F &screenPoint, F32 width, F32 spinAngle);
    /// draws a square, with center point "position", width of "width" on an angle of "spinAngle" in 3d
    virtual void DrawBillboard(const Point3F &position, F32 width, F32 spinAngle);
    /// Draws a wireframe cube around "center" with size "extent"
    virtual void WireCube(const Point3F &extent, const Point3F &center);
    /// Draws a solid cube around "center" with size "extent"
    virtual void SolidCube(const Point3F &extent, const Point3F & enter);
    /// Draws an unfilled circle using line segments
    virtual void DrawCircle(const Point2I &center, const F32 radius, const ColorI &color, const F32 &lineWidth = 1.0f);
    /// Draws a filled circle
    virtual void DrawCircleFill(const Point2I &center, const F32 radius, const ColorI &color);
    /// @}
    //------------------------------------------------------------------------------------------
    // RENDER FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void EnableState(DGLRenderState rs) = 0;
    virtual void DisableState(DGLRenderState rs) = 0;
    virtual void EnableClientState(DGLClientState cs) = 0;
    virtual void DisableClientState(DGLClientState cs) = 0;
    virtual void setBlendFunc(DGLBlend sFactor, DGLBlend dFactor) = 0;
    virtual void SetOrthoState(F32 wMin, F32 wMax, F32 hMin, F32 hMax, F32 mNear, U32 mFar) = 0;
    virtual void SetVertexPoint(U32 size, U32 stride, const void *pointer) = 0;
    virtual void ClearBuffer(DGLBufferBit bit) = 0;
    virtual void ClearColor(F32 r, F32 g, F32 b, F32 a) = 0;
    virtual void ScissorTest(S32 x, S32 y, S32 width, S32 height) = 0;
    virtual void SetColorF(F32 r, F32 g, F32 b, F32 a) = 0;
    virtual void SetColorI(U32 r, U32 g, U32 b, U32 a) = 0;
    virtual void SetLineWidth(F32 width) = 0;

    //------------------------------------------------------------------------------------------
    // DRAW ARRAY FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void DrawArrays(DGLPrimitiveType type, U32 first, U32 count) = 0;

    //------------------------------------------------------------------------------------------
    // TEXTURE FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void AreTexturesLoaded(S32 size, const U32* addr, bool isLoad) = 0;
    virtual void LoadTexture(U32 n, U32 glName) = 0;
    virtual void DeleteTextures(U32 n, const U32* glName) = 0;

    //------------------------------------------------------------------------------------------
    // MATRIX FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void setMatrix(DGLMatrixType type) = 0;
    /// loads matrix "m" into the current matrix mode
    virtual void LoadMatrix(const MatrixF *m) = 0;
    /// Set model view matrix
    virtual void SetModelViewMatrix() = 0;
    /// Set projection matrix
    virtual void SetProjMatrix() = 0;
    /// multiplies the current transformation matrix by matrix "m"
    virtual void MultMatrix(const MatrixF *m) = 0;
    /// returns the current modelview matrix
    virtual void GetModelview(MatrixF *m) = 0;
    /// returns the current projection matrix
    virtual void GetProjection(MatrixF *m) = 0;
    /// @}
    virtual void PopMatrix() = 0;
    virtual void PushMatrix() = 0;
    virtual void LoadIdentity() = 0;
    //------------------------------------------------------------------------------------------
    // CAMERA FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    // Variables 
    static F64    frustLeft, frustRight, frustBottom, frustTop, frustNear, frustFar;
    static RectI  viewPort;
    static F32    pixelScale;
    static F32    worldToScreenScale;
    static bool   isOrtho;
    RectI         sgCurrentClipRect;

    /// Get the pixel scale
    F32 GetPixelScale() { return pixelScale; }
    /// The scale factor of a world unit to a screen unit
    F32 GetWorldToScreenScale() { return worldToScreenScale; }
    /// Returns the screen length of a line of distance "radius" that is "dist" units away from the camera that is perpendicular to the line of sight, namely:
    F32 ProjectRadius(F32 dist, F32 radius) { return (radius / dist) * worldToScreenScale; }
    /// sets the viewport for the window
    virtual void SetViewport(const RectI &aViewPort) = 0;
    /// gets the current viewport of the window
    void GetViewport(RectI* outViewport);
    /// Sets the viewing frustrum.  This effectively creates the view volume and sets up the 6 clipping planes (near, far, left, right, top, bottom)
    virtual void SetFrustum(F64 left, F64 right, F64 bottom, F64 top, F64 nearDist, F64 farDist, bool ortho = false) = 0;
    /// Returns the parameters for the current viewing frustrum
    virtual void GetFrustum(F64 *left, F64 *right, F64 *bottom, F64 *top, F64 *nearDist, F64 *farDist) = 0;
    /// returns whether or not the coordinate system is orthographic (if it is not projected)
    bool IsOrtho() { return isOrtho; }
    /// Sets up an orthographical viewport and clipping region.  This is best used for guis
    /// @param clipRect The bounds of the coordinate system
    virtual void SetClipRect(const RectI &clipRect) = 0;
    /// Gets the last clip rect specified by a call to dglSetClipRect
    const RectI& GetClipRect() { return sgCurrentClipRect; }
    /// @}

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
    // Misc
    /// Projects a point on the screen in 3d space into a point on the screen
    /// @returns true if the point is on the screen, false if not and screenPoint will be (0,0,0)
    bool  PointToScreen(Point3F &point3D, Point3F &screenPoint);

    //--------------------------------------------------------------------------
    // Debug functions
    /// Checks to see if all states are "canonical"
    /// @see dglSetCanonicalState
    virtual bool IsInCanonicalState();

    /// Sets states to a "canonical" state
    /// @note a "canonical" state is described as:
    ///
    ///  BLEND disabled
    ///
    ///  TEXTURE_2D disabled on both texture units.
    ///
    ///  ActiveTexture set to 0
    ///
    ///  LIGHTING off
    ///
    ///  winding : clockwise ?
    ///
    ///  cullface : disabled
    virtual void SetCanonicalState();

    /// Gets the current state of all transformation matrices
    /// @param mvDepth Number of "pushes" made to the modelview matrix without balancing "pops"
    /// @param pDepth  Number of "pushes" made to the projection matrix without balancing "pops"
    /// @param t0Depth  Number of "pushes" made to the texture 0 matrix without balancing "pops"
    /// @param t0Matrix The current texture 0 matrix, should be a 4-element array
    /// @param t1Depth  Number of "pushes" made to the texture 1 matrix without balancing "pops"
    /// @param t1Matrix The current texture 1 matrix, should be a 4-element array
    /// @param vp The current viewport, should be a 4-element array
    /// @see dglCheckState
    virtual void GetTransformState(S32* mvDepth,
       S32* pDepth,
       S32* t0Depth,
       F32* t0Matrix,
       S32* t1Depth,
       F32* t1Matrix,
       S32* vp);

    /// Checks to see that the given information matches the current transform state
    /// @see dglGetTransformState
    virtual bool CheckState(const S32 mvDepth, const S32 pDepth,
       const S32 t0Depth, const F32* t0Matrix,
       const S32 t1Depth, const F32* t1Matrix,
       const S32* vp);

};

//------------------------------------------------------------------------------
inline bool DGLDevice::setResolution(U32 width, U32 height, U32 bpp)
{
   return setScreenMode(width, height, bpp, smIsFullScreen);
}


//------------------------------------------------------------------------------
inline bool DGLDevice::toggleFullScreen()
{
   return setScreenMode(smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp, !smIsFullScreen);
}


//------------------------------------------------------------------------------
inline DGLVideoMode DGLDevice::getResolution()
{
   return smCurrentRes;
}


//------------------------------------------------------------------------------
inline bool DGLDevice::isFullScreen()
{
   return smIsFullScreen;
}

#endif // _H_DGL


