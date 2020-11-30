#include "gladGL.h"

#include "console/console.h"

#ifdef TORQUE_OS_WIN32
   #include "gladWGL.h"
#endif // TORQUE_OS_WIN32


namespace GLAD
{
    void gglPerformBinds()
   {
       if(!gladLoadGL())
       {
           AssertFatal(false,"Failed to load GLAD!");
       }

   }

   void gglPerformExtensionBinds(void *context)
   {
   #ifdef TORQUE_OS_WIN32
      gladLoadWGL(wglGetCurrentDC());
   #endif // TORQUE_OS_WIN32
   }
}