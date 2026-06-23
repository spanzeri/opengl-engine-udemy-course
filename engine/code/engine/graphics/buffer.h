#pragma once

#include "../base.h"

class Buffer : NonCopiable {
public:
    enum struct Type { Vertex, Index, Uniform };

    Buffer() = default;
    Buffer(Type type, const void* data, usize size, bool dynamic = false);
    template <typename T>
    Buffer(Type type, std::span<T> data, bool dynamic = false);
    ~Buffer();

    Buffer(Buffer&& other);
    Buffer& operator=(Buffer&& other);

    void Update(const void* data, usize size, usize offset = 0);

    void Bind() const;
    void BindBase(u32 index) const;

    u32 GetId() const;
    usize GetSize() const;
    bool IsValid() const;

private:
    u32   m_id     = 0;
    u32   m_target = 0;
    usize m_size   = 0;
};

template <typename T>
Buffer::Buffer(Buffer::Type type, std::span<T> data, bool dynamic)
    : Buffer(type, data.data(), data.size_bytes(), dynamic) {
}

