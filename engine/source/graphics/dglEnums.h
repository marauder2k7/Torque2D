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
#ifndef _DGLENUMS_H_
#define _DGLENUMS_H_

enum DGLPrimitiveType
{
   DGLPT_FIRST = 0,
   DGLPointList = 0,
   DGLLineList,
   DGLLineStrip,
   DGLLineLoop,
   DGLTriangleList,
   DGLTriangleStrip,
   DGLTriangleFan,
   DGLQuads,
   DGLPT_COUNT
};

enum DGLMatrixType
{
   DGLMM_FIRST = 0,
   DGLMatrixMode = 0,
   DGLModelView,
   DGLProjection,
   DGLTexture,
   DGLMM_COUNT
};

enum DGLTextureFilter
{
   DGLTextureFilter_FIRST = 0,
   DGLTextureFilterNone = 0,
   DGLTextureFilterNearest,
   DGLTextureFilterLinear,
   DGLTextureFilter_COUNT
};

enum DGLClamp
{
   DGLClamp_FIRST = 0,
   DGLClampDef,
   DGLClampRepeat,
   DGLClampEdge,
   DGLClamp_COUNT
};

/// The supported blend modes.
enum DGLBlend
{
   DGLBlend_FIRST = 0,
   DGLBlendZero = 0, /// (0, 0, 0, 0)
   DGLBlendOne, /// (1, 1, 1, 1)
   DGLBlendSrcColor, /// (Rs, Gs, Bs, As)
   DGLBlendInvSrcColor, /// (1 - Rs, 1 - Gs, 1 - Bs, 1 - As)
   DGLBlendSrcAlpha, /// (As, As, As, As)
   DGLBlendInvSrcAlpha, /// ( 1 - As, 1 - As, 1 - As, 1 - As)
   DGLBlendDestAlpha, /// (Ad Ad Ad Ad)
   DGLBlendInvDestAlpha, /// (1 - Ad 1 - Ad 1 - Ad 1 - Ad)
   DGLBlendDestColor, /// (Rd, Gd, Bd, Ad)
   DGLBlendInvDestColor, /// (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad)
   DGLBlendSrcAlphaSat, /// (f, f, f, 1) where f = min(As, 1 - Ad)
   DGLBlend_COUNT
};

enum DGLRenderState
{
   DGLRS_FIRST = 0,
   DGLRSBlend,
   DGLRSCullFace,
   DGLRSAlphaTest,
   DGLRSDepthTest,
   DGLRSScissorTest,
   DGLRSLighting,
   DGLRSClipPlane0,
   DGLRSClipPlane1,
   DGLRSClipPlane2,
   DGLRSClipPlane3,
   DGLRSDebug,
   DGLRSDebugSync,
   DGLRSTexture2D,
   DGLRS_COUNT
};

enum DGLClientState
{
   DGLCS_FIRST = 0,
   DGLCSColorArray,
   DGLCSVertexArray,
   DGLCSTextCoordArray,
   DGLCS_COUNT
};

enum DGLBufferBit
{
   DGLBB_FIRST = 0,
   DGLBBColorBB,
   DGLBBDepthBB,
   DGLBBAccumBB,
   DGLBBStencilBB,
   DGLBB_COUNT
};

#endif