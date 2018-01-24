#ifndef _PLATFORM_H_
#define _PLATFORM_H_

//! \file Platform.h
//! Contains the platform abstraction interface.

#include <string>
#include <vector>

namespace romulus
{
namespace platform
{
// Dynamic library related functions.
typedef void* DynamicLibraryHandle;

//! Attempt to load a dynamic link library.
//! \param libraryName - The name of the library to load, less extension.
//! \return A handle to the loaded library, or 0 on failure.
DynamicLibraryHandle LoadDynamicLibrary(const std::string& libraryName);

//! Free a loaded dynamic link library.
//! \param handle - A handle to the library.
void FreeDynamicLibrary(DynamicLibraryHandle library);

//! Attempt to find a procedure's address.
//! \param library - A handle to the library to search.
//! \param procedureName - The name of the procedure to find.
//! \return The procedure's address, or 0 on failure.
void* FindProcedureAddress(DynamicLibraryHandle library, const std::string& procedureName);

// Timer related functions.
//! Retrieve the current time, in seconds.
//! Note that this value does not necessarily represent the system
//! time.
//! \return The time value, in seconds.
double GetTime();

// Directory related functions.
typedef std::vector<std::string> DirectoryContentList;

//! Validate a neutral directory path.
//! \param path - The path to validate.
//! \return True if the path is valid.
inline bool ValidateNeutralDirectoryPath(const std::string& path)
{
    size_t index = path.find_first_of('\\');
    if (index != std::string::npos)
        return false;

    index = path.find_first_of('/');
    if (index != std::string::npos)
        return false;

    return path[path.size() - 1] != ':';
}

//! Translate a neutral path into a platform specific path.
//! \param path - The neutral path.
//! \return The platform specific path.
std::string TranslateNeutralPath(const std::string& path);

//! Enumerates the files in a given directory.
//! \param directoryPath - The directory to search.
//! \return A list of the files in said directory.
DirectoryContentList EnumerateFiles(const std::string& directoryPath);

//! Enumerates subdirectories of a given directory.
//! \param directoryPath - The directory to search.
//! \return A list of the subdirectories in said directory.
DirectoryContentList EnumerateSubdirectories(const std::string& directoryPath);

//! Attempt to create a directory.
//! \param path - Path to the directory.
//! \return True on success.
bool CreateDirectory(const std::string& path);

//! Attempt to delete a directory.
//! \param path - Path to the directory;
//! \return True on success.
bool DeleteDirectory(const std::string& path);

void HandleAssertion(const char* error, const char* file, int line);

//! Return a backtrace of the current thread.
std::string Backtrace();
}
}

#endif // _PLATFORM_H_

