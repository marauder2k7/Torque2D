#ifndef GLADWGL_H_
#define GLADWGL_H_

#include "platform/platform.h"

#include "gladGL.h"
#include <glad/glad_wgl.h>

#define gglHasWExtension(EXTENSION) GLAD_WGL_##EXTENSION

#endif // !GLADWGL_H_
