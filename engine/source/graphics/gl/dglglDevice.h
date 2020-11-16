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
   virtual void initDevice();
   bool activate(U32 width, U32 height, U32 bpp, bool fullScreen);
   void shutdown();
   void destroy();
   bool setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true);
   void swapBuffers();
   const char* getDriverInfo();
   bool getGammaCorrection(F32 &g);
   bool setGammaCorrection(F32 g);
   bool getVerticalSync();
   bool setVerticalSync(bool on);
   void initGLstate();
   virtual void create();
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
   virtual void EnableState(DGLRenderState rs);
   virtual void DisableState(DGLRenderState rs);
   virtual void EnableClientState(DGLClientState cs);
   virtual void DisableClientState(DGLClientState cs);
   virtual void setBlendFunc(DGLBlend sFactor, DGLBlend dFactor);
   virtual void SetOrthoState(F32 wMin, F32 wMax, F32 hMin, F32 hMax, F32 mNear, U32 mFar);
   virtual void SetVertexPoint(U32 size, U32 stride, const void *pointer);
   virtual void ClearBuffer(DGLBufferBit bit);
   virtual void ClearColor(F32 r, F32 g, F32 b, F32 a);
   virtual void ScissorTest(S32 x, S32 y, S32 width, S32 height);
   virtual void SetColorF(F32 r, F32 g, F32 b, F32 a);
   virtual void SetColorI(U32 r, U32 g, U32 b, U32 a);
   virtual void SetLineWidth(F32 width);

   //-------------------------------------------------------------------------------
   // TEXTURE
   //-------------------------------------------------------------------------------
   virtual void AreTexturesLoaded(S32 size, const U32* addr, bool isLoad);
   virtual void LoadTexture(U32 n, U32 glName);
   virtual void DeleteTextures(U32 n, const U32* glName);

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
   virtual void DrawArrays(DGLPrimitiveType type, U32 first, U32 count);

};


#endif // !_DGLGLDEVICE_H_
