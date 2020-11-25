#include "gladGL.h"

#include "console/console.h"

#ifdef TORQUE_OS_WIN32
   #include "gladWGL.h"
#endif // TORQUE_OS_WIN32

#include <dlfcn.h>

namespace GLAD
{
    static void* open_handle(const char *lib_names[], int length)
    {
        int i;
        void *handle;

        for (i = 0; i < length; ++i) {
            handle = dlopen(lib_names[i], RTLD_LAZY | RTLD_GLOBAL);
            if (handle != NULL) {
                return handle;
            }
        }

        return NULL;
    }

    static void close_handle(void* handle)
    {
        if (handle != NULL) {
            dlclose(handle);
        }
    }

    static void* dlsym_handle(void* handle, const char *name)
    {
        if (handle == NULL) {
            return NULL;
        }

        return dlsym(handle, name);
    }

    static void* global_egl_handle;

    static void* load_with_egl(const char *name)
    {
        void* result = dlsym_handle(global_egl_handle, name);
        if (result == NULL)
        {
            result = (void*)eglGetProcAddress(name);
        }
        return result;
    }

    void gglPerformBinds()
   {
       if(!gladLoadEGL())
       {
           AssertFatal(false,"Failed to load GLAD!");
       }

       static const char *NAMES[] = {"libGLESv2.so", "libEGL.so"};

       global_egl_handle = open_handle(NAMES, sizeof(NAMES) / sizeof(NAMES[0]));
       if (global_egl_handle)
       {
           adprintf("We have one of the libs");
           gladLoadGLES2Loader((GLADloadproc) load_with_egl);
           close_handle(global_egl_handle);
       }

   }

   void gglPerformExtensionBinds(void *context)
   {
   #ifdef TORQUE_OS_WIN32
      gladLoadWGL(wglGetCurrentDC());
   #endif // TORQUE_OS_WIN32
   }
}