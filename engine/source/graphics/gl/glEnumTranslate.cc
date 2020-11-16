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
GLenum DGLGLTextureFilter[DGLTextureFilter_COUNT];
GLenum DGLGLBlend[DGLBlend_COUNT];
GLenum DGLGLMatrixMode[DGLMM_COUNT];
GLenum DGLGLRenderState[DGLRS_COUNT];
GLenum DGLGLClientState[DGLCS_COUNT];
GLenum DGLGLBufferBit[DGLBB_COUNT];
GLenum DGLGLClamp[DGLClamp_COUNT];

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

   // Texture Filter
   DGLGLTextureFilter[DGLTextureFilterNone]     = GL_NEAREST;
   DGLGLTextureFilter[DGLTextureFilterNearest]  = GL_NEAREST;
   DGLGLTextureFilter[DGLTextureFilterLinear]   = GL_LINEAR;

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
   DGLGLMatrixType[DGLMatrixMode]         = GL_MATRIX_MODE;
   DGLGLMatrixType[DGLModelView]          = GL_MODELVIEW;
   DGLGLMatrixType[DGLProjection]         = GL_PROJECTION;
   DGLGLMatrixType[DGLTexture]            = GL_TEXTURE;

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

   // Client States
   DGLGLClientState[DGLCSColorArray]      = GL_COLOR_ARRAY;
   DGLGLClientState[DGLCSVertexArray]     = GL_VERTEX_ARRAY;
   DGLGLClientState[DGLCSTextCoordArray]  = GL_TEXTURE_COORD_ARRAY;

   // Buffer Bit
   DGLGLBufferBit[DGLBBAccumBB]           = GL_ACCUM_BUFFER_BIT;
   DGLGLBufferBit[DGLBBColorBB]           = GL_COLOR_BUFFER_BIT;
   DGLGLBufferBit[DGLBBStencilBB]         = GL_STENCIL_BUFFER_BIT;
   DGLGLBufferBit[DGLBBDepthBB]           = GL_DEPTH_BUFFER_BIT;

   // Clamp method
   DGLGLClamp[DGLClampDef]                = GL_CLAMP;
   DGLGLClamp[DGLClampEdge]               = GL_CLAMP_TO_EDGE;
   DGLGLClamp[DGLClampRepeat]             = GL_REPEAT;

}