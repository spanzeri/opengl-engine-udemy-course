#pragma once

#include "../base.h"

#include <memory>

class Texture {
public:
    Texture(s32 width, s32 height, s32 num_channels, u8* data);
    ~Texture();

    void Init(s32 width, s32 height, s32 num_channels, u8* data);

    u32 GetId() const;

    s32 GetWidth() const;
    s32 GetHeight() const;
    s32 GetChannels() const;

    static std::shared_ptr<Texture> Load(std::string_view path);

private:
    s32 m_width;
    s32 m_height;
    s32 m_channels;
    u32 m_texture_id = 0;
};
