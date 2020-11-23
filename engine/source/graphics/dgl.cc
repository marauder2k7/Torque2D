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

#include "math/mPoint.h"
#include "graphics/TextureManager.h"
#include "graphics/dgl.h"
#include "game/gameInterface.h"

#include "graphics/gColor.h"
#include "math/mPoint.h"
#include "math/mRect.h"
#include "graphics/gFont.h"
#include "console/console.h"
#include "math/mMatrix.h"
#include "memory/frameAllocator.h"
#include "debug/profiler.h"
#include "string/unicode.h"

#include "dglMac_ScriptBinding.h"
#include "dgl_ScriptBinding.h"
#include <vector>

Vector<DGLDevice*> DGLDevice::smDeviceList;
DGLDevice * DGLDevice::smCurrentDevice = NULL;
bool DGLDevice::smCritical = false;
bool DGLDevice::smNeedResurrect = false;
bool DGLDevice::smDisableVSync = true;
bool DGLDevice::smEdgeClamp = false;

DGLVideoMode DGLDevice::smCurrentRes;
bool DGLDevice::smIsFullScreen;

namespace {

ColorI sg_bitmapModulation(255, 255, 255, 255);
ColorI sg_textAnchorColor(255, 255, 255, 255);
ColorI sg_stackColor(255, 255, 255, 255);
ColorF colorAlphaW(1.0f, 1.0f, 1.0f, 0.0f);

} // namespace {}

DGLDevice::DGLDevice()
{
   VECTOR_SET_ASSOCIATION(mVideoModeList);
   AssertFatal(smCurrentDevice == NULL, "Already a GFXDevice created! Bad!");
   mProjectionMatrix.identity();
   smCurrentDevice = this;
}

void DGLDevice::init()
{
   Con::printSeparator();
   Con::printf("DGL Device Initialization:");
   Con::printSeparator();

   smCurrentRes = DGLVideoMode(0, 0, 0);
   smIsFullScreen = false;

   destroy();
}

//------------------------------------------------------------------------------

void DGLDevice::destroy()
{
   if (smCurrentDevice)
   {
      smCritical = true;
      Game->textureKill();
      smCurrentDevice->shutdown();
      smCritical = false;
   }

   smCurrentDevice = NULL;

   for (U32 i = 0; i < (U32)smDeviceList.size(); i++)
      delete smDeviceList[i];

   smDeviceList.clear();
}

bool DGLDevice::installDevice(DGLDevice * dev)
{
   if (dev)
   {
      smDeviceList.push_back(dev);
      return true;
   }
   return false;
}

bool DGLDevice::setDevice(const char * renderName, U32 width, U32 height, U32 bpp, bool fullScreen)
{
   S32 deviceIndex = NO_DEVICE;
   S32 iOpenGL = -1;
   S32 iD3D = -1;
   Con::printf("SetDevice - Activate");

   bool bOpenglRender = true; //(bool)(dStricmp(renderName,"OpenGL") == 0);
   bool bD3DRender = false; //(bool)(dStricmp(renderName,"D3D") == 0);
   bool bAllowD3D = false; //Con::getBoolVariable("$pref::Video::allowD3D");
   bool bAllowOpengl = true; //Con::getBoolVariable("$pref::Video::allowOpenGL");


   for (S32 i = 0; i < smDeviceList.size(); i++)
   {
      if (dStrcmp(smDeviceList[i]->mDeviceName, renderName) == 0)
         deviceIndex = i;

      if (dStrcmp(smDeviceList[i]->mDeviceName, "OpenGL") == 0)
         iOpenGL = i;
      if (dStrcmp(smDeviceList[i]->mDeviceName, "D3D") == 0)
         iD3D = i;
   }

   if (deviceIndex == NO_DEVICE)
   {
      Con::warnf(ConsoleLogEntry::General, "\"%s\" display device not found!", renderName);
      return false;
   }
   
   // Change the display device:
   if (smDeviceList[deviceIndex] == NULL)
      return false;


   if (smCurrentDevice && smCurrentDevice != smDeviceList[deviceIndex])
   {
      Con::printf("Deactivating the previous display device...");
      Game->textureKill();
      smNeedResurrect = true;
      smCurrentDevice->shutdown();
   }
   if (iOpenGL != -1 && !bAllowOpengl)
   {
      // change to D3D, delete OpenGL in the recursive call
      if (bOpenglRender)
      {
         U32 w, h, d;

         if (fullScreen)
            dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
         else
            dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);

         return setDevice("D3D", w, h, d, fullScreen);
      }
      else
      {
         delete smDeviceList[iOpenGL];
         smDeviceList.erase(iOpenGL);
      }
   }
   else if (iD3D != -1 && !bAllowD3D)
   {
      // change to OpenGL, delete D3D in the recursive call
      if (bD3DRender)
      {
         U32 w, h, d;
         if (fullScreen)
            dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
         else
            dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);

         return setDevice("OpenGL", w, h, d, fullScreen);
      }
      else
      {
         delete smDeviceList[iD3D];
         smDeviceList.erase(iD3D);
      }
   }
   else if (iD3D != -1 && bOpenglRender &&
      !Con::getBoolVariable("$pref::Video::preferOpenGL") &&
      !Con::getBoolVariable("$pref::Video::appliedPref"))
   {
      U32 w, h, d;
      if (fullScreen)
         dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
      else
         dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);
      Con::setBoolVariable("$pref::Video::appliedPref", true);

      return setDevice("D3D", w, h, d, fullScreen);
   }
   else
      Con::setBoolVariable("$pref::Video::appliedPref", true);

   Con::printf("Activating the %s display device...", renderName);
   smCurrentDevice = smDeviceList[deviceIndex];

   smCritical = true;
   bool result = smCurrentDevice->activate(width, height, bpp, fullScreen);
   smCritical = false;


   if (result)
   {
      if (smNeedResurrect)
      {
         Game->textureResurrect();
         smNeedResurrect = false;
      }
      //if (sgOriginalGamma != -1.0 || getGammaCorrection(sgOriginalGamma))
         //setGammaCorrection(sgOriginalGamma + sgGammaCorrection);
      Con::evaluate("resetCanvas();");
   }
}

void DGLDevice::GetFrustum(F64 *left, F64 *right, F64 *bottom, F64 *top, F64 *nearPlane, F64 *farPlane)
{
   *left = frustLeft;
   *right = frustRight;
   *bottom = frustBottom;
   *top = frustTop;
   *nearPlane = frustNear;
   *farPlane = frustFar;
}

void DGLDevice::GetViewport(RectI* outViewport)
{
   AssertFatal(outViewport != NULL, "Error, bad point in GetViewport");
   *outViewport = viewPort;
}

//--------------------------------------------------------------------------

void DGLDevice::SetBitmapModulation(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_bitmapModulation = c;
   sg_textAnchorColor = sg_bitmapModulation;
}

void DGLDevice::GetBitmapModulation(ColorF* color)
{
   *color = sg_bitmapModulation;
}

void DGLDevice::GetBitmapModulation(ColorI* color)
{
   *color = sg_bitmapModulation;
}

void DGLDevice::ClearBitmapModulation()
{
   sg_bitmapModulation.set(255, 255, 255, 255);
}

void DGLDevice::SetTextAnchorColor(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_textAnchorColor = c;
}

void DGLDevice::DrawBlendBox(RectI &bounds, ColorF &c1, ColorF &c2, ColorF &c3, ColorF &c4)
{
   F32 left = bounds.point.x, right = bounds.point.x + bounds.extent.x - 1;
   F32 top = bounds.point.y, bottom = bounds.point.y + bounds.extent.y - 1;

   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   const F32 verts[] = {
       left, top,
       right,  top,
       left,  bottom,
       right,   bottom,
   };
   const U32 squareColors[] = {
       static_cast<U32>(255 * c1.red),  static_cast<U32>(255 * c1.green),  static_cast<U32>(255 * c1.blue),  static_cast<U32>(255 * c1.alpha),
       static_cast<U32>(255 * c2.red),  static_cast<U32>(255 * c2.green),  static_cast<U32>(255 * c2.blue),  static_cast<U32>(255 * c2.alpha),
       static_cast<U32>(255 * c3.red),  static_cast<U32>(255 * c3.green),  static_cast<U32>(255 * c3.blue),  static_cast<U32>(255 * c3.alpha),
       static_cast<U32>(255 * c4.red),  static_cast<U32>(255 * c4.green),  static_cast<U32>(255 * c4.blue),  static_cast<U32>(255 * c4.alpha),
   };

   SetVertexPoint(2, 0, verts);
   EnableClientState(DGLCSVertexArray);
   SetColorPoint(4, 0, squareColors);
   EnableClientState(DGLCSColorArray);

   DrawArrays(DGLTriangleStrip, 0, 4);
}

/// Function to draw a set of boxes blending throughout an array of colors
void DGLDevice::DrawBlendRangeBox(RectI &bounds, bool vertical, U8 numColors, ColorI *colors)
{
   S32 left = bounds.point.x, right = bounds.point.x + bounds.extent.x - 1;
   S32 top = bounds.point.y, bottom = bounds.point.y + bounds.extent.y - 1;

   // Calculate increment value
   S32 x_inc = S32(mFloor((right - left) / (numColors - 1)));
   S32 y_inc = S32(mFloor((bottom - top) / (numColors - 1)));


   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   F32 verts[] = {
       0.0f,	0.0f,	0.0f,	0.0f,
       0.0f,	0.0f,	0.0f,	0.0f,
   };

   SetVertexPoint(2, 0, verts);
   EnableClientState(DGLCSVertexArray);

   for (U16 i = 0; i < numColors - 1; i++)
   {
      // If we are at the end, x_inc and y_inc need to go to the end (otherwise there is a rendering bug)
      if (i == numColors - 2)
      {
         x_inc += right - left - 1;
         y_inc += bottom - top - 1;
      }

      if (vertical)  // Vertical (+y)		colors go up and down
      {
         // First color
         SetColorI(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
         verts[0] = (F32)left;
         verts[1] = (F32)top;
         verts[2] = (F32)(left + x_inc);
         verts[3] = (F32)top;
         // Second color
         SetColorI(colors[i + 1].red, colors[i + 1].green, colors[i + 1].blue, colors[i + 1].alpha);

         verts[4] = (F32)left;
         verts[5] = (F32)bottom;
         verts[6] = (F32)(left + x_inc);
         verts[7] = (F32)bottom;
         DrawArrays(DGLTriangleStrip, 0, 4);
         left += x_inc;
      }
      else  // Horizontal (+x)	colors go lateral
      {
         // First color
         SetColorI(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
         verts[0] = (F32)left;
         verts[1] = (F32)(top + y_inc);
         verts[2] = (F32)right;
         verts[3] = (F32)(top + y_inc);

         // Second color
         SetColorI(colors[i + 1].red, colors[i + 1].green, colors[i + 1].blue, colors[i + 1].alpha);
         verts[4] = (F32)left;
         verts[5] = (F32)top;
         verts[6] = (F32)right;
         verts[7] = (F32)top;
         DrawArrays(DGLTriangleStrip, 0, 4);
         top += y_inc;
      }
   }

}


//--------------------------------------------------------------------------
void DGLDevice::DrawBitmapStretchSR(TextureObject* texture,
                                    const RectI&   dstRect,
                                    const RectI&   srcRect,
                                    const U32      in_flip,
                                    F32			   fSpin,
                                    bool				bSilhouette)
{	
   AssertFatal(texture != NULL, "GSurface::drawBitmapStretchSR: NULL Handle");
   if(!dstRect.isValidRect())
      return;
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapStretchSR: routines assume normal rects");


   DisableState(DGLRSLighting);
   EnableState(DGLRSTexture2D);
   BindTexture(texture->getGLTextureName());


   if (bSilhouette)
   {
      SetTextureEnvironment(DGLTexEnvi, DGLTexEnvMode, DGLTexBlend);
   
      ColorF kModulationColor;
      GetBitmapModulation(&kModulationColor);
      SetTextureEnvironmentF(DGLTexEnvi, DGLTexEnvColor, kModulationColor.address());
   }
   else
   {
      SetTextureEnvironment(DGLTexEnvi, DGLTexEnvMode, DGLModulate);
   }

   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);

   F32 texLeft   = F32(srcRect.point.x)                    / F32(texture->getTextureWidth());
   F32 texRight  = F32(srcRect.point.x + srcRect.extent.x) / F32(texture->getTextureWidth());
   F32 texTop    = F32(srcRect.point.y)                    / F32(texture->getTextureHeight());
   F32 texBottom = F32(srcRect.point.y + srcRect.extent.y) / F32(texture->getTextureHeight());
     Point2F scrPoints[4];
   if(fSpin == 0.0f)
   {
            scrPoints[0].x = (F32)dstRect.point.x;
            scrPoints[0].y = (F32)dstRect.point.y;
            scrPoints[1].x = (F32)(dstRect.point.x + dstRect.extent.x);
            scrPoints[1].y = (F32)dstRect.point.y;
            scrPoints[2].x = (F32)dstRect.point.x;
            scrPoints[2].y = (F32)(dstRect.point.y + dstRect.extent.y);
            scrPoints[3].x = (F32)(dstRect.point.x + dstRect.extent.x);
            scrPoints[3].y = (F32)(dstRect.point.y + dstRect.extent.y);
            //screenLeft   = dstRect.point.x;
            //screenRight  = dstRect.point.x + dstRect.extent.x;
            //screenTop    = dstRect.point.y;
            //screenBottom = dstRect.point.y + dstRect.extent.y;
        }
        else
   {
     //WE NEED TO IMPLEMENT A FAST 2D ROTATION -- NOT THIS SLOWER 3D ROTATION
     MatrixF rotMatrix( EulerF( 0.0, 0.0, mDegToRad(fSpin) ) );

     Point3F offset( dstRect.point.x + dstRect.extent.x / 2.0f,
                                         dstRect.point.y + dstRect.extent.y / 2.0f, 0.0 );
         Point3F points[4];
   
     points[0] = Point3F(-dstRect.extent.x / 2.0f,  dstRect.extent.y / 2.0f, 0.0);
     points[1] = Point3F( dstRect.extent.x / 2.0f,  dstRect.extent.y / 2.0f, 0.0);
     points[2] = Point3F(-dstRect.extent.x / 2.0f, -dstRect.extent.y / 2.0f, 0.0);
     points[3] = Point3F( dstRect.extent.x / 2.0f, -dstRect.extent.y / 2.0f, 0.0);

     for( int i=0; i<4; i++ )
     {
        rotMatrix.mulP( points[i] );
        points[i] += offset;
        scrPoints[i].x = points[i].x;
        scrPoints[i].y = points[i].y;
     }
   }
  

   if(in_flip & GFlip_X)
   {
      F32 temp = texLeft;
      texLeft = texRight;
      texRight = temp;
   }
   if(in_flip & GFlip_Y)
   {
      F32 temp = texTop;
      texTop = texBottom;
      texBottom = temp;
   }

   SetColorI(sg_bitmapModulation.red,
             sg_bitmapModulation.green,
             sg_bitmapModulation.blue,
             sg_bitmapModulation.alpha);


    F32 verts[] = {
        scrPoints[0].x, scrPoints[0].y,
        scrPoints[1].x, scrPoints[1].y,
        scrPoints[2].x, scrPoints[2].y,
        scrPoints[3].x, scrPoints[3].y,
    };
    F32 texVerts[] = {
        texLeft,  texTop,
        texRight, texTop,		
        texLeft,  texBottom,
        texRight, texBottom,
    };
    
    
    DisableClientState(DGLCSColorArray);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    EnableClientState(DGLCSVertexArray);
    EnableClientState(DGLCSTextCoordArray);

    SetVertexPoint(2, 0, verts);
    SetTexPoint(2, 0, texVerts);
    DrawArrays(DGLTriangleStrip, 0, 4);

   if (bSilhouette)
   {
      SetTextureEnvironmentF(DGLTexEnvi, DGLTexEnvColor, ColorF(0.0f, 0.0f, 0.0f, 0.0f).address());
   }

   DisableState(DGLRSBlend);
   DisableState(DGLRSTexture2D);
}

void DGLDevice::DrawBitmap(TextureObject* texture, const Point2I& in_rAt, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmap: NULL Handle");

   // All non-StretchSR bitmaps are transformed into StretchSR calls...
   //
   RectI subRegion(0, 0,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   RectI stretch(in_rAt.x, in_rAt.y,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   DrawBitmapStretchSR(texture,
                          stretch,
                          subRegion,
                          in_flip);
}

void DGLDevice::DrawBitmapTile(TextureObject* texture, const RectI& dstRect, const U32 in_flip, F32 fSpin, bool bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapTile: NULL Handle");
   // since the texture coords are calculated from the texture sub-rect we pass in,
   // but no actual indexing of that sub-rect happens, we can pass any texture coords
   // that we like to dglDrawBitmapSR(). We use this to force tiling.
   RectI subregion(0,0, dstRect.extent.x, dstRect.extent.y);
   DrawBitmapStretchSR(texture, dstRect, subregion, in_flip, fSpin, bSilhouette);
}

void DGLDevice::DrawBitmapStretch(TextureObject* texture, const RectI& dstRect, const U32 in_flip, F32 fSpin, bool bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapStretch: NULL Handle");
   AssertFatal(dstRect.isValidRect() == true,
               "GSurface::drawBitmapStretch: routines assume normal rects");

   RectI subRegion(0, 0,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   DrawBitmapStretchSR(texture,
                          dstRect,
                          subRegion,
                          in_flip,
                          fSpin,
                          bSilhouette);
}

void DGLDevice::DrawBitmapSR(TextureObject *texture, const Point2I& in_rAt, const RectI& srcRect, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapSR: NULL Handle");
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapSR: routines assume normal rects");

   RectI stretch(in_rAt.x, in_rAt.y,
                 srcRect.len_x(),
                 srcRect.len_y());
   DrawBitmapStretchSR(texture,
                          stretch,
                          srcRect,
                          in_flip);
}

U32 DGLDevice::DrawText(GFont*   font,
          const Point2I& ptDraw,
          const UTF16*    in_string,
          const ColorI*  colorTable,
          const U32      maxColorIndex,
          F32            rot)
{
   return DrawTextN(font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot);
}

U32 DGLDevice::DrawText(GFont*   font,
          const Point2I& ptDraw,
          const UTF8*    in_string,
          const ColorI*  colorTable,
          const U32      maxColorIndex,
          F32            rot)
{
   // Just a note - dStrlen(utf8) isn't strictly correct but it's guaranteed to be
   // as long or longer than the real length. dglDrawTextN fails gracefully
   // if you specify overlong, so this is ok.
   return DrawTextN(font, ptDraw, in_string, dStrlen((const UTF8 *) in_string), colorTable, maxColorIndex, rot);
}

struct TextVertex
{
   Point2F p;
   Point2F t;
   ColorI c;
    TextVertex() { set( 0.0f, 0.0f, 0.0f, 0.0f, ColorI(0, 0, 0) ); }
   void set(F32 x, F32 y, F32 tx, F32 ty, ColorI color)
   {
      p.x = x;
      p.y = y;
      t.x = tx;
      t.y = ty;
      c = color;
   }
};

//------------------------------------------------------------------------------

U32 DGLDevice::DrawTextN(GFont*          font,
                 const Point2I&  ptDraw,
                 const UTF8*     in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   PROFILE_START(DrawText_UTF8);
   
   U32 len = dStrlen(in_string) + 1;
   FrameTemp<UTF16> ubuf(len);
   convertUTF8toUTF16(in_string, ubuf, len);
   U32 tmp = DrawTextN(font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot);

   PROFILE_END();

   return tmp;
}

//-----------------------------------------------------------------------------

U32 DGLDevice::DrawTextN(GFont*          font,
                 const Point2I&  ptDraw,
                 const UTF16*    in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   // return on zero length strings
   if( n < 1 )
      return ptDraw.x;
   PROFILE_START(DrawText);

   MatrixF rotMatrix( EulerF( 0.0, 0.0, mDegToRad( rot ) ) );
   Point3F offset( (F32)ptDraw.x, (F32)ptDraw.y, 0.0f );
   Point3F points[4];

   U32 nCharCount = 0;

   Point2I        pt;
   UTF16          c;
   pt.x           = 0;

   ColorI         currentColor;
   S32            currentPt = 0;

   TextureObject  *lastTexture = NULL;

   currentColor   = sg_bitmapModulation;

   FrameTemp<TextVertex> vert(4*n);

   DisableState(DGLRSLighting);
   EnableState(DGLRSTexture2D);
   SetTextureEnvironment(DGLTexEnvi, DGLTexEnvMode, DGLModulate);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   EnableState(DGLRSBlend);

   // Enable states and link our arrays.
   EnableClientState(DGLCSVertexArray);
   SetVertexPoint(2, sizeof(TextVertex), &vert[0].p);

   EnableClientState(DGLCSColorArray);
   SetColorPointU(4, sizeof(TextVertex), &vert[0].c);

   EnableClientState(DGLCSTextCoordArray);
   SetTexPoint(2, sizeof(TextVertex), &vert[0].t);

   // first build the point, color, and coord arrays
   U32 i;

   for(i = 0,c = in_string[i];in_string[i] && i < n;i++,c = in_string[i])
   {
      nCharCount++;
      if(nCharCount > n)
          break;

      // We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
      if ((c >=  1 && c <=  7) ||
         (c >= 11 && c <= 12) ||
         (c == 14))
      {
         // Color code
         if (colorTable)
         {
            static U8 remap[15] =
            {
               0x0, // 0 special null terminator
               0x0, // 1 ascii start-of-heading??
               0x1, 
               0x2, 
               0x3, 
               0x4, 
               0x5, 
               0x6, 
               0x0, // 8 special backspace
               0x0, // 9 special tab
               0x0, // a special \n
               0x7, 
               0x8,
               0x0, // a special \r
               0x9 
            };

            U8 remapped = remap[c];
            // Ignore if the color is greater than the specified max index:
            if ( remapped <= maxColorIndex )
            {
               const ColorI &clr = colorTable[remapped];
               sg_bitmapModulation = clr;
               currentColor = clr;
            }
         }
         continue;
      }

      // reset color?
      if ( c == 15 )
      {
         currentColor = sg_textAnchorColor;
         sg_bitmapModulation = sg_textAnchorColor;
         continue;
      }

      // push color:
      if ( c == 16 )
      {
         sg_stackColor = sg_bitmapModulation;
         continue;
      }

      // pop color:
      if ( c == 17 )
      {
         currentColor = sg_stackColor;
         sg_bitmapModulation = sg_stackColor;
         continue;
      }

      // Tab character
      if ( c == dT('\t') ) 
      {
          const PlatformFont::CharInfo &ci = font->getCharInfo( dT(' ') );
          pt.x += ci.xIncrement * GFont::TabWidthInSpaces;
          continue;
      }

      if( !font->isValidChar( c ) )  
         continue;

      const PlatformFont::CharInfo &ci = font->getCharInfo(c);

      if(ci.bitmapIndex == -1)
      {
         pt.x += ci.xOrigin + ci.xIncrement;
         continue;
      }

      TextureObject *newObj = font->getTextureHandle(ci.bitmapIndex);
      if(newObj != lastTexture)
      {
         if(currentPt)
         {
            BindTexture(lastTexture->getGLTextureName());
            DrawArrays(DGLQuads, 0, currentPt);
            currentPt = 0;
         }
         lastTexture = newObj;
      }
      if(ci.width != 0 && ci.height != 0)
      {
         pt.y = font->getBaseline() - ci.yOrigin;
         pt.x += ci.xOrigin;

         F32 texLeft   = F32(ci.xOffset)             / F32(lastTexture->getTextureWidth());
         F32 texRight  = F32(ci.xOffset + ci.width)  / F32(lastTexture->getTextureWidth());
         F32 texTop    = F32(ci.yOffset)             / F32(lastTexture->getTextureHeight());
         F32 texBottom = F32(ci.yOffset + ci.height) / F32(lastTexture->getTextureHeight());

         F32 screenLeft   = (F32)pt.x;
         F32 screenRight  = (F32)(pt.x + ci.width);
         F32 screenTop    = (F32)pt.y;
         F32 screenBottom = (F32)(pt.y + ci.height);

         points[0] = Point3F(screenLeft, screenBottom, 0.0);
         points[1] = Point3F(screenRight,  screenBottom, 0.0);
         points[2] = Point3F( screenRight,  screenTop, 0.0);
         points[3] = Point3F( screenLeft, screenTop, 0.0);

         for( int i=0; i<4; i++ )
         {
            rotMatrix.mulP( points[i] );
            points[i] += offset;
         }

         vert[currentPt++].set(points[0].x, points[0].y, texLeft, texBottom, currentColor);
         vert[currentPt++].set(points[1].x, points[1].y, texRight, texBottom, currentColor);
         vert[currentPt++].set(points[2].x, points[2].y, texRight, texTop, currentColor);
         vert[currentPt++].set(points[3].x, points[3].y, texLeft, texTop, currentColor);
         pt.x += ci.xIncrement - ci.xOrigin;
      }
      else
         pt.x += ci.xIncrement;
   }
   if(currentPt)
   {
      BindTexture(lastTexture->getGLTextureName());
      DrawArrays(DGLQuads, 0, currentPt);
   }

   DisableClientState ( DGLCSVertexArray );
   DisableClientState ( DGLCSColorArray );
   DisableClientState ( DGLCSTextCoordArray );

   DisableState(DGLRSBlend);
   DisableState(DGLRSTexture2D);

   pt.x += ptDraw.x; // DAW: Account for the fact that we removed the drawing point from the text start at the beginning.

   AssertFatal(pt.x >= ptDraw.x, "How did this happen?");
   PROFILE_END();

   return pt.x - ptDraw.x;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// Drawing primitives

void DGLDevice::DrawLine(S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color)
{

   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   SetColorI(color.red, color.green, color.blue, color.alpha);
   F32 verts[] = {
         (x1 + 0.5f), (y1 + 0.5f),
         (x2 + 0.5f), (y2 + 0.5f),
   };
    
   SetVertexPoint(2, 0, verts );

   DrawArrays(DGLLineList, 0, 2);

}

void DGLDevice::DrawLine(const Point2I &startPt, const Point2I &endPt, const ColorI &color)
{
    DrawLine(startPt.x, startPt.y, endPt.x, endPt.y, color);
}

void DGLDevice::DrawTriangleFill(const Point2I &pt1, const Point2I &pt2, const Point2I &pt3, const ColorI &color)
{
   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

	SetColorI(color.red, color.green, color.blue, color.alpha);
	F32 vertices[] = {
		(F32)pt1.x, (F32)pt1.y,
      (F32)pt2.x, (F32)pt2.y,
      (F32)pt3.x, (F32)pt3.y,
	};

   SetVertexPoint(2, 0, vertices);
	EnableClientState(DGLCSVertexArray);
   DrawArrays(DGLTriangleStrip, 0, 2);
}

void DGLDevice::DrawRect(const Point2I &upperL, const Point2I &lowerR, const ColorI &color, const float &lineWidth)
{
   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   SetLineWidth(lineWidth);

   SetColorI(color.red, color.green, color.blue, color.alpha);
   F32 vertices[] = {
       (F32)upperL.x, (F32)upperL.y,
       (F32)upperL.x, (F32)lowerR.y,
       (F32)lowerR.x, (F32)upperL.y,
       (F32)lowerR.x, (F32)lowerR.y,
   };

   SetVertexPoint(2, 0, vertices);
   EnableClientState(DGLCSVertexArray);
   DrawArrays(DGLLineLoop, 0, 4);

}

// the fill convention for lined rects is that they outline the rectangle border of the
// filled region specified.

void DGLDevice::DrawRect(const RectI &rect, const ColorI &color, const float &lineWidth)
{
   Point2I lowerR(rect.point.x + rect.extent.x - 1, rect.point.y + rect.extent.y - 1);
   DrawRect(rect.point, lowerR, color, lineWidth);
}

// the fill convention says that pixel at upperL will be filled and
// that pixel at lowerR will NOT be filled.

void DGLDevice::DrawRectFill(const Point2I &upperL, const Point2I &lowerR, const ColorI &color)
{
   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   SetColorI(color.red, color.green, color.blue, color.alpha);
    F32 vertices[] = {
        (F32)upperL.x, (F32)upperL.y,
        (F32)upperL.x, (F32)lowerR.y,
        (F32)lowerR.x, (F32)upperL.y,
        (F32)lowerR.x, (F32)lowerR.y,
    };

    SetVertexPoint(2, 0, vertices);
    EnableClientState(DGLCSVertexArray);
    DrawArrays(DGLTriangleStrip, 0, 4);
}
void DGLDevice::DrawRectFill(const RectI &rect, const ColorI &color)
{
   Point2I lowerR(rect.point.x + rect.extent.x, rect.point.y + rect.extent.y);
   DrawRectFill(rect.point, lowerR, color);
}

void DGLDevice::DrawQuadFill(const Point2I &point1, const Point2I &point2, const Point2I &point3, const Point2I &point4, const ColorI &color)
{
   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   SetColorI(color.red, color.green, color.blue, color.alpha);

	//Points 3 and 4 are switched by design.
	F32 vertices[] = {
      (F32)point1.x, (F32)point1.y,
      (F32)point2.x, (F32)point2.y,
      (F32)point4.x, (F32)point4.y,
      (F32)point3.x, (F32)point3.y,
	};

   SetVertexPoint(2, 0, vertices);
   EnableClientState(DGLCSVertexArray);
   DrawArrays(DGLTriangleStrip, 0, 4);

}

void DGLDevice::Draw2DSquare( const Point2F &screenPoint, F32 width, F32 spinAngle )
{
   width *= 0.5;

   MatrixF rotMatrix( EulerF( 0.0, 0.0, spinAngle ) );

   Point3F offset( screenPoint.x, screenPoint.y, 0.0 );
   Point3F points[4];

   points[0] = Point3F(-width, -width, 0.0);
   points[1] = Point3F(-width,  width, 0.0);
   points[2] = Point3F( width,  width, 0.0);
   points[3] = Point3F( width, -width, 0.0);

   for( int i=0; i<4; i++ )
   {
      rotMatrix.mulP( points[i] );
      points[i] += offset;
   }
   
    F32 verts[] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,//may need to switch last two
        1.0, 1.0,
    };

    F32 texVerts[] = {
        points[0].x, points[0].y,
        points[1].x, points[1].y,
        points[3].x, points[3].y,
        points[2].x, points[2].y,
    };
    
    DisableClientState(DGLCSColorArray);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    EnableClientState(DGLCSVertexArray);
    EnableClientState(DGLCSTextCoordArray);
    
    SetVertexPoint(2, 0, verts);
    SetTexPoint(2, 0, texVerts);
    DrawArrays(DGLTriangleFan, 0, 4);

}

void DGLDevice::DrawBillboard( const Point3F &position, F32 width, F32 spinAngle )
{
   MatrixF modelview;
   GetModelview( &modelview );
   modelview.transpose();


   width *= 0.5;
   Point3F points[4];
   points[0] = Point3F(-width, 0.0, -width);
   points[1] = Point3F(-width, 0.0,  width);
   points[2] = Point3F( width, 0.0,  width);
   points[3] = Point3F( width, 0.0, -width);


   MatrixF rotMatrix( EulerF( 0.0, spinAngle, 0.0 ) );

   for( int i=0; i<4; i++ )
   {
      rotMatrix.mulP( points[i] );
      modelview.mulP( points[i] );
      points[i] += position;
   }


    F32 verts[] = {
        0.0, 1.0,
        0.0, 0.0,
        1.0, 1.0,//may need to switch last two
        1.0, 0.0,
    };

    F32 texVerts[] = {
        points[0].x, points[0].y,
        points[1].x, points[1].y,
        points[3].x, points[3].y,
        points[2].x, points[2].y,
    };
    
    DisableClientState(DGLCSColorArray);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    EnableClientState(DGLCSVertexArray);
    EnableClientState(DGLCSTextCoordArray);
    
    SetVertexPoint(2, 0, verts);
    SetTexPoint(2, 0, texVerts);
    DrawArrays(DGLTriangleFan, 0, 4);

}

void DGLDevice::WireCube(const Point3F & extent, const Point3F & center)
{
   static Point3F cubePoints[8] =
   {
      Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
         Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
   };

   static U32 cubeFaces[6][4] =
   {
      { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
      { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
   };
   
   //glDisable(GL_CULL_FACE);
   DisableState(DGLRSCullFace);

//PUAP -Mat untested
   for (S32 i = 0; i < 6; i++)
   {
       F32 verts[] = {
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
       };
       int j = 0;
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         //glVertex3f(cubePoints[idx].x * extent.x + center.x,
            //cubePoints[idx].y * extent.y + center.y,
            //cubePoints[idx].z * extent.z + center.z);
          verts[j] =	 cubePoints[idx].x * extent.x + center.x;
          verts[++j] = cubePoints[idx].y * extent.y + center.y;
          verts[++j] = cubePoints[idx].z * extent.z + center.z;
      }
       //glVertexPointer(3, GL_FLOAT, 0, verts);
       //glDrawArrays(GL_LINE_LOOP, 0, 4);

       SetVertexPoint(3, 0, verts);
       DrawArrays(DGLLineLoop, 0, 4);
   }

}


void DGLDevice::SolidCube(const Point3F & extent, const Point3F & center)
{
   static Point3F cubePoints[8] =
   {
      Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
         Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
   };

   static U32 cubeFaces[6][4] =
   {
      { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
      { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
   };
   
   for (S32 i = 0; i < 6; i++)
   {
       F32 verts[] = {
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
       };
       int j = 0;
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         //glVertex3f(cubePoints[idx].x * extent.x + center.x,
            //cubePoints[idx].y * extent.y + center.y,
            //cubePoints[idx].z * extent.z + center.z);
          verts[j] = cubePoints[idx].x * extent.x + center.x;
          verts[++j] = cubePoints[idx].y * extent.y + center.y;
          verts[++j] = cubePoints[idx].z * extent.z + center.z;
      }
       //glVertexPointer(3, GL_FLOAT, 0, verts);
       //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

       SetVertexPoint(3, 0, verts);
       DrawArrays(DGLTriangleFan, 0, 4);
   }

}

//Draws an unfilled circle with line segments.
//Circle drawing code was modified from this source with gratitude. It is in the public domain.
//http://slabode.exofire.net/circle_draw.shtml
void DGLDevice::DrawCircle(const Point2I &center, const F32 radius, const ColorI &color, const F32 &lineWidth)
{
	F32 adjustedRadius = radius - (lineWidth/2);
	const S32 num_segments = (const S32)round(10 * sqrtf(adjustedRadius));
	F32 theta = 2 * 3.1415926f / F32(num_segments);
	F32 c = cosf(theta);//precalculate the sine and cosine
	F32 s = sinf(theta);
	F32 t;

	F32 x = adjustedRadius;//we start at angle = 0 
	F32 y = 0;

   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   SetLineWidth(lineWidth);

   SetColorI(color.red, color.green, color.blue, color.alpha);

   vector<F32> verts;
   for (int ii = 0; ii < num_segments; ii++)
   {
      verts.push_back(F32(x + center.x));
      verts.push_back(F32(y + center.y));

      //apply the rotation matrix
      t = x;
      x = c * x - s * y;
      y = s * t + c * y;
   }
   verts.push_back(F32(verts[0]));
   verts.push_back(F32(verts[1]));

   EnableClientState(DGLCSVertexArray);
   SetVertexPoint(2, 0, verts.data());
   DrawArrays(DGLLineLoop, 0, num_segments + 1);
   DisableClientState(DGLCSVertexArray);

}

void DGLDevice::DrawCircleFill(const Point2I &center, const F32 radius, const ColorI &color)
{
   
	const S32 num_segments = (const S32)round(10 * sqrtf(radius));
	F32 theta = 2 * 3.1415926f / F32(num_segments);
	F32 c = cosf(theta);//precalculate the sine and cosine
	F32 s = sinf(theta);
	F32 t;

	F32 x = radius;//we start at angle = 0 
	F32 y = 0;

   EnableState(DGLRSBlend);
   setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
   DisableState(DGLRSTexture2D);

   SetColorI(color.red, color.green, color.blue, color.alpha);

   vector<F32> verts;
   verts.push_back(F32(center.x));
   verts.push_back(F32(center.y));
   for (int ii = 0; ii < num_segments; ii++)
   {
      verts.push_back(F32(x + center.x));
      verts.push_back(F32(y + center.y));

      //apply the rotation matrix
      t = x;
      x = c * x - s * y;
      y = s * t + c * y;
   }
   verts.push_back(F32(verts[2]));
   verts.push_back(F32(verts[3]));

   EnableClientState(DGLCSVertexArray);
   SetVertexPoint(2, 0, verts.data());
   DrawArrays(DGLTriangleFan, 0, num_segments + 1);
   DisableClientState(DGLCSVertexArray);

}

bool DGLDevice::PointToScreen( Point3F &point3D, Point3F &screenPoint )
{
   /*
   GLfloat       glMV[16];
   GLfloat       glPR[16];
   GLint          glVP[4];

   glGetFloatv(GL_PROJECTION_MATRIX, glPR);
   glGetFloatv(GL_MODELVIEW_MATRIX, glMV);

   RectI viewport;
   GetViewport(&viewport);

   glVP[0] = viewport.point.x;
   glVP[1] = viewport.point.y + viewport.extent.y;
   glVP[2] = viewport.extent.x;
   glVP[3] = -viewport.extent.y;

   MatrixF mv;
   GetModelview(&mv);
   MatrixF pr;
   GetProjection(&pr);

   F64 x, y, z;
// PUAP -Mat untested
   int result = gluProject( point3D.x, point3D.y, point3D.z, (const F64 *)&glMV, (const F64 *)&glPR, (const GLint *)&glVP, &x, &y, &z );

   screenPoint.x = x;
   screenPoint.y = y;
   screenPoint.z = z;

   */
   return (true);
}

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
GLfloat gVertexFloats[8];
GLfloat gTextureVerts[8];
#endif

//------------------------------------------------------------------------------
bool DGLDevice::prevRes()
{
   U32 resIndex;
   for (resIndex = mVideoModeList.size() - 1; resIndex > 0; resIndex--)
   {
      if (mVideoModeList[resIndex].bpp == smCurrentRes.bpp
         && mVideoModeList[resIndex].w <= smCurrentRes.w
         && mVideoModeList[resIndex].h != smCurrentRes.h)
         break;
   }

   if (mVideoModeList[resIndex].bpp == smCurrentRes.bpp)
      return(setResolution(mVideoModeList[resIndex].w, mVideoModeList[resIndex].h, mVideoModeList[resIndex].bpp));

   return(false);
}


//------------------------------------------------------------------------------
bool DGLDevice::nextRes()
{
   U32 resIndex;
   for (resIndex = 0; resIndex < (U32)mVideoModeList.size() - 1; resIndex++)
   {
      if (mVideoModeList[resIndex].bpp == smCurrentRes.bpp
         && mVideoModeList[resIndex].w >= smCurrentRes.w
         && mVideoModeList[resIndex].h != smCurrentRes.h)
         break;
   }

   if (mVideoModeList[resIndex].bpp == smCurrentRes.bpp)
      return(setResolution(mVideoModeList[resIndex].w, mVideoModeList[resIndex].h, mVideoModeList[resIndex].bpp));

   return(false);
}


//------------------------------------------------------------------------------
// This function returns a string containing all of the available resolutions for this device
// in the format "<bit depth> <width> <height>", separated by tabs.
//
const char* DGLDevice::getResolutionList()
{
   if (Con::getBoolVariable("$pref::Video::clipHigh", false))
      for (S32 i = mVideoModeList.size() - 1; i >= 0; --i)
         if (mVideoModeList[i].w > 1152 || mVideoModeList[i].h > 864)
            mVideoModeList.erase(i);

   if (Con::getBoolVariable("$pref::Video::only16", false))
      for (S32 i = mVideoModeList.size() - 1; i >= 0; --i)
         if (mVideoModeList[i].bpp == 32)
            mVideoModeList.erase(i);

   U32 resCount = mVideoModeList.size();
   if (resCount > 0)
   {
      char* tempBuffer = new char[resCount * 15];
      tempBuffer[0] = 0;
      for (U32 i = 0; i < resCount; i++)
      {
         char newString[15];
         dSprintf(newString, sizeof(newString), "%d %d %d\t", mVideoModeList[i].w, mVideoModeList[i].h, mVideoModeList[i].bpp);
         dStrcat(tempBuffer, newString);
      }
      tempBuffer[dStrlen(tempBuffer) - 1] = 0;

      char* returnString = Con::getReturnBuffer(dStrlen(tempBuffer) + 1);
      dStrcpy(returnString, tempBuffer);
      delete[] tempBuffer;

      return returnString;
   }

   return NULL;
}
