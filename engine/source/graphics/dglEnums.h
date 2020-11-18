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

enum DGLPolyMode
{
   DGLPolyMode_FIRST = 0,
   DGLFront,
   DGLBack,
   DGLFrontAndBack,
   DGLFill,
   DGLLine,
   DGLPoint,
   DGLPolyMode_COUNT
};

enum DGLTextureFilter
{
   DGLTextureFilter_FIRST = 0,
   DGLTextureFilterNone = 0,
   DGLTextureFilterNearest,
   DGLTextureFilterLinear,
   DGLClampDef,
   DGLClampRepeat,
   DGLClampEdge,
   DGLTextureFilter_COUNT
};

// These represent Symbolic base formats
enum DGLFormat
{
   DGLFormat_FIRST = 0,

   // 8 bit texture formats...
   DGLFormatA8 = 0,// first in group...
   DGLFormatL8,
   DGLFormatA4L4,

   // 16 bit texture formats...
   DGLFormatR5G6B5,// first in group...
   DGLFormatR4G4B4A4,
   DGLFormatR5G5B5A1,
   DGLFormatR5G5B5X1,
   DGLFormatA8L8,
   DGLFormatL16,
   DGLFormatR16F,
   DGLFormatD16,

   // 24 bit texture formats...
   DGLFormatR8G8B8,// first in group...
   DGLFormatR8G8B8_SRGB,
   // 32 bit texture formats...
   DGLFormatR8G8B8A8,// first in group...
   DGLFormatR8G8B8X8,
   DGLFormatB8G8R8A8,
   DGLFormatR8G8B8A8_SRGB,
   DGLFormatR32F,
   DGLFormatR16G16,
   DGLFormatR16G16F,
   DGLFormatR10G10B10A2,
   DGLFormatR11G11B10,
   DGLFormatD32,
   DGLFormatD24X8,
   DGLFormatD24S8,
   GFXFormatD24FS8,

   // Guaranteed RGBA8 (for apis which really dont like bgr)
   DGLFormatR8G8B8A8_LINEAR_FORCE,

   // 64 bit texture formats...
   DGLFormatR16G16B16A16,// first in group...
   DGLFormatR16G16B16A16F,

   // 128 bit texture formats...
   DGLFormatR32G32B32A32F,// first in group...

   DGLFormat_COUNT,

   DGLFormat_8BIT    = DGLFormatA8,
   DGLFormat_16BIT   = DGLFormatR5G6B5,
   DGLFormat_24BIT   = DGLFormatR8G8B8,
   DGLFormat_32BIT   = DGLFormatR8G8B8A8,
   DGLFormat_64BIT   = DGLFormatR16G16B16A16,
   DGLFormat_128BIT  = DGLFormatR32G32B32A32F

};

enum DGLCompare
{
   DGLCompare_FIRST = 0,
   DGLNever,
   DGLLess,
   DGLEqual,
   DGLLEqual,
   DGLGreater,
   DGLNotEqual,
   DGLGEqual,
   DGLAlways,
   DGLCompare_COUNT
};

enum DGLAPIValues
{
   DGLAPIValues_FIRST = 0,
   DGLMaxClip,
   DGLMVStackDepth,
   DGLPROJDepth,
   DGLTexStackDepth,
   DGLAPIValues_COUNT
};

enum DGLTextureParam
{
   DGLTextureParam_FIRST = 0,
   DGLTextureParamMagFilter,
   DGLTextureParamMinFilter,
   DGLTextureParamWrapS,
   DGLTextureParamWrapT,
   DGLTextureParam_COUNT
};

enum DGLTextureEnvironment
{
   DGLTextureEnvironment_FIRST = 0,
   //Target
   DGLTexEnvi,
   DGLTexFilter,
   DGLPointSprite,
   //parameter name
   DGLTexEnvMode,
   //parameter
   DGLModulate,
   DGLReplace,
   DGLTextureEnvironment_COUNT
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