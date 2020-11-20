#include "platform/platformGL.h"
#include "graphics/gl/glad/gladWGL.h"


void PlatformGL::setVSync(const int i)
{
   if (gglHasWExtension(EXT_swap_control))
   {
      // NVidia needs to have the default framebuffer bound or the vsync calls fail
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      if (gglHasWExtension(EXT_swap_control_tear))
      {
         if (i == 1 || i == -1)
         {
            BOOL ret = wglSwapIntervalEXT(-1);

            if (!ret)
               wglSwapIntervalEXT(1);
         }
         else
         {
            wglSwapIntervalEXT(i);
         }
         return;
      }
      //fallback with no EXT_swap_control_tear
      wglSwapIntervalEXT(i);
   }
}