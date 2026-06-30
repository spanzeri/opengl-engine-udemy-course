#pragma once

#include "../base.h"

#include <vector>

struct VertexElement {
    enum struct Type : u32 {
        F32 = 0x1406,   // Matches GL_FLOAT
    };

    u32     index  = 0;           // attribute location
    u32     size   = 0;           // number of components
    Type    type   = Type::F32;   // data type
    u32     offset = 0;           // bytes offset from start of the vertex

    static constexpr s32 PositionIndex = 0;
    static constexpr s32 ColorIndex    = 1;
    static constexpr s32 NormalIndex   = 2;
    static constexpr s32 UVIndex       = 3;
};

struct VertexLayout {
    std::vector<VertexElement> elements;
    u32 stride = 0;     // total size of an entire vertex
};

