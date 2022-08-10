#ifndef GLSL_SSBO
#define GLSL_SSBO 1
layout(std430, binding = 3) buffer layoutName
{
    uint width, height, depth;
    float data[];
};

uint get_index(uint x, uint y, uint z) {
    uint row_stride = width * depth;
    return (y * row_stride) + (x * depth) + z;
}

float get_item(uint x, uint y, uint z) {
    return data[get_index(x, y, z)];
}
void set_item(uint x, uint y, uint z, float value) {
    data[get_index(x, y, z)] = value;
}

#endif