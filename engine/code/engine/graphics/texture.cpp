#include "texture.h"

#include "../engine.h"

#define STB_IMAGE_IMPLEMENTATION 1
#include <stb_image.h>

#include <glad/gl.h>

static ssize ComputeMipLevels(s32 width, s32 height) {
    ssize res = 1;
    for (;;) {
        width = Max(1, width >> 1);
        height = Max(1, height >> 1);
        res += 1;
        if (width == 1 && height == 1) {
            return res;
        }
    }
}

Texture::Texture(s32 width, s32 height, s32 num_channels, u8* data) {
    Init(width, height, num_channels, data);
}

Texture::~Texture() {
    if (m_texture_id != 0) {
        glDeleteTextures(1, &m_texture_id);
        m_texture_id = 0;
    }
}

void Texture::Init(s32 width, s32 height, s32 num_channels, u8* data) {
    m_width = width;
    m_height = height;
    m_channels = num_channels;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_id);

    GLsizei level_count = ComputeMipLevels(width, height);

    glTextureStorage2D(m_texture_id, level_count, GL_RGBA8, width, height);

    glTextureSubImage2D(m_texture_id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(m_texture_id);

    glTextureParameteri(m_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

u32 Texture::GetId() const {
    return m_texture_id;
}

s32 Texture::GetWidth() const {
    return m_width;
}

s32 Texture::GetHeight() const {
    return m_height;
}

s32 Texture::GetChannels() const {
    return m_channels;
}

std::shared_ptr<Texture> Texture::Load(std::string_view path) {

    auto& fs = Engine::GetInstance().file_system;
    auto fullpath = fs.GetAssetsDir() / path;

    if (!std::filesystem::exists(fullpath)) {
        ERROR("Image not found: {}", fullpath.c_str());
        return nullptr;
    }

    s32 width, height, num_channels;
    u8* data = stbi_load(fullpath.c_str(), &width, &height, &num_channels, 4);
    if (!data) {
        ERROR("Failed to load image: {}", fullpath.c_str());
        return nullptr;
    }

    auto tex = std::make_shared<Texture>(width, height, 4, data);
    stbi_image_free(data);

    return tex;
}
