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
    DGLDevice();
    virtual void init();

    static DGLDevice *get() { return smDGL; }

 private:
    static DGLDevice * smDGL;

 public:
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
    void DrawBitmap(TextureObject* texObject,
       const Point2I& in_rAt,
       const U32      in_flip = GFlip_None);
    /// Draws a power-of-two bitmap that is tiled. Requires a POT bitmap to draw properly.
    /// @param texObject texture object to be drawn
    /// @param in_rTile rectangle where the texture will be drawn in 2d coordinates
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    void DrawBitmapTile(TextureObject* texObject,
       const RectI&   in_rTile,
       const U32      in_flip = GFlip_None,
       F32			 fSpin = 0.0f,
       bool           bSilhouette = false);
    /// Draws a bitmap that is stretched
    /// @param texObject texture object to be drawn
    /// @param in_rStretch rectangle where the texture will be drawn in 2d coordinates
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    void DrawBitmapStretch(TextureObject* texObject,
       const RectI&   in_rStretch,
       const U32      in_flip = GFlip_None,
       F32			 fSpin = 0.0f,
       bool           bSilhouette = false);
    /// Draws a sub region of a texture
    /// @param texObject texture object to be drawn
    /// @param in_rAt point where the texture is to be drawn
    /// @param in_rSubRegion portion of the texture to be drawn
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    void DrawBitmapSR(TextureObject* texObject,
       const Point2I& in_rAt,
       const RectI&   in_rSubRegion,
       const U32      in_flip = GFlip_None);
    /// Draws a stretched sub region of a texture
    /// @param texObject texture object to be drawn
    /// @param in_rStretch rectangle where the texture object will be drawn
    /// @param in_rSubRegion sub region of the texture that will be applied over the stretch region of the screen
    /// @param in_flip enumerated constant representing any flipping to be done about the x and/or y axis
    void DrawBitmapStretchSR(TextureObject* texObject,
       const RectI&   in_rStretch,
       const RectI&   in_rSubRegion,
       const U32      in_flip = GFlip_None,
       F32			   fSpin = 0.0f,
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
    void DrawLine(S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color);
    /// draws a line from startPt to endPt in specified color
    void DrawLine(const Point2I &startPt, const Point2I &endPt, const ColorI &color);
    /// draws an UNTEXTURED filled triangle with the three points which should be given in counterclockwise order
    void DrawTriangleFill(const Point2I& pt1, const Point2I& pt2, const Point2I& pt3, const ColorI& color);
    /// draws a wireframe rectangle from upperL to lowerR in specified color and optional line width
    void DrawRect(const Point2I &upperL, const Point2I &lowerR, const ColorI &color, const float &lineWidth = 1.0f);
    /// draws a wireframe rectangle in "rect" in specified color and optional line width
    void DrawRect(const RectI &rect, const ColorI &color, const float &lineWidth = 1.0f);
    /// draws an UNTEXTURED filled rectangle from upperL to lowerR in specified color
    void DrawRectFill(const Point2I &upperL, const Point2I &lowerR, const ColorI &color);
    /// draws an UNTEXTURED filled rectangle in "rect" in specified color
    void DrawRectFill(const RectI &rect, const ColorI &color);
    /// draws an UNTEXTURED filled quad in specified color
    void DrawQuadFill(const Point2I &point1, const Point2I &point2, const Point2I &point3, const Point2I &point4, const ColorI &color);
    /// draws a square, with center point "screenPoint", width of "width" on an angle of "spinAngle" in 2d
    void Draw2DSquare(const Point2F &screenPoint, F32 width, F32 spinAngle);
    /// draws a square, with center point "position", width of "width" on an angle of "spinAngle" in 3d
    void DrawBillboard(const Point3F &position, F32 width, F32 spinAngle);
    /// Draws a wireframe cube around "center" with size "extent"
    void WireCube(const Point3F &extent, const Point3F &center);
    /// Draws a solid cube around "center" with size "extent"
    void SolidCube(const Point3F &extent, const Point3F & enter);
    /// Draws an unfilled circle using line segments
    void DrawCircle(const Point2I &center, const F32 radius, const ColorI &color, const F32 &lineWidth = 1.0f);
    /// Draws a filled circle
    void DrawCircleFill(const Point2I &center, const F32 radius, const ColorI &color);
    /// @}
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
    // Matrix functions

    /// @defgroup dgl_matrix Matrix Functions
    /// @ingroup dgl
    /// These functions manipulate the current matrix.  The current matrix could be modelivew, projection, or texture
    /// @note Make sure you specify which matrix you want to manipulate with a call to glMatrixMode(enum matrix); before calling dglLoadMatrix() or dglMultMatrix()
    /// @{

    /// loads matrix "m" into the current matrix mode
    void LoadMatrix(const MatrixF *m);

    void SetModelViewMatrix();
    void SetProjMatrix();
    /// multiplies the current transformation matrix by matrix "m"
    void MultMatrix(const MatrixF *m);
    /// returns the current modelview matrix
    void GetModelview(MatrixF *m);
    /// returns the current projection matrix
    void GetProjection(MatrixF *m);
    /// @}

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
    // Camera functions
    /// @defgroup dgl_camera_func Camera functions
    /// @ingroup dgl
    /// @{

    /// Returns the pixel scale, namely:
    ///
    /// viewPort.extent.x / 640.0
    F32 GetPixelScale();
    /// The scale factor of a world unit to a screen unit
    F32 GetWorldToScreenScale();
    /// Returns the screen length of a line of distance "radius" that is "dist" units away from the camera that is perpendicular to the line of sight, namely:
    ///
    /// (radius / dist) * worldToScreenScale
    F32 ProjectRadius(F32 dist, F32 radius);
    /// @}

    /// @defgroup dgl_view Viewing Volume Functions
    /// @ingroup dgl
    /// These functions set up the view cube of the window.
    /// @{

    /// sets the viewport for the window
    void SetViewport(const RectI &aViewPort);
    /// gets the current viewport of the window
    void GetViewport(RectI* outViewport);
    /// Sets the viewing frustrum.  This effectively creates the view volume and sets up the 6 clipping planes (near, far, left, right, top, bottom)
    /// @param left This is the position of the left vertical clipping plane
    /// @param right This is the position of the right vertical clipping plane
    /// @param top This is the position of the top horizontal clipping plane
    /// @param bottom This is the position of the bottom horizontal clipping plane
    /// @param nearDist This is the distance between the eye and the near clipping plane
    /// @param farDist This is the distance between the eye and the far clipping plane
    /// @param ortho (optional, default is false) If left false, calling this function will create a projection viewing volume.  If true, it will be orthographic
    void SetFrustum(F64 left, F64 right, F64 bottom, F64 top, F64 nearDist, F64 farDist, bool ortho = false);
    /// Returns the parameters for the current viewing frustrum
    /// @see dglSetFrustrum
    void GetFrustum(F64 *left, F64 *right, F64 *bottom, F64 *top, F64 *nearDist, F64 *farDist);
    /// returns whether or not the coordinate system is orthographic (if it is not projected)
    bool IsOrtho();
    /// Sets up an orthographical viewport and clipping region.  This is best used for guis
    /// @param clipRect The bounds of the coordinate system
    void SetClipRect(const RectI &clipRect);
    /// Gets the last clip rect specified by a call to dglSetClipRect
    /// @see dglSetClipRect
    const RectI& GetClipRect();
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
    bool IsInCanonicalState();

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
    void SetCanonicalState();

    /// Gets the current state of all transformation matrices
    /// @param mvDepth Number of "pushes" made to the modelview matrix without balancing "pops"
    /// @param pDepth  Number of "pushes" made to the projection matrix without balancing "pops"
    /// @param t0Depth  Number of "pushes" made to the texture 0 matrix without balancing "pops"
    /// @param t0Matrix The current texture 0 matrix, should be a 4-element array
    /// @param t1Depth  Number of "pushes" made to the texture 1 matrix without balancing "pops"
    /// @param t1Matrix The current texture 1 matrix, should be a 4-element array
    /// @param vp The current viewport, should be a 4-element array
    /// @see dglCheckState
    void GetTransformState(S32* mvDepth,
       S32* pDepth,
       S32* t0Depth,
       F32* t0Matrix,
       S32* t1Depth,
       F32* t1Matrix,
       S32* vp);

    /// Checks to see that the given information matches the current transform state
    /// @see dglGetTransformState
    bool CheckState(const S32 mvDepth, const S32 pDepth,
       const S32 t0Depth, const F32* t0Matrix,
       const S32 t1Depth, const F32* t1Matrix,
       const S32* vp);


    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
    // Advanced hardware functionality.

#if defined TORQUE_OS_IOS || defined TORQUE_OS_ANDROID

    //Luma: Add some additional commands here so that it works in all cases
    extern GLfloat gVertexFloats[8];
    extern GLfloat gTextureVerts[8];
    // uses memory insted of heap memory. PUAP optimization
#define DrawTextureQuadiPhone( vX1, vY1, vX2, vY2, vX3, vY3, vX4, vY4, tX1, tY1, tX2, tY2, tX3, tY3, tX4, tY4 ) \
gVertexFloats[0] = (GLfloat)(vX1);							\
gVertexFloats[1] = (GLfloat)(vY1);							\
gVertexFloats[2] = (GLfloat)(vX2);							\
gVertexFloats[3] = (GLfloat)(vY2);							\
gVertexFloats[4] = (GLfloat)(vX3);							\
gVertexFloats[5] = (GLfloat)(vY3);							\
gVertexFloats[6] = (GLfloat)(vX4);							\
gVertexFloats[7] = (GLfloat)(vY4);							\
gTextureVerts[0] = (GLfloat)(tX1);							\
gTextureVerts[1] = (GLfloat)(tY1);							\
gTextureVerts[2] = (GLfloat)(tX2);							\
gTextureVerts[3] = (GLfloat)(tY2);							\
gTextureVerts[4] = (GLfloat)(tX3);							\
gTextureVerts[5] = (GLfloat)(tY3);							\
gTextureVerts[6] = (GLfloat)(tX4);							\
gTextureVerts[7] = (GLfloat)(tY4);							\
glDisableClientState(GL_COLOR_ARRAY);						\
glDisableClientState(GL_POINT_SIZE_ARRAY_OES);				\
glEnableClientState(GL_VERTEX_ARRAY);						\
glEnableClientState(GL_TEXTURE_COORD_ARRAY);				\
glVertexPointer(2, GL_FLOAT, 0, gVertexFloats);				\
glTexCoordPointer(2, GL_FLOAT, 0, gTextureVerts);			\
glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#endif

};

#endif // _H_DGL


