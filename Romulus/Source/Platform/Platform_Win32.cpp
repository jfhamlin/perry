#include "Platform/Platform.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sstream>

namespace romulus
{
namespace platform
{
// Dynamic library related functions.
DynamicLibraryHandle LoadDynamicLibrary(const std::string& libraryName)
{
    HMODULE handle = LoadLibraryA((libraryName + ".dll").c_str());
    return reinterpret_cast<DynamicLibraryHandle>(handle);
}

void FreeDynamicLibrary(DynamicLibraryHandle library)
{
    if (library)
        FreeLibrary(reinterpret_cast<HMODULE>(library));
}

void* FindProcedureAddress(DynamicLibraryHandle library, const std::string& procedureName)
{
    if (!library)
        return 0;

    return GetProcAddress(reinterpret_cast<HMODULE>(library), procedureName.c_str());
}

// Timer related functions.
namespace
{
double g_inversePerformanceFrequency;
class PerformanceFrequencyInitializer
{
public:

    PerformanceFrequencyInitializer()
    {
        __int64 frequency;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
        g_inversePerformanceFrequency = 1.0 / static_cast<double>(frequency);
    }
};
PerformanceFrequencyInitializer g_initializerPerformanceFrequency;
}

double GetTime()
{
    __int64 counter;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counter));
    return g_inversePerformanceFrequency * static_cast<double>(counter);
}

// Directory related functions.
std::string TranslateNeutralPath(const std::string& path)
{
    const char NeutralDirectorySeperator = ':';
    const char Win32DirectorySeperator = '\\';

    std::string platformPath = path;

    for (size_t i = 0; i < platformPath.size(); ++i)
        if (platformPath[i] == NeutralDirectorySeperator)
            platformPath[i] = Win32DirectorySeperator;

    return platformPath;
}

DirectoryContentList EnumerateFiles(const std::string& directoryPath)
{
    DirectoryContentList fileList;

    // Open the file handle.
    WIN32_FIND_DATAA findData;
    HANDLE dirHandle = FindFirstFileA(TranslateNeutralPath(directoryPath + "\\*").c_str(), &findData);

    if (dirHandle == INVALID_HANDLE_VALUE) // Invalid dir, return the empty list.
        return fileList;                   // Should probably do something smarter in this case.

    do
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        if (findData.cFileName[0] == '.')
            continue;

        fileList.push_back(findData.cFileName);
    }
    while (FindNextFileA(dirHandle, &findData));

    FindClose(dirHandle);

    return fileList;
}

DirectoryContentList EnumerateSubdirectories(const std::string& directoryPath)
{
    DirectoryContentList dirList;

    // Open the file handle.
    WIN32_FIND_DATAA findData;
    HANDLE dirHandle = FindFirstFileA(TranslateNeutralPath(directoryPath + "\\*").c_str(), &findData);

    if (dirHandle == INVALID_HANDLE_VALUE) // Invalid dir, return the empty list.
        return dirList;                   // Should probably do something smarter in this case.

    do
    {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (findData.cFileName[0] == '.')
            continue;

        dirList.push_back(findData.cFileName);
    }
    while (FindNextFileA(dirHandle, &findData));

    FindClose(dirHandle);

    return dirList;
}

/* windows.h #defines "CreateDirectory". Get rid of this definition. */
#undef CreateDirectory

bool CreateDirectory(const std::string& path)
{
    return true; // Not sure if this is right. Also, need to convert to wchar* CreateDirectoryW(path.c_str(), NULL);
}

bool DeleteDirectory(const std::string& path)
{
    return true;
}

void HandleAssertion(const char* error, const char* file, int line)
{
    std::stringstream text;
    text << "File: " << file << "\n"
        << "Line: " << line << "\n"
        << error;

    MessageBoxA(NULL, text.str().c_str(), "Assertion Failure",
        MB_OK | MB_ICONEXCLAMATION);
    exit(-1);
}

std::string Backtrace()
{
    return "Not yet implemented on Win32";
}

}
}
