#ifndef _DGLGLDEVICE_H_
#define _DGLGLDEVICE_H_

#include "graphics/dgl.h"

#include "platform/platform.h"

#include "graphics/gl/glad/gladGL.h"

class DGLGLDevice : public DGLDevice
{
private:
   U32         mMaxShaderTextures;
   void*       mContext;

protected:
   bool        mRestoreGamma;
   U16         mOriginalRamp[256 * 3];
   static bool smCanSwitchBitDepth;
   static bool smCanDo16Bit;
   static bool smCanDo32Bit;

public:

   struct GLCapabilities
   {
      bool anisotropicFiltering;
      bool bufferStorage;
      bool textureStorage;
      bool copyImage;
      bool vertexAttributeBinding;
      bool khrDebug;
      bool extDebugMarker;
   };
   GLCapabilities mCapabilities;

   //------------------------------------------------------------------------------
   // INIT
   //------------------------------------------------------------------------------
   DGLGLDevice();
   virtual bool activate(U32 width, U32 height, U32 bpp, bool fullScreen);
   void shutdown();
   void initGLstate();
   virtual bool IsInCanonicalState();
   virtual void SetCanonicalState();
   virtual bool CheckState(const S32 mvDepth, const S32 pDepth, const S32 t0Depth, const F32 * t0Matrix, const S32 t1Depth, const F32 * t1Matrix, const S32 * vp);
   virtual void GetTransformState(S32 * mvDepth, S32 * pDepth, S32 * t0Depth, F32 * t0Matrix, S32 * t1Depth, F32 * t1Matrix, S32 * vp);
   static DGLDevice* create();
   virtual bool setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint);
   virtual void enumerateVideoModes();

   //------------------------------------------------------------------------------
   // MATRIX
   //------------------------------------------------------------------------------
   virtual void setMatrix(DGLMatrixType type);
   virtual void LoadMatrix(const MatrixF *m);
   virtual void SetModelViewMatrix();
   virtual void SetProjMatrix();
   virtual void MultMatrix(const MatrixF *m);
   virtual void GetModelview(MatrixF *m);
   virtual void GetProjection(MatrixF *m);
   virtual void PopMatrix();
   virtual void PushMatrix();
   virtual void LoadIdentity();

   //------------------------------------------------------------------------------
   // RENDER
   //------------------------------------------------------------------------------
   virtual void swapBuffers();
   virtual void EnableState(DGLRenderState rs);
   virtual void DisableState(DGLRenderState rs);
   virtual void EnableClientState(DGLClientState cs);
   virtual void DisableClientState(DGLClientState cs);
   virtual void setBlendFunc(DGLBlend sFactor, DGLBlend dFactor);
   virtual void setAlphaFunc(DGLCompare cmp, F32 testMode);
   virtual void SetOrthoState(F32 wMin, F32 wMax, F32 hMin, F32 hMax, F32 mNear, U32 mFar);
   virtual void SetVertexPoint(U32 size, U32 stride, const void *pointer);
   virtual void SetColorPoint(U32 size, U32 stride, const void * pointer);
   virtual void SetColorPointU(U32 size, U32 stride, const void * pointer);
   virtual void SetTexPoint(U32 size, U32 stride, const void * pointer);
   virtual void ClearBuffer(DGLBufferBit bit);
   virtual void ClearColor(F32 r, F32 g, F32 b, F32 a);
   virtual void ScissorTest(S32 x, S32 y, S32 width, S32 height);
   virtual void SetColorF(F32 r, F32 g, F32 b, F32 a);
   virtual void SetColorI(U32 r, U32 g, U32 b, U32 a);
   virtual void SetLineWidth(F32 width);
   virtual void SetPolygonMode(DGLPolyMode face, DGLPolyMode mode);
   virtual void SetTextureEnvironment(DGLTextureEnvironment target, DGLTextureEnvironmentPname name, DGLTextureEnvironment param);
   virtual void SetTextureEnvironmentF(DGLTextureEnvironment target, DGLTextureEnvironmentPname name, const F32 *val);
   virtual void SetRotate(F32 ang, F32 x, F32 y, F32 z);
   virtual void SetTranslate(F32 x, F32 y, F32 z);
   virtual void SetClipPlane(DGLRenderState clipPlane, const F64 *side);
   virtual void SetReadBuffer(DGLPolyMode poly);
   virtual void SetReadPixels(U32 posx, U32 posy, S32 width, S32 height, void * pixels);
   virtual void DrawBuffer(DGLPolyMode poly);

   //-------------------------------------------------------------------------------
   // TEXTURE
   //-------------------------------------------------------------------------------
   virtual void AreTexturesLoaded(S32 size, const U32* addr, bool isLoad);
   virtual void LoadTexture(U32 n, U32 &glName);
   virtual void BindTexture(U32 glName);
   virtual void DeleteTextures(U32 n, const U32* glName);
   virtual void SetTextureParam(DGLTextureParam param, const DGLTextureFilter filter);
   virtual void UploadTexture16(U32 mip, DGLFormat inFmt, U32 width, U32 height, U32 b, U16 *bits);
   virtual void UploadTexture(U32 mip, DGLFormat inFmt, U32 width, U32 height, U32 b, U8 *bits);
   
   //-------------------------------------------------------------------------------
   // CAMERA
   //-------------------------------------------------------------------------------
   virtual void SetClipRect(const RectI &clipRect);
   virtual void SetFrustum(F64 left, F64 right, F64 bottom, F64 top, F64 nearPlane, F64 farPlane, bool ortho);
   virtual void GetFrustum(F64 *left, F64 *right, F64 *bottom, F64 *top, F64 *nearPlane, F64 *farPlane);
   virtual void SetViewport(const RectI &aViewPort);

   //------------------------------------------------------------------------------
   // Draw
   //------------------------------------------------------------------------------
   virtual void DrawElements(DGLPrimitiveType type, U32 count, const void * buff);
   virtual void DrawArrays(DGLPrimitiveType type, U32 first, U32 count);

   //------------------------------------------------------------------------------
   // MISC
   //------------------------------------------------------------------------------
   virtual void GetApiValue(DGLAPIValues inval, S32 &outVal);

};


#endif // !_DGLGLDEVICE_H_
