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

void STDCALL glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
   const GLchar *message, const void *userParam)
{
   // JTH [11/24/2016]: This is a temporary fix so that we do not get spammed for redundant fbo changes.
   // This only happens on Intel cards. This should be looked into sometime in the near future.
   char* api = "API_ID_REDUNDANT_FBO";
   if (!dStrnicmp(message, api, dStrlen(api)))
      return;
   if (severity == GL_DEBUG_SEVERITY_HIGH)
      Con::errorf("OPENGL: %s", message);
   else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
      Con::warnf("OPENGL: %s", message);
   else if (severity == GL_DEBUG_SEVERITY_LOW)
      Con::printf("OPENGL: %s", message);
}

//------------------------------------------------------------------------------

void STDCALL glAmdDebugCallback(GLuint id, GLenum category, GLenum severity, GLsizei length,
   const GLchar* message, GLvoid* userParam)
{
   if (severity == GL_DEBUG_SEVERITY_HIGH)
      Con::errorf("AMDOPENGL: %s", message);
   else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
      Con::warnf("AMDOPENGL: %s", message);
   else if (severity == GL_DEBUG_SEVERITY_LOW)
      Con::printf("AMDOPENGL: %s", message);
}

//------------------------------------------------------------------------------

DGLGLDevice::DGLGLDevice() :
   mMaxShaderTextures(2),
   mContext(NULL)
{

}

//------------------------------------------------------------------------------

void DGLGLDevice::initGLstate()
{
   Con::printf("Init GL");


   bool fullScreen = Con::getBoolVariable("$pref::Video::fullScreen");

   const char* resString = Con::getVariable((fullScreen ? "$pref::Video::resolution" : "$pref::Video::windowedRes"));


   // Get the video settings from the prefs:
   char* tempBuf = new char[dStrlen(resString) + 1];
   dStrcpy(tempBuf, resString);
   char* temp = dStrtok(tempBuf, " x\0");
   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   U32 width = (temp ? dAtoi(temp) : MIN_RESOLUTION_X);
   temp = dStrtok(NULL, " x\0");
   U32 height = (temp ? dAtoi(temp) : MIN_RESOLUTION_Y);
   temp = dStrtok(NULL, "\0");
   U32 bpp = (temp ? dAtoi(temp) : MIN_RESOLUTION_BIT_DEPTH);
   delete[] tempBuf;

   // If no device is specified, see which ones we have...
   if (!DGLDevice::setDevice(Con::getVariable("$pref::Video::displayDevice"), width, height, bpp, fullScreen))
   {
      // First, try the default OpenGL device:
      if (!DGLDevice::setDevice("OpenGL", width, height, bpp, fullScreen))
      {
         AssertFatal(false, "Could not find a compatible display device!");
         return;
      }
   }

   glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&mMaxShaderTextures);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Capabilities
   if (gglHasExtension(EXT_texture_filter_anisotropic))
   {
      mCapabilities.anisotropicFiltering = true;
   }

   if (gglHasExtension(ARB_buffer_storage))
   {
      mCapabilities.bufferStorage = true;
   }

   if (gglHasExtension(ARB_texture_storage))
   {
      mCapabilities.textureStorage = true;
   }

   if (gglHasExtension(ARB_copy_image))
   {
      mCapabilities.copyImage = true;
   }

   if (gglHasExtension(ARB_vertex_attrib_binding))
   {
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
      smEdgeClamp = true;
   }

#if TORQUE_DEBUG
   if (gglHasExtension(ARB_debug_output))
   {
      glEnable(GL_DEBUG_OUTPUT);
      glDebugMessageCallbackARB(glDebugCallback, NULL);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
      GLuint unusedIds = 0;
      glDebugMessageControlARB(GL_DONT_CARE,
         GL_DONT_CARE,
         GL_DONT_CARE,
         0,
         &unusedIds,
         GL_TRUE);
   }
   else if (gglHasExtension(AMD_debug_output))
   {
      glEnable(GL_DEBUG_OUTPUT);
      glDebugMessageCallbackAMD(glAmdDebugCallback, NULL);
      //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
      GLuint unusedIds = 0;
      glDebugMessageEnableAMD(GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
   }
#endif

   PlatformGL::setVSync(smDisableVsync ? 0 : 1);

   GLuint vao;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glEnable(GL_FRAMEBUFFER_SRGB);
}

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
   glMatrixMode(DGLGLMatrixType[type]);
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
   glPolygonMode(DGLGLPolyMode[face], DGLGLPolyMode[face]);
}

void DGLGLDevice::SetTextureEnvironment(DGLTextureEnvironment target, DGLTextureEnvironment name, DGLTextureEnvironment param)
{
   glTexEnvi(DGLGLTextureEnvironment[target], DGLGLTextureEnvironment[name], DGLGLTextureEnvironment[param]);
}

void DGLGLDevice::SetRotate(F32 ang, F32 x, F32 y, F32 z)
{
   glRotatef(ang, x, y, z);
}

void DGLGLDevice::SetTranslate(F32 x, F32 y, F32 z)
{
   glTranslatef(x, y, z);
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

void DGLGLDevice::LoadTexture(U32 n, U32 glName)
{
   glGenTextures(n, &glName);
}

void DGLGLDevice::BindTexture(U32 glName)
{
   glBindTexture(GL_TEXTURE_2D, glName);
}

void DGLGLDevice::DeleteTextures(U32 n, const U32* glName)
{
   glDeleteTextures(1, glName);
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




