#include "buffer.h"

#include <glad/gl.h>

Buffer::Buffer(Type type, const void* data, usize size, bool dynamic) {
    glCreateBuffers(1, &m_id);

    GLbitfield flags = dynamic ? GL_DYNAMIC_STORAGE_BIT : 0;
    glNamedBufferStorage(m_id, size, data, flags);

    switch (type) {
        case Buffer::Type::Vertex:  m_target = GL_ARRAY_BUFFER;         break;
        case Buffer::Type::Index:   m_target = GL_ELEMENT_ARRAY_BUFFER; break;
        case Buffer::Type::Uniform: m_target = GL_UNIFORM_BUFFER;       break;
    }
}

Buffer::~Buffer() {
    if (m_id) {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
        m_size = 0;
    }
}

Buffer::Buffer(Buffer&& other)
    : m_id(other.m_id), m_target(other.m_target), m_size(other.m_size)
{
    other.m_id = 0;
    other.m_size = 0;
}

Buffer& Buffer::operator=(Buffer&& other) {
    if (this != &other) {
        if (m_id != 0) {
            glDeleteBuffers(1, &m_id);
        }
        m_id     = other.m_id;
        m_size   = other.m_size;
        m_target = other.m_target;

        other.m_id   = 0;
        other.m_size = 0;
    }
    return *this;
}

void Buffer::Update(const void* data, usize size, usize offset) {
    ASSERT(offset + size <= m_size, "Buffer::Update out of bound");
    if (m_id) {
        glNamedBufferSubData(m_id, (GLintptr)offset, (GLsizeiptr)size, data);
    }
}

void Buffer::Bind() const {
    if (m_id) {
        glBindBuffer(m_target, m_id);
    }
}

void Buffer::BindBase(u32 index) const {
    if (m_id) {
        glBindBufferBase(m_target, index, m_id);
    }
}

u32 Buffer::GetId() const {
    return m_id;
}

usize Buffer::GetSize() const {
    return m_size;
}

bool Buffer::IsValid() const {
    return m_id != 0;
}
