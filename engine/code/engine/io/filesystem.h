#pragma once

#include "../base.h"

#include <filesystem>
#include <string>
#include <vector>

class FileSystem {
public:
    std::filesystem::path GetExecutableDir() const;
    std::filesystem::path GetAssetsDir() const;

    std::vector<u8> LoadFile(std::filesystem::path path) const;
    std::vector<u8> LoadAssetFile(std::string_view relpath) const;
    std::string     LoadAssetTextFile(std::string_view relpath) const;
};
