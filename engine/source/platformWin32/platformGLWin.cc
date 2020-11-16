#include "platform/platformGL.h"
#include "graphics/gl/glad/gladWGL.h"


namespace PlatformGL
{
   void setVSync(const int i)
   {
      if (gglHasWExtension(EXT_swap_control))
      {
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

            wglSwapIntervalEXT(i);
         }
      }
   }
}