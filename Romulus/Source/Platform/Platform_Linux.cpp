#include "Platform/Platform.h"
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <execinfo.h>
#include <iostream>
#include <stdlib.h>

namespace romulus
{
namespace platform
{
// Dynamic library related functions.
namespace
{
std::string GetCurrentDirectory()
{
    char* currentWorkingDir = getcwd(0, 0);
    std::string dir = currentWorkingDir;
    free(currentWorkingDir);
    return dir;
}
}

DynamicLibraryHandle LoadDynamicLibrary(const std::string& libraryName)
{
    std::string workingDir = GetCurrentDirectory();
    if (workingDir[workingDir.size() - 1] != '/')
        workingDir += '/';

    return dlopen((workingDir + libraryName + ".so").c_str(), RTLD_NOW);
}

void FreeDynamicLibrary(DynamicLibraryHandle library)
{
    if (!library)
        return;

    dlclose(library);
}

void* FindProcedureAddress(DynamicLibraryHandle library, const std::string& procedureName)
{
    if (!library)
        return 0;

    return dlsym(library, procedureName.c_str());
}

// Timer related functions.
double GetTime()
{
    // We'll just use the time since the UNIX epoch.
    timeval tv;
    gettimeofday(&tv, 0);
    return (double)tv.tv_sec + ((double)tv.tv_usec * 0.000001);
}

// Directory related functions.
std::string TranslateNeutralPath(const std::string& path)
{
    const char NeutralDirectorySeperator = ':';
    const char LinuxDirectorySeperator = '/';

    std::string platformPath = path;

    for (size_t i = 0; i < platformPath.size(); ++i)
        if (platformPath[i] == NeutralDirectorySeperator)
            platformPath[i] = LinuxDirectorySeperator;

    return platformPath;
}

DirectoryContentList EnumerateFiles(const std::string& directoryPath)
{
    std::string unixDir = TranslateNeutralPath(directoryPath);

    // Attemp to open the directory.
    DIR *dir = opendir(unixDir.c_str());
    if (!dir) // The directory is invalid.
        return DirectoryContentList();

    DirectoryContentList fileList;

    unixDir += '/';

    // Now just enumerate through the directory.
    dirent* entry = readdir(dir);
    while (entry)
    {
        if (entry->d_name[0] == '.') // Omit any entries that begin with a dot.
        {
            entry = readdir(dir);
            continue;
        }

        struct stat info;
        stat((unixDir + entry->d_name).c_str(), &info);

        if (S_ISDIR(info.st_mode) == 0)
            fileList.push_back(entry->d_name);

        entry = readdir(dir);
    }

    closedir(dir);

    return fileList;
}

DirectoryContentList EnumerateSubdirectories(const std::string& directoryPath)
{
    std::string unixDir = TranslateNeutralPath(directoryPath);

    // Attempt to open the directory.
    DIR *dir = opendir(unixDir.c_str());
    if (!dir) // The directory is invalid.
        return DirectoryContentList();

    DirectoryContentList fileList;

    unixDir += '/';

    // Now just enumerate through the directory.
    dirent* entry = readdir(dir);
    while (entry)
    {
        if (entry->d_name[0] == '.') // Omit any entries that begin with a dot.
        {
            entry = readdir(dir);
            continue;
        }

        struct stat info;
        stat((unixDir + entry->d_name).c_str(), &info);

        if (S_ISDIR(info.st_mode))
            fileList.push_back(entry->d_name);

        entry = readdir(dir);
    }

    closedir(dir);

    return fileList;
}

bool CreateDirectory(const std::string& path)
{
    std::string unixPath = TranslateNeutralPath(path);

    return mkdir(unixPath.c_str(), S_IRWXU) == 0;
}

bool DeleteDirectory(const std::string& path)
{
    std::string unixPath = TranslateNeutralPath(path);
    return rmdir(unixPath.c_str()) == 0;
}

void HandleAssertion(const char* error, const char* file, int line)
{
    std::cerr << "Assertion Error! \n"
              << "Error: " << error << "\n"
              << "File: " << file << "\n"
              << "Line: " << line << "\n"
              << "Backtrace: " << "\n" << Backtrace()
              << std::endl;
    exit(-1);
}

std::string Backtrace()
{
    std::string trace;

    const int depth = 10;
    void* array[depth];
    size_t size;
    char** strings;
    size_t i;

    size = backtrace (array, depth);
    strings = backtrace_symbols (array, size);

    for (i = 1; i < size; i++)
        trace += std::string(strings[i]) + "\n";

    free (strings);

    return trace;
}

}
}

