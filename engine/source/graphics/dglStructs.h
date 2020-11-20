#ifndef _DGLENUMS_H_
#include "graphics/dglEnums.h"
#endif
#ifndef _MMATH_H_
#include "math/mMath.h"
#endif // !1


enum devices
{
   NO_DEVICE = -1,
   OPENGL_DEVICE,
   VOODOO2_DEVICE,
   N_DEVICES
};

struct DGLVideoMode
{
   U32 w, h, bpp;

   DGLVideoMode(U32 _w = 0, U32 _h = 0, U32 _bpp = 0)
   {
      w = _w;
      h = _h;
      bpp = _bpp;
   }

   bool operator==(const DGLVideoMode& otherRes) const
   {
      return ((w == otherRes.w) && (h == otherRes.h) && (bpp == otherRes.bpp));
   }

   void operator=(const DGLVideoMode& otherRes)
   {
      w = otherRes.w;
      h = otherRes.h;
      bpp = otherRes.bpp;
   }
};