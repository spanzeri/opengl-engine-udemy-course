#pragma once

#include "../base.h"

#include <vector>

enum {
    TYPE_F32 = 0x1406,      // GL_FLOAT
};

struct VertexElement {
    u32 index;  // attribute location
    u32 size;   // number of components
    u32 type;   // data type
    u32 offset; // bytes offset from start of the vertex
};

struct VertexLayout {
    std::vector<VertexElement> elements;
    u32 stride;     // total size of an entire vertex
};

