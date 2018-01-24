#ifndef _BASICFILEMANAGER_H_
#define _BASICFILEMANAGER_H_

//! \file BasicFileManager.h
//! \brief Contains declaration for BasicFileManager.

#include "File/IFileManager.h"

namespace romulus
{
//! Simple native filesystem only file manager.
class BasicFileManager : public IFileManager
{
public:

    BasicFileManager(const std::string& rootPath = ".");
    virtual ~BasicFileManager();

    virtual bool ReadFile(const std::string& path, boost::shared_ptr<std::istream>& stream);
    virtual bool WriteFile(const std::string& path, const char* source, const size_t length);

    virtual bool CreateDirectory(const std::string& path);
    virtual bool DeleteDirectory(const std::string& path);
    virtual platform::DirectoryContentList QueryDirectory(const std::string& path);

private:

    std::string m_rootPath;
};
}

#endif // _BASICFILEMANAGER_H_
