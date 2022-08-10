#version 460 core
#extension GL_GOOGLE_include_directive : require 
#include "include/ssbo.glsl"

void main()
{
    set_item(300, 8, 2, 42);
}