#include "util.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

void clear_colour(colour c) {
    glClearColor(c.r, c.g, c.b, c.a);
}
