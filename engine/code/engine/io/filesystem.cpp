#include "filesystem.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#endif

#include <fstream>

std::filesystem::path FileSystem::GetExecutableDir() const {
#if defined(_WIN32)
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(NULL, buf, MAX_PATH);
    return std::filesystem::path(buf).remove_filename();
#elif defined(__APPLE__)
    u32 size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string tmp(size, '\0');
    _NSGetExecutablePath(tmp.data(), &size);
    return std::filesystem::weakly_canonical(std::filesystem::path(tmp)).remove_filename();
#elif defined(__linux__)
    return std::filesystem::weakly_canonical(std::filesystem::read_symlink("/proc/self/exe")).remove_filename();
#else
    return std::filesystem::current_path();
#endif
}

std::filesystem::path FileSystem::GetAssetsDir() const {
    return std::filesystem::weakly_canonical(std::filesystem::path(RESOURCE_PATH));
}

std::vector<u8> FileSystem::LoadFile(std::filesystem::path path) const {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        return {};
    }

    auto size = file.tellg();
    file.seekg(0);

    auto buffer = std::vector<u8>(size);
    if (!file.read((char*)buffer.data(), buffer.size())) {
        return {};
    }

    return buffer;
}

std::vector<u8> FileSystem::LoadAssetFile(std::string_view relpath) const {
    return LoadFile(GetAssetsDir() / relpath);
}

std::string FileSystem::LoadAssetTextFile(std::string_view relpath) const {
    auto buf = LoadFile(GetAssetsDir() / relpath);
    return std::string(buf.begin(), buf.end());
}

