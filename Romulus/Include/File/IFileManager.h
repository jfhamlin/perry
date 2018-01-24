#ifndef _IFILEMANAGER_H_
#define _IFILEMANAGER_H_

#include "Platform/Platform.h"
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>

namespace romulus
{
class InvalidPathException : public std::exception
{	
};

//! Interface for file/directory operations.
class IFileManager
{
public:

    IFileManager() { }
    virtual ~IFileManager() { }

    //! Attempt to open a file for reading.
    //! \param path - Path to the file.
    //! \param stream - The stream pointer to assign.
    //! \return True on success.
    virtual bool ReadFile(const std::string& path, boost::shared_ptr<std::istream>& stream) = 0;
    //! Attempt to write a file.
    //! \param path - Path to the file.
    //! \param source - Address of data to write.
    //! \param length - Length of the data.
    //! \return True on success.
    virtual bool WriteFile(const std::string& path, const char* source, const size_t length) = 0;

    //! Attempt to create the directory specified by path.
    //! \param path - The directory to create.
    //! \return True on success.
    virtual bool CreateDirectory(const std::string& path) = 0;
    //! Attempt to delete a directory.
    //! \param path - The directory to delete.
    //! \return True on success.
    virtual bool DeleteDirectory(const std::string& path) = 0;

    //! Lookup a directory's contents.
    //! \param path - The directory to query.
    //! \return A list of directory contents.
    virtual platform::DirectoryContentList QueryDirectory(const std::string& path) = 0;
};
}

#endif // _IFILEMANAGER_H_
