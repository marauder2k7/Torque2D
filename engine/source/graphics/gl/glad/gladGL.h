#ifndef GLADGL_H_
#define GLADGL_H_

#include <glad/glad.h>

/// Extensions are cachedn into simple booleans (ints) T3D:JTH
#define gglHasExtension(EXTENSION) GLAD_GL_##EXTENSION

#endif // !GLADGL_H_
