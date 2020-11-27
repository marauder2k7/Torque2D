#include "platform/platform.h"
#include "graphics/gl/dglGLDevice.h"
#include "platform/platformGL.h"

#include "graphics/gl/glEnumTranslate.h"

//------------------------------------------------------------------------------

namespace GLAD
{
   extern void gglPerformBinds();
   extern void gglPerformExtensionBinds(void *context);
}

//------------------------------------------------------------------------------

void loadGlCore()
{
   static bool loaded = false;
   if (loaded)
      return;

   loaded = true;

   GLAD::gglPerformBinds();
}

//------------------------------------------------------------------------------

void loadGlExtensions(void *context)
{
   static bool extLoaded = false;
   if (extLoaded)
      return;

   extLoaded = true;

   GLAD::gglPerformExtensionBinds(context);
}

//------------------------------------------------------------------------------

DGLGLDevice::DGLGLDevice() :
   mMaxShaderTextures(2),
   mContext(NULL)
{
   memset(&mCapabilities, 0, sizeof(GLCapabilities));
   mDeviceName = "OpenGL";
   loadGlCore();
   DGLGLEnumTranslate::init();
}

//------------------------------------------------------------------------------

void DGLGLDevice::initGLstate()
{
   glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&mMaxShaderTextures);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   Con::printf("  Extended Capabilities:");
   // Capabilities

   if (gglHasExtension(EXT_texture_filter_anisotropic))
   {
      Con::printf("  Anistropic Filtering");
      mCapabilities.anisotropicFiltering = true;
   }

   if (gglHasExtension(ARB_buffer_storage))
   {
      Con::printf("  Buffer Storage");
      mCapabilities.bufferStorage = true;
   }

   if (gglHasExtension(ARB_texture_storage))
   {
      Con::printf("  Texture Storage");
      mCapabilities.textureStorage = true;
   }

   if (gglHasExtension(ARB_copy_image))
   {
      Con::printf("  Copy Image");
      mCapabilities.copyImage = true;
   }

   if (gglHasExtension(ARB_vertex_attrib_binding))
   {
      Con::printf("  Vertex Attribute Binding");
      mCapabilities.vertexAttributeBinding = true;
   }

   if (gglHasExtension(KHR_debug))
   {
      mCapabilities.khrDebug = true;
   }

   if (gglHasExtension(EXT_debug_marker))
   {
      mCapabilities.extDebugMarker = true;
   }

   if (gglHasExtension(SGIS_texture_edge_clamp))
   {
      Con::printf("  Texture Clamping");
      smEdgeClamp = true;
   }

   PlatformGL::setVSync(smDisableVSync ? 0 : 1);

   //OpenGL 3 need a binded VAO for render
   //GLuint vao;
   //glGenVertexArrays(1, &vao);
   //glBindVertexArray(vao);

   //Luma:	Clear window at first, as it is showing previous gl color buffer stuff.
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   //enable sRGB
   //glEnable(GL_FRAMEBUFFER_SRGB);
}

//------------------------------------------------------------------------------
// DEBUG
//------------------------------------------------------------------------------

bool DGLGLDevice::IsInCanonicalState()
{
   bool ret = true;
   // Canonical state:
   //  BLEND disabled
   //  TEXTURE_2D disabled on both texture units.
   //  ActiveTexture set to 0
   //  LIGHTING off
   //  winding : clockwise ?
   //  cullface : disabled

   ret &= glIsEnabled(GL_BLEND) == GL_FALSE;
   ret &= glIsEnabled(GL_CULL_FACE) == GL_FALSE;
   GLint temp;

   if (gglHasExtension(ARB_multitexture)) 
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   } 
   else 
   {
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   }

   ret &= glIsEnabled(GL_LIGHTING) == GL_FALSE;

   ret &= glIsEnabled(GL_COLOR_ARRAY)         == GL_FALSE;
   ret &= glIsEnabled(GL_VERTEX_ARRAY)        == GL_FALSE;
   ret &= glIsEnabled(GL_NORMAL_ARRAY)        == GL_FALSE;
   if (gglHasExtension(EXT_fog_coord))
      ret &= glIsEnabled(GL_FOG_COORDINATE_ARRAY_EXT) == GL_FALSE;

   return ret;
}

void DGLGLDevice::SetCanonicalState()
{

   glDisable(GL_BLEND);
   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_ONE, GL_ZERO);
   glDisable(GL_LIGHTING);
   if (gglHasExtension(ARB_multitexture)) 
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   } 
   else 
   {
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   if (gglHasExtension(EXT_fog_coord))
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
}

bool DGLGLDevice::CheckState(const S32 mvDepth, const S32 pDepth,
   const S32 t0Depth, const F32* t0Matrix,
   const S32 t1Depth, const F32* t1Matrix,
   const S32* vp)
{
   GLint md, pd;
   RectI v;

   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,  &md);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &pd);

   GLint t0d, t1d;
   GLfloat t0m[16], t1m[16];
   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t0d);
   glGetFloatv(GL_TEXTURE_MATRIX, t0m);
   if (gglHasExtension(ARB_multitexture))
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t1d);
      glGetFloatv(GL_TEXTURE_MATRIX, t1m);
      glActiveTextureARB(GL_TEXTURE0_ARB);
   }
   else
   {
      t1d = 0;
      for (U32 i = 0; i < 16; i++)
         t1m[i] = 0;
   }

   GetViewport(&v);

   return ((md == mvDepth) &&
           (pd == pDepth) &&
           (t0d == t0Depth) &&
           (dMemcmp(t0m, t0Matrix, sizeof(F32) * 16) == 0) &&
           (t1d == t1Depth) &&
           (dMemcmp(t1m, t1Matrix, sizeof(F32) * 16) == 0) &&
           ((v.point.x  == vp[0]) &&
            (v.point.y  == vp[1]) &&
            (v.extent.x == vp[2]) &&
            (v.extent.y == vp[3])));

   return true;
}

void DGLGLDevice::GetTransformState(S32* mvDepth,
   S32* pDepth,
   S32* t0Depth,
   F32* t0Matrix,
   S32* t1Depth,
   F32* t1Matrix,
   S32* vp)
{
   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, (GLint*)mvDepth);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, (GLint*)pDepth);

   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t0Depth);
   glGetFloatv(GL_TEXTURE_MATRIX, t0Matrix);
   if (gglHasExtension(ARB_multitexture))
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t1Depth);
      glGetFloatv(GL_TEXTURE_MATRIX, t1Matrix);
      glActiveTextureARB(GL_TEXTURE0_ARB);
   }
   else
   {
      *t1Depth = 0;
      for (U32 i = 0; i < 16; i++)
         t1Matrix[i] = 0;
   }
   RectI v;
   GetViewport(&v);
   vp[0] = v.point.x;
   vp[1] = v.point.y;
   vp[2] = v.extent.x;
   vp[3] = v.extent.y;
}

//------------------------------------------------------------------------------
// DEBUG END
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CAMERA
//------------------------------------------------------------------------------

void DGLGLDevice::SetClipRect(const RectI &clipRect)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   U32 screenHeight = Platform::getWindowSize().y;
   glOrtho(clipRect.point.x, clipRect.point.x + clipRect.extent.x,
           clipRect.extent.y, 0,
           0, 1);

   glTranslatef(0.0f, (F32)-clipRect.point.y, 0.0f);

   SetModelViewMatrix();
   //glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glViewport(clipRect.point.x, screenHeight - (clipRect.point.y + clipRect.extent.y),
              clipRect.extent.x, clipRect.extent.y);

   sgCurrentClipRect = clipRect;

}

void DGLGLDevice::SetFrustum(F64 left, F64 right, F64 bottom, F64 top, F64 nearPlane, F64 farPlane, bool ortho)
{
   // this converts from a coord system looking down the pos-y axis
   // to ogl's down neg z axis.
   // it's stored in OGL matrix form
   static F32 darkToOGLCoord[16] = { 1, 0,  0, 0,
                                     0, 0, -1, 0,
                                     0, 1,  0, 0,
                                     0, 0,  0, 1 };

   frustLeft = left;
   frustRight = right;
   frustBottom = bottom;
   frustTop = top;
   frustNear = nearPlane;
   frustFar = farPlane;
   isOrtho = ortho;
   if (ortho)
   {
      glOrtho(left, right, bottom, top, nearPlane, farPlane);
      worldToScreenScale = F32(viewPort.extent.x / (frustRight - frustLeft));
   }
   else
   {
      glFrustum(left, right, bottom, top, nearPlane, farPlane);
      worldToScreenScale = F32((frustNear * viewPort.extent.x) / frustRight - frustLeft);
   }
   glMultMatrixf(darkToOGLCoord);
}

void DGLGLDevice::GetFrustum(F64 *left, F64 *right, F64 *bottom, F64 *top, F64 *nearPlane, F64 *farPlane)
{
   *left = frustLeft;
   *right = frustRight;
   *bottom = frustBottom;
   *top = frustTop;
   *nearPlane = frustNear;
   *farPlane = frustFar;
}

void DGLGLDevice::SetViewport(const RectI &aViewPort)
{
   viewPort = aViewPort;
   U32 screenHeight = Platform::getWindowSize().y;
   //glViewport(viewPort.point.x, viewPort.point.y + viewPort.extent.y,
   //           viewPort.extent.x, -viewPort.extent.y);

   glViewport(viewPort.point.x, screenHeight - (viewPort.point.y + viewPort.extent.y),
      viewPort.extent.x, viewPort.extent.y);
   pixelScale = viewPort.extent.x / (F32)MIN_RESOLUTION_X;
   worldToScreenScale = F32((frustNear * viewPort.extent.x) / (frustRight - frustLeft));
}

//------------------------------------------------------------------------------
// CAMERA END
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DRAW
//------------------------------------------------------------------------------

void DGLGLDevice::DrawElements(DGLPrimitiveType type, U32 count, const void* buff)
{
   glDrawElements(DGLGLPrimitiveType[type], count, GL_UNSIGNED_SHORT, buff);
}

void DGLGLDevice::DrawArrays(DGLPrimitiveType type, U32 first, U32 count)
{
   glDrawArrays(DGLGLPrimitiveType[type], first, count);
}

//------------------------------------------------------------------------------
// DRAW END
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// MATRIX
//------------------------------------------------------------------------------

void DGLGLDevice::setMatrix(DGLMatrixType type)
{
   glMatrixMode(DGLGLMatrixMode[type]);
}

void DGLGLDevice::LoadMatrix(const MatrixF *m)
{
   //F32 mat[16];
   //m->transposeTo(mat);
   const_cast<MatrixF*>(m)->transpose();
   glLoadMatrixf(*m);
   const_cast<MatrixF*>(m)->transpose();
}

void DGLGLDevice::SetModelViewMatrix()
{
   glMatrixMode(GL_MODELVIEW);
}

void DGLGLDevice::SetProjMatrix()
{
   glMatrixMode(GL_PROJECTION);
}

void DGLGLDevice::MultMatrix(const MatrixF *m)
{
   //F32 mat[16];
   //m->transposeTo(mat);
   //const F32* mp = *m;
   //Con::errorf(ConsoleLogEntry::General, "Mult: %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",
   // mp[0],
   // mp[1],
   // mp[2],
   // mp[3],
   // mp[4],
   // mp[5],
   // mp[6],
   // mp[7],
   // mp[8],
   // mp[9],
   // mp[10],
   // mp[11],
   // mp[12],
   // mp[13],
   // mp[14],
   // mp[15]);

   const_cast<MatrixF*>(m)->transpose();
   glMultMatrixf(*m);
   const_cast<MatrixF*>(m)->transpose();
}

void DGLGLDevice::GetModelview(MatrixF *m)
{
   glGetFloatv(GL_MODELVIEW_MATRIX, *m);
   m->transpose();
}

void DGLGLDevice::GetProjection(MatrixF *m)
{
   glGetFloatv(GL_PROJECTION_MATRIX, *m);
   m->transpose();
}

void DGLGLDevice::PopMatrix()
{
   glPopMatrix();
}

void DGLGLDevice::PushMatrix()
{
   glPushMatrix();
}

void DGLGLDevice::LoadIdentity()
{
   glLoadIdentity();
}

//------------------------------------------------------------------------------
// MATRIX END
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RENDER
//------------------------------------------------------------------------------

void DGLGLDevice::EnableState(DGLRenderState rs)
{
   glEnable(DGLGLRenderState[rs]);
}

void DGLGLDevice::DisableState(DGLRenderState rs)
{
   glDisable(DGLGLRenderState[rs]);
}

void DGLGLDevice::EnableClientState(DGLClientState cs)
{
   glEnableClientState(DGLGLClientState[cs]);
}

void DGLGLDevice::DisableClientState(DGLClientState cs)
{
   glDisableClientState(DGLGLClientState[cs]);

}

void DGLGLDevice::setBlendFunc(DGLBlend sFactor, DGLBlend dFactor)
{
   glBlendFunc(DGLGLBlend[sFactor], DGLGLBlend[dFactor]);
}

void DGLGLDevice::setAlphaFunc(DGLCompare cmp, F32 testMode)
{
   glAlphaFunc(DGLGLCompare[cmp], testMode);
}

void DGLGLDevice::SetOrthoState(F32 wMin, F32 wMax, F32 hMin, F32 hMax, F32 mNear, U32 mFar)
{
   glOrtho(wMin, wMax, hMin, hMax, mNear, mFar);
}

void DGLGLDevice::SetVertexPoint(U32 size, U32 stride, const void * pointer)
{
   // We only use float here as this is all that is used in the engine.
   // this function should be removed asap when the shaders are implemented.
   glVertexPointer(size, GL_FLOAT, stride, pointer);
}

void DGLGLDevice::SetColorPoint(U32 size, U32 stride, const void * pointer)
{
   // We only use float here as this is all that is used in the engine.
   // this function should be removed asap when the shaders are implemented.
   glColorPointer(size, GL_FLOAT, stride, pointer);
}

void DGLGLDevice::SetColorPointU(U32 size, U32 stride, const void * pointer)
{
   // We only use float here as this is all that is used in the engine.
   // this function should be removed asap when the shaders are implemented.
   glColorPointer(size, GL_UNSIGNED_BYTE, stride, pointer);
}

void DGLGLDevice::SetTexPoint(U32 size, U32 stride, const void * pointer)
{
   // We only use float here as this is all that is used in the engine.
   // this function should be removed asap when the shaders are implemented.
   glTexCoordPointer(size, GL_FLOAT, stride, pointer);
}

void DGLGLDevice::ClearBuffer(DGLBufferBit bit)
{
   glClear(DGLGLBufferBit[bit]);
}

void DGLGLDevice::ClearColor(F32 r, F32 g, F32 b, F32 a)
{
   glClearColor(r, g, b, a);
}

void DGLGLDevice::ScissorTest(S32 x, S32 y, S32 width, S32 height)
{
   glScissor(x, y, width, height);
}

void DGLGLDevice::SetColorF(F32 r, F32 g, F32 b, F32 a)
{
   glColor4f(r, g, b, a);
}

void DGLGLDevice::SetColorI(U32 r, U32 g, U32 b, U32 a)
{
   glColor4ub(r, g, b, a);
}

void DGLGLDevice::SetLineWidth(F32 width)
{
   glLineWidth(width);
}

void DGLGLDevice::SetPolygonMode(DGLPolyMode face, DGLPolyMode mode)
{
   glPolygonMode(DGLGLPolyMode[face], DGLGLPolyMode[mode]);
}

void DGLGLDevice::SetTextureEnvironment(DGLTextureEnvironment target, DGLTextureEnvironmentPname name, DGLTextureEnvironment param)
{
   glTexEnvi(DGLGLTextureEnvironment[target], DGLGLTextureEnvironmentPname[name], DGLGLTextureEnvironment[param]);
}

void DGLGLDevice::SetTextureEnvironmentF(DGLTextureEnvironment target, DGLTextureEnvironmentPname name, const F32 * val)
{
   glTexEnvfv(DGLGLTextureEnvironment[target], DGLGLTextureEnvironmentPname[name], val);
}

void DGLGLDevice::SetRotate(F32 ang, F32 x, F32 y, F32 z)
{
   glRotatef(ang, x, y, z);
}

void DGLGLDevice::SetTranslate(F32 x, F32 y, F32 z)
{
   glTranslatef(x, y, z);
}

void DGLGLDevice::SetClipPlane(DGLRenderState clipPlane, const F64 * side)
{
   glClipPlane(DGLGLRenderState[clipPlane], side);
}

void DGLGLDevice::SetReadBuffer(DGLPolyMode poly)
{
   glReadBuffer(DGLGLPolyMode[poly]);
}

void DGLGLDevice::SetReadPixels(U32 posx, U32 posy, S32 width, S32 height, void * pixels)
{
   glReadPixels(posx, posy, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

void DGLGLDevice::DrawBuffer(DGLPolyMode poly)
{
   glDrawBuffer(DGLGLPolyMode[poly]);
}


//------------------------------------------------------------------------------
// RENDER END
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TEXTURE
//------------------------------------------------------------------------------

void DGLGLDevice::AreTexturesLoaded(S32 size, const U32* addr, bool isLoad)
{
   glAreTexturesResident(size, addr, (GLboolean*)isLoad);

}

void DGLGLDevice::LoadTexture(U32 n, U32 &glName)
{
   glGenTextures(n, &glName);
}

void DGLGLDevice::BindTexture(U32 glName)
{
   glBindTexture(GL_TEXTURE_2D, glName);
}

void DGLGLDevice::DeleteTextures(U32 n, const U32* glName)
{
   glDeleteTextures(n, glName);
}

void DGLGLDevice::SetTextureParam(DGLTextureParam param, const DGLTextureFilter filter)
{
   // we will only use 2d textures...... i think
   glTexParameteri(GL_TEXTURE_2D, DGLGLTextureParam[param], DGLGLTextureFilter[filter]);
}

void DGLGLDevice::UploadTexture16(U32 mip, DGLFormat inFmt, U32 width, U32 height, U32 b, U16 *bits)
{
   glTexImage2D(GL_TEXTURE_2D, mip, DGLGLInternalFormat[inFmt], width, height, b, DGLGLTextureFormat[inFmt], DGLGLTextureData[inFmt], bits);
}

void DGLGLDevice::UploadTexture(U32 mip, DGLFormat inFmt, U32 width, U32 height, U32 b, U8 *bits)
{
   glTexImage2D(GL_TEXTURE_2D, mip, DGLGLInternalFormat[inFmt], width, height, b, DGLGLTextureFormat[inFmt], DGLGLTextureData[inFmt], bits);
}

//------------------------------------------------------------------------------
// TEXTURE END
//------------------------------------------------------------------------------

void DGLGLDevice::GetApiValue(DGLAPIValues inval, S32 &outVal)
{
   glGetIntegerv(DGLGLAPIValues[inval], &outVal);
}


