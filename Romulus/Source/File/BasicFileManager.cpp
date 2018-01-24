#include "File/BasicFileManager.h"
#include <fstream>

namespace romulus
{
BasicFileManager::BasicFileManager(const std::string& rootPath):
    m_rootPath(rootPath + ":")
{
    if (!platform::ValidateNeutralDirectoryPath(rootPath))
        throw InvalidPathException();
}

BasicFileManager::~BasicFileManager()
{
}

bool BasicFileManager::ReadFile(const std::string& path, boost::shared_ptr<std::istream>& stream)
{
    if (!platform::ValidateNeutralDirectoryPath(path))
        throw InvalidPathException();

    std::string platformPath = platform::TranslateNeutralPath(m_rootPath + path);
    std::ifstream* file = new std::ifstream(platformPath.c_str(),
        std::ios::in | std::ios::binary);
    stream.reset(file);

    return file->is_open();
}

bool BasicFileManager::WriteFile(const std::string& path, const char* source, const size_t length)
{
    if (!platform::ValidateNeutralDirectoryPath(path))
        throw InvalidPathException();

    std::string platformPath = platform::TranslateNeutralPath(m_rootPath + path);
    std::ofstream file(platformPath.c_str());
    if (!file.is_open())
        return false;

    file.write(source, static_cast<std::streamsize>(length));
    file.close();
    return true;
}

bool BasicFileManager::CreateDirectory(const std::string& path)
{
    if (!platform::ValidateNeutralDirectoryPath(path))
        throw InvalidPathException();

    return platform::CreateDirectory(m_rootPath + path);
}

bool BasicFileManager::DeleteDirectory(const std::string& path)
{
    if (!platform::ValidateNeutralDirectoryPath(path))
        throw InvalidPathException();

    return platform::DeleteDirectory(m_rootPath + path);
}

platform::DirectoryContentList BasicFileManager::QueryDirectory(const std::string& path)
{
    if (!platform::ValidateNeutralDirectoryPath(path))
        throw InvalidPathException();

    platform::DirectoryContentList contents;

    platform::DirectoryContentList temp = platform::EnumerateFiles(m_rootPath + path);
    for (size_t i = 0; i < temp.size(); ++i)
    contents.push_back(temp[i]);

    temp = platform::EnumerateSubdirectories(m_rootPath + path);
    for (size_t i = 0; i < temp.size(); ++i)
        contents.push_back(temp[i]);

    return contents;
}
}
