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

#ifndef _DGLADAPTER_H_
#include "graphics/dglAdapter.h"
#endif // !_DGLADAPTER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

class TextureObject;
class GFont;
class MatrixF;
class RectI;
class ColorI;
class ColorF;
class Point2I;
class Point2F;
class Point3F;

 enum GFlipConstants
 {
    GFlip_None = 0,
    GFlip_X = BIT(0),
    GFlip_Y = BIT(1),
    GFlip_XY = GFlip_X | GFlip_Y
 };

 extern signed int gFSAASamples;

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

    static Vector<DGLDevice *>   smDeviceList;
    static DGLDevice*            smCurrentDevice;
    static bool                  smCritical;

 protected:

    Vector<DGLVideoMode>         mVideoModeList;
    bool                         mFullScreenOnly;
    static DGLVideoMode          smCurrentRes;
    static bool                  smIsFullScreen;
    MatrixF mProjectionMatrix;
    /// The global vsync state.
    static bool smDisableVSync;

 public:

    static bool smNeedResurrect;
    static bool smEdgeClamp;

    const char* mDeviceName;

    DGLDevice();

    static DGLDevice *get() { return smCurrentDevice; }


    static void init();
    static void destroy();                               // clean up and shut down
    static bool installDevice(DGLDevice *dev);
    static bool setDevice(const char *renderName,
       U32 width, U32 height,
       U32 bpp, bool fullScreen);    // set the current display device

    virtual bool activate(U32 width, U32 height, U32 bpp, bool fullScreen) = 0;

    virtual void shutdown() = 0;
    virtual bool setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true) = 0;
    virtual bool setResolution(U32 width, U32 height, U32 bpp);
    virtual bool toggleFullScreen();
    //virtual DGLDevice* create() = 0;

    bool prevRes();
    bool nextRes();
    virtual void enumerateVideoModes() = 0;
    const char* getResolutionList();
    bool isFullScreenOnly() { return(mFullScreenOnly); }

    static DGLVideoMode getResolution();
    static bool       isFullScreen();


    void SetBitmapModulation(const ColorF& in_rColor);
    void GetBitmapModulation(ColorF* color);
    void GetBitmapModulation(ColorI* color);
    void ClearBitmapModulation();

    void SetTextAnchorColor(const ColorF&);
    void DrawBlendBox(RectI & bounds, ColorF & c1, ColorF & c2, ColorF & c3, ColorF & c4);
    void DrawBlendRangeBox(RectI & bounds, bool vertical, U8 numColors, ColorI * colors);

    void DrawBitmap(TextureObject* texObject,
       const Point2I& in_rAt,
       const U32      in_flip = GFlip_None);

    void DrawBitmapTile(TextureObject* texObject,
       const RectI&   in_rTile,
       const U32      in_flip = GFlip_None,
       F32			    fSpin = 0.0f,
       bool           bSilhouette = false);

    void DrawBitmapStretch(TextureObject* texObject,
       const RectI&   in_rStretch,
       const U32      in_flip = GFlip_None,
       F32			    fSpin = 0.0f,
       bool           bSilhouette = false);

    void DrawBitmapSR(TextureObject* texObject,
       const Point2I& in_rAt,
       const RectI&   in_rSubRegion,
       const U32      in_flip = GFlip_None);

    void DrawBitmapStretchSR(TextureObject* texObject,
       const RectI&   in_rStretch,
       const RectI&   in_rSubRegion,
       const U32      in_flip = GFlip_None,
       F32			    fSpin = 0.0f,
       bool           bSilhouette = false);

    U32 DrawText(GFont *font, const Point2I &ptDraw, const UTF16 *in_string, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);

    U32 DrawText(GFont *font, const Point2I &ptDraw, const UTF8 *in_string, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);

    U32 DrawTextN(GFont *font, const Point2I &ptDraw, const UTF16 *in_string, U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);
    U32 DrawTextN(GFont *font, const Point2I &ptDraw, const UTF8  *in_string, U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f);

    //-------------------------------------------------------------------------------
    // Drawing primitives
    //-------------------------------------------------------------------------------
    void DrawLine(S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color);
    void DrawLine(const Point2I &startPt, const Point2I &endPt, const ColorI &color);
    void DrawTriangleFill(const Point2I& pt1, const Point2I& pt2, const Point2I& pt3, const ColorI& color);
    void DrawRect(const Point2I &upperL, const Point2I &lowerR, const ColorI &color, const float &lineWidth = 1.0f);
    void DrawRect(const RectI &rect, const ColorI &color, const float &lineWidth = 1.0f);
    void DrawRectFill(const Point2I &upperL, const Point2I &lowerR, const ColorI &color);
    void DrawRectFill(const RectI &rect, const ColorI &color);
    void DrawQuadFill(const Point2I &point1, const Point2I &point2, const Point2I &point3, const Point2I &point4, const ColorI &color);
    void Draw2DSquare(const Point2F &screenPoint, F32 width, F32 spinAngle);
    void DrawBillboard(const Point3F &position, F32 width, F32 spinAngle);
    void WireCube(const Point3F &extent, const Point3F &center);
    void SolidCube(const Point3F &extent, const Point3F & enter);
    void DrawCircle(const Point2I &center, const F32 radius, const ColorI &color, const F32 &lineWidth = 1.0f);
    void DrawCircleFill(const Point2I &center, const F32 radius, const ColorI &color);
    /// @}

    virtual void GetApiValue(DGLAPIValues inval, S32 &outVal) = 0;
    //------------------------------------------------------------------------------------------
    // RENDER FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void swapBuffers() = 0;
    virtual void EnableState(DGLRenderState rs) = 0;
    virtual void DisableState(DGLRenderState rs) = 0;
    virtual void EnableClientState(DGLClientState cs) = 0;
    virtual void DisableClientState(DGLClientState cs) = 0;
    virtual void setBlendFunc(DGLBlend sFactor, DGLBlend dFactor) = 0;
    virtual void setAlphaFunc(DGLCompare cmp, F32 testMode) = 0;
    virtual void SetOrthoState(F32 wMin, F32 wMax, F32 hMin, F32 hMax, F32 mNear, U32 mFar) = 0;
    virtual void SetVertexPoint(U32 size, U32 stride, const void *pointer) = 0;
    virtual void SetColorPoint(U32 size, U32 stride, const void * pointer) = 0;
    virtual void SetColorPointU(U32 size, U32 stride, const void * pointer) = 0;
    virtual void SetTexPoint(U32 size, U32 stride, const void * pointer) = 0;
    virtual void ClearBuffer(DGLBufferBit bit) = 0;
    virtual void ClearColor(F32 r, F32 g, F32 b, F32 a) = 0;
    virtual void ScissorTest(S32 x, S32 y, S32 width, S32 height) = 0;
    virtual void SetColorF(F32 r, F32 g, F32 b, F32 a) = 0;
    virtual void SetColorI(U32 r, U32 g, U32 b, U32 a) = 0;
    virtual void SetLineWidth(F32 width) = 0;
    virtual void SetPolygonMode(DGLPolyMode face, DGLPolyMode mode) = 0;
    virtual void SetTextureEnvironment(DGLTextureEnvironment target, DGLTextureEnvironmentPname name, DGLTextureEnvironment param) = 0;
    virtual void SetTextureEnvironmentF(DGLTextureEnvironment target, DGLTextureEnvironmentPname name, const F32 *val) = 0;
    virtual void SetRotate(F32 ang, F32 x, F32 y, F32 z) = 0;
    virtual void SetTranslate(F32 x, F32 y, F32 z) = 0;
    virtual void SetClipPlane(DGLRenderState clipPlane, const F64 *side) = 0;
    virtual void SetReadBuffer(DGLPolyMode poly) = 0;
    virtual void SetReadPixels(U32 posx, U32 posy, S32 width, S32 height, void * pixels) = 0;
    virtual void DrawBuffer(DGLPolyMode poly) = 0;

    //------------------------------------------------------------------------------------------
    // DRAW FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void DrawElements(DGLPrimitiveType type, U32 count, const void * buff) = 0;
    virtual void DrawArrays(DGLPrimitiveType type, U32 first, U32 count) = 0;

    //------------------------------------------------------------------------------------------
    // TEXTURE FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void AreTexturesLoaded(S32 size, const U32 &addr, bool &isLoad) = 0;
    virtual void LoadTexture(U32 n, U32 &glName) = 0;
    virtual void DeleteTextures(U32 n, const U32* glName) = 0;
    virtual void SetTextureParam(DGLTextureParam param, const DGLTextureFilter filter) = 0;
    virtual void BindTexture(U32 glName) = 0;
    virtual void UploadTexture16(U32 mip, DGLFormat inFmt, U32 width, U32 height, U32 b, U16 *bits) = 0;
    virtual void UploadTexture(U32 mip, DGLFormat inFmt, U32 width, U32 height, U32 b, U8 *bits) = 0;

    //------------------------------------------------------------------------------------------
    // MATRIX FUNCTIONS
    // Pass these off to the api's.
    //------------------------------------------------------------------------------------------
    virtual void setMatrix(DGLMatrixType type) = 0;
    virtual void LoadMatrix(const MatrixF *m) = 0;
    virtual void SetModelViewMatrix() = 0;
    virtual void SetProjMatrix() = 0;
    virtual void MultMatrix(const MatrixF *m) = 0;
    virtual void GetModelview(MatrixF *m) = 0;
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
    F32    pixelScale;
    F32    worldToScreenScale;
    RectI  sgCurrentClipRect;
    /// Get the pixel scale
    F32 GetPixelScale() { return pixelScale; }
    F32 GetWorldToScreenScale() { return worldToScreenScale; }
    F32 ProjectRadius(F32 dist, F32 radius) { return (radius / dist) * worldToScreenScale; }
    virtual void SetViewport(const RectI &aViewPort) = 0;
    virtual void GetViewport(RectI* outViewport) = 0;
    virtual void SetFrustum(F64 left, F64 right, F64 bottom, F64 top, F64 nearDist, F64 farDist, bool ortho = false) = 0;
    virtual void GetFrustum(F64 &left, F64 &right, F64 &bottom, F64 &top, F64 &nearDist, F64 &farDist) = 0;
    virtual bool IsOrtho() = 0;
    virtual void SetClipRect(const RectI &clipRect) = 0;
    const RectI& GetClipRect() { return sgCurrentClipRect; }
    /// @}


    bool  PointToScreen(Point3F &point3D, Point3F &screenPoint);

    //--------------------------------------------------------------------------
    // Debug functions
    //--------------------------------------------------------------------------
    virtual bool IsInCanonicalState() = 0;

    virtual void SetCanonicalState() = 0;

    virtual void GetTransformState(S32* mvDepth,
       S32* pDepth,
       S32* t0Depth,
       F32* t0Matrix,
       S32* t1Depth,
       F32* t1Matrix,
       S32* vp) = 0;

    virtual bool CheckState(const S32 mvDepth, const S32 pDepth,
       const S32 t0Depth, const F32* t0Matrix,
       const S32 t1Depth, const F32* t1Matrix,
       const S32* vp) = 0;

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


