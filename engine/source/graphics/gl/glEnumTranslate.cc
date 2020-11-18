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

#include "platform/platform.h"
#include "graphics/gl/glEnumTranslate.h"

GLenum DGLGLPrimitiveType[DGLPT_COUNT];
GLenum DGLGLBlend[DGLBlend_COUNT];
GLenum DGLGLMatrixMode[DGLMM_COUNT];
GLenum DGLGLRenderState[DGLRS_COUNT];
GLenum DGLGLClientState[DGLCS_COUNT];
GLenum DGLGLBufferBit[DGLBB_COUNT];
GLenum DGLGLPolyMode[DGLPolyMode_COUNT];
GLenum DGLGLCompare[DGLCompare_COUNT];

GLenum DGLGLTextureFilter[DGLTextureFilter_COUNT];
GLenum DGLGLTextureParam[DGLTextureParam_COUNT];

GLenum DGLGLTextureFormat[DGLFormat_COUNT];
GLenum DGLGLInternalFormat[DGLFormat_COUNT];
GLenum DGLGLTextureData[DGLFormat_COUNT];

GLenum DGLGLTextureEnvironment[DGLTextureEnvironment_COUNT];

GLenum DGLGLAPIValues[DGLAPIValues_COUNT];

void DGLGLEnumTranslate::init()
{

   // Primitives
   DGLGLPrimitiveType[DGLPointList]       = GL_POINTS;
   DGLGLPrimitiveType[DGLLineList]        = GL_LINES;
   DGLGLPrimitiveType[DGLLineLoop]        = GL_LINE_LOOP;
   DGLGLPrimitiveType[DGLTriangleList]    = GL_TRIANGLES;
   DGLGLPrimitiveType[DGLTriangleStrip]   = GL_TRIANGLE_STRIP;
   DGLGLPrimitiveType[DGLTriangleFan]     = GL_TRIANGLE_FAN;
   DGLGLPrimitiveType[DGLQuads]           = GL_QUADS;

   // Blend
   DGLGLBlend[DGLBlendZero]               = GL_ZERO;
   DGLGLBlend[DGLBlendOne]                = GL_ONE;
   DGLGLBlend[DGLBlendSrcColor]           = GL_SRC_COLOR;
   DGLGLBlend[DGLBlendInvSrcColor]        = GL_ONE_MINUS_SRC_COLOR;
   DGLGLBlend[DGLBlendSrcAlpha]           = GL_SRC_ALPHA;
   DGLGLBlend[DGLBlendInvSrcAlpha]        = GL_ONE_MINUS_SRC_ALPHA;
   DGLGLBlend[DGLBlendDestAlpha]          = GL_DST_ALPHA;
   DGLGLBlend[DGLBlendInvDestAlpha]       = GL_ONE_MINUS_DST_ALPHA;
   DGLGLBlend[DGLBlendDestColor]          = GL_DST_COLOR;
   DGLGLBlend[DGLBlendInvDestColor]       = GL_ONE_MINUS_DST_COLOR;
   DGLGLBlend[DGLBlendSrcAlphaSat]        = GL_SRC_ALPHA_SATURATE;

   // Matrix Mode
   DGLGLMatrixMode[DGLMatrixMode]         = GL_MATRIX_MODE;
   DGLGLMatrixMode[DGLModelView]          = GL_MODELVIEW;
   DGLGLMatrixMode[DGLProjection]         = GL_PROJECTION;
   DGLGLMatrixMode[DGLTexture]            = GL_TEXTURE;

   // Render States
   DGLGLRenderState[DGLRSBlend]           = GL_BLEND;
   DGLGLRenderState[DGLRSCullFace]        = GL_CULL_FACE;
   DGLGLRenderState[DGLRSAlphaTest]       = GL_ALPHA_TEST;
   DGLGLRenderState[DGLRSDepthTest]       = GL_DEPTH_TEST;
   DGLGLRenderState[DGLRSScissorTest]     = GL_SCISSOR_TEST;
   DGLGLRenderState[DGLRSLighting]        = GL_LIGHTING;
   DGLGLRenderState[DGLRSClipPlane0]      = GL_CLIP_PLANE0;
   DGLGLRenderState[DGLRSClipPlane1]      = GL_CLIP_PLANE1;
   DGLGLRenderState[DGLRSClipPlane2]      = GL_CLIP_PLANE2;
   DGLGLRenderState[DGLRSClipPlane3]      = GL_CLIP_PLANE3;
   DGLGLRenderState[DGLRSDebug]           = GL_DEBUG_OUTPUT;
   DGLGLRenderState[DGLRSDebugSync]       = GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB;
   DGLGLRenderState[DGLRSTexture2D]       = GL_TEXTURE_2D;

   //Compare enums for alpha func
   DGLGLCompare[DGLNever]     = GL_NEVER;
   DGLGLCompare[DGLLess]      = GL_LESS;
   DGLGLCompare[DGLEqual]     = GL_EQUAL;
   DGLGLCompare[DGLLEqual]    = GL_LEQUAL;
   DGLGLCompare[DGLGEqual]    = GL_GEQUAL;
   DGLGLCompare[DGLNotEqual]  = GL_NOTEQUAL;
   DGLGLCompare[DGLGreater]   = GL_GREATER;
   DGLGLCompare[DGLAlways]    = GL_ALWAYS;

   // Client States
   DGLGLClientState[DGLCSColorArray]      = GL_COLOR_ARRAY;
   DGLGLClientState[DGLCSVertexArray]     = GL_VERTEX_ARRAY;
   DGLGLClientState[DGLCSTextCoordArray]  = GL_TEXTURE_COORD_ARRAY;

   // Buffer Bit
   DGLGLBufferBit[DGLBBAccumBB]           = GL_ACCUM_BUFFER_BIT;
   DGLGLBufferBit[DGLBBColorBB]           = GL_COLOR_BUFFER_BIT;
   DGLGLBufferBit[DGLBBStencilBB]         = GL_STENCIL_BUFFER_BIT;
   DGLGLBufferBit[DGLBBDepthBB]           = GL_DEPTH_BUFFER_BIT;

   //Polygon Mode
   DGLGLPolyMode[DGLFront]                = GL_FRONT;
   DGLGLPolyMode[DGLBack]                 = GL_BACK;
   DGLGLPolyMode[DGLFrontAndBack]         = GL_FRONT_AND_BACK;
   DGLGLPolyMode[DGLFill]                 = GL_FILL;
   DGLGLPolyMode[DGLPoint]                = GL_POINT;
   DGLGLPolyMode[DGLLine]                 = GL_LINE;

   // Clamp method
   DGLGLTextureFilter[DGLClampDef]        = GL_CLAMP;
   DGLGLTextureFilter[DGLClampEdge]       = GL_CLAMP_TO_EDGE;
   DGLGLTextureFilter[DGLClampRepeat]     = GL_REPEAT;

   // Texture Filter
   DGLGLTextureFilter[DGLTextureFilterNone]     = GL_NEAREST;
   DGLGLTextureFilter[DGLTextureFilterNearest]  = GL_NEAREST;
   DGLGLTextureFilter[DGLTextureFilterLinear]   = GL_LINEAR;

   // Texture Params
   DGLGLTextureParam[DGLTextureParamMagFilter]  = GL_TEXTURE_MAG_FILTER;
   DGLGLTextureParam[DGLTextureParamMinFilter]  = GL_TEXTURE_MIN_FILTER;
   DGLGLTextureParam[DGLTextureParamWrapS]      = GL_TEXTURE_WRAP_S;
   DGLGLTextureParam[DGLTextureParamWrapS]      = GL_TEXTURE_WRAP_T;

   // Texture format
   // These represent Symbolic base formats
   DGLGLTextureFormat[DGLFormatA8]           = GL_ALPHA;
   DGLGLTextureFormat[DGLFormatL8]           = GL_LUMINANCE;
   DGLGLTextureFormat[DGLFormatA4L4]         = GL_LUMINANCE_ALPHA;
   DGLGLTextureFormat[DGLFormatR5G6B5]       = GL_RGB;
   DGLGLTextureFormat[DGLFormatR4G4B4A4]     = GL_RGBA;
   DGLGLTextureFormat[DGLFormatR5G5B5A1]     = GL_RGBA;
   DGLGLTextureFormat[DGLFormatR5G5B5X1]     = GL_RGBA;
   DGLGLTextureFormat[DGLFormatL16]          = GL_RED;
   DGLGLTextureFormat[DGLFormatD16]          = GL_DEPTH_COMPONENT;
   DGLGLTextureFormat[DGLFormatR8G8B8]       = GL_RGB;
   DGLGLTextureFormat[DGLFormatR8G8B8A8]     = GL_RGBA;
   DGLGLTextureFormat[DGLFormatR8G8B8X8]     = GL_RGBA;
   DGLGLTextureFormat[DGLFormatB8G8R8A8]     = GL_BGRA;
   DGLGLTextureFormat[DGLFormatR10G10B10A2]  = GL_RGBA;
   DGLGLTextureFormat[DGLFormatR11G11B10]    = GL_RGB;
   DGLGLTextureFormat[DGLFormatD32]          = GL_DEPTH_COMPONENT;
   DGLGLTextureFormat[DGLFormatD24X8]        = GL_DEPTH_STENCIL;
   DGLGLTextureFormat[DGLFormatD24S8]        = GL_DEPTH_STENCIL;
   DGLGLTextureFormat[DGLFormatR16G16B16A16] = GL_RGBA;
   //sRGB
   DGLGLTextureFormat[DGLFormatR8G8B8_SRGB]   = GL_RGB;
   DGLGLTextureFormat[DGLFormatR8G8B8A8_SRGB] = GL_RGBA;

   // Internal Texture Format ---This is a nightmare =/ 
   // These represent the actual internal format in OpenGL.
   DGLGLInternalFormat[DGLFormatA8]             = GL_ALPHA;
   DGLGLInternalFormat[DGLFormatL8]             = GL_LUMINANCE;
   DGLGLInternalFormat[DGLFormatA4L4]           = GL_LUMINANCE_ALPHA;
   DGLGLInternalFormat[DGLFormatR5G6B5]         = GL_RGB565;
   DGLGLInternalFormat[DGLFormatR4G4B4A4]       = GL_RGBA4;
   DGLGLInternalFormat[DGLFormatR5G5B5A1]       = GL_RGB5_A1;
   DGLGLInternalFormat[DGLFormatR5G5B5X1]       = GL_RGB5_A1;
   DGLGLInternalFormat[DGLFormatL16]            = GL_R16;
   DGLGLInternalFormat[DGLFormatD16]            = GL_DEPTH_COMPONENT16;
   DGLGLInternalFormat[DGLFormatR8G8B8]         = GL_RGB8;
   DGLGLInternalFormat[DGLFormatR8G8B8A8]       = GL_RGBA8;
   DGLGLInternalFormat[DGLFormatR8G8B8X8]       = GL_RGBA8;
   DGLGLInternalFormat[DGLFormatB8G8R8A8]       = GL_RGBA8;
   DGLGLInternalFormat[DGLFormatR10G10B10A2]    = GL_RGB10_A2;
   DGLGLInternalFormat[DGLFormatR11G11B10]      = GL_R11F_G11F_B10F;
   DGLGLInternalFormat[DGLFormatD32]            = GL_DEPTH_COMPONENT32;
   DGLGLInternalFormat[DGLFormatD24X8]          = GL_DEPTH24_STENCIL8;
   DGLGLInternalFormat[DGLFormatD24S8]          = GL_DEPTH24_STENCIL8;
   DGLGLInternalFormat[DGLFormatR16G16B16A16]   = GL_RGBA16;
   //sRGB
   DGLGLInternalFormat[DGLFormatR8G8B8_SRGB]    = GL_SRGB8;
   DGLGLInternalFormat[DGLFormatR8G8B8A8_SRGB]  = GL_SRGB8_ALPHA8;

   // Texture Data Type
   DGLGLTextureData[DGLFormatA8]          = GL_UNSIGNED_BYTE;
   DGLGLTextureData[DGLFormatL8]          = GL_UNSIGNED_BYTE;
   DGLGLTextureData[DGLFormatR5G6B5]      = GL_UNSIGNED_SHORT_5_6_5;
   DGLGLTextureData[DGLFormatR4G4B4A4]    = GL_UNSIGNED_SHORT_4_4_4_4;
   DGLGLTextureData[DGLFormatR5G5B5A1]    = GL_UNSIGNED_SHORT_5_5_5_1;
   DGLGLTextureData[DGLFormatR5G5B5X1]    = GL_UNSIGNED_SHORT_5_5_5_1;
   DGLGLTextureData[DGLFormatL16]         = GL_UNSIGNED_SHORT;
   DGLGLTextureData[DGLFormatD16]         = GL_UNSIGNED_SHORT;
   DGLGLTextureData[DGLFormatR8G8B8]      = GL_UNSIGNED_BYTE;
   DGLGLTextureData[DGLFormatR8G8B8A8]    = GL_UNSIGNED_BYTE;
   DGLGLTextureData[DGLFormatR8G8B8X8]    = GL_UNSIGNED_BYTE;
   DGLGLTextureData[DGLFormatB8G8R8A8]    = GL_UNSIGNED_BYTE;;
   DGLGLTextureData[DGLFormatR10G10B10A2] = GL_UNSIGNED_INT_10_10_10_2;
   DGLGLTextureData[DGLFormatR11G11B10]   = GL_UNSIGNED_INT_10F_11F_11F_REV;
   DGLGLTextureData[DGLFormatD32]         = GL_UNSIGNED_INT;
   DGLGLTextureData[DGLFormatD24X8]       = GL_UNSIGNED_INT_24_8;
   DGLGLTextureData[DGLFormatD24S8]       = GL_UNSIGNED_INT_24_8;
   DGLGLTextureData[DGLFormatR16G16B16A16] = GL_UNSIGNED_SHORT;
   // sRGB
   DGLGLTextureData[DGLFormatR8G8B8_SRGB]  = GL_UNSIGNED_BYTE;
   DGLGLTextureData[DGLFormatR8G8B8A8_SRGB] = GL_UNSIGNED_BYTE;

   //Texture Environment
   DGLGLTextureEnvironment[DGLTexEnvi]       = GL_TEXTURE_ENV;
   DGLGLTextureEnvironment[DGLTexFilter]     = GL_TEXTURE_FILTER_CONTROL;
   DGLGLTextureEnvironment[DGLPointSprite]   = GL_POINT_SPRITE;

   DGLGLTextureEnvironment[DGLTexEnvMode]    = GL_TEXTURE_ENV_MODE;

   DGLGLTextureEnvironment[DGLModulate]      = GL_MODULATE;
   DGLGLTextureEnvironment[DGLReplace]       = GL_REPLACE;

   // Misc
   DGLGLAPIValues[DGLMaxClip]       = GL_MAX_CLIP_PLANES;
   DGLGLAPIValues[DGLMVStackDepth]  = GL_MODELVIEW_STACK_DEPTH;
   DGLGLAPIValues[DGLPROJDepth]     = GL_PROJECTION_STACK_DEPTH;
   DGLGLAPIValues[DGLTexStackDepth] = GL_TEXTURE_STACK_DEPTH;

}