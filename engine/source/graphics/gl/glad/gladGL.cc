#include "gladGL.h"

#include "console/console.h"

#ifdef TORQUE_OS_WIN32
   #include "gladWGL.h"
#endif // TORQUE_OS_WIN32


namespace GLAD
{
   void gglPerformBinds()
   {
      if (!gladLoadGL())
      {
         AssertFatal(false, "Unable to load GLAD.");
      }
   }

   void gglPerformExtensionBinds(void *context)
   {
      if (!gladLoadWGL(wglGetCurrentDC()))
      {
         AssertFatal(false, "Unable to load GLAD WGL extensions. Make sure your OpenGL drivers are up to date!");
      }
   }
}