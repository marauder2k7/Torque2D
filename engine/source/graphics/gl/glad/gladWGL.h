#ifndef _GLADWGL_H_
#define _GLADWGL_H_

#include "platform/platform.h"

#include "gladGL.h"
#include <glad/glad_wgl.h>

#define gglHasWExtension(EXTENSION) GLAD_WGL_##EXTENSION

#endif // !_GLADWGL_H_
