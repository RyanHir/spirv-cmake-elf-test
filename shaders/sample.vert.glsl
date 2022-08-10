#version 460 core
#extension GL_GOOGLE_include_directive : require 
#include "include/foo.glsl"
layout (location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
