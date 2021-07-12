#include "checkup_graphics/shader_collection.h"

namespace checkup
{
    namespace graphics
    {
        const char* full_screen_quad_vs =
        "attribute vec4 vertexPosition;\n"
        "attribute vec2 vertexTextureCord;\n"
        "varying vec2 textureCord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vertexPosition;\n"
        "    textureCord = vertexTextureCord;\n"
        "}\n";

        const char* uniform_color_fs =
        "precision mediump float;\n"
        "uniform vec4 baseColor;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = baseColor;\n"
        "}\n";
    }
}