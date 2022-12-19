#ifndef MISC_H_
#define MISC_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>


void SplitFileExtension(const std::string& path, std::string* root,
                        std::string* ext);

// Check if the path points to an existing directory.
bool ExistsFile(const std::string& path);

// Check if the path points to an existing directory.
bool ExistsDir(const std::string& path);

// Check if the path points to an existing file or directory.
bool ExistsPath(const std::string& path);

// Create the directory if it does not exist.
void CreateDirIfNotExists(const std::string& path);

// Join multiple paths into one path.
template <typename... T>
std::string JoinPaths(T const&... paths);

// Return list of files in directory.
std::vector<std::string> GetFileList(const std::string& path);

// Return list of directories, recursively in all sub-directories.
std::vector<std::string> GetDirList(const std::string& path);

// Extract the base name of a path, e.g., "image.jpg" for "/dir/image.jpg".
std::string GetPathBaseName(const std::string& path);

// Extract the patrent name of a path,
std::string GetParentPath(const std::string& path);


// Read each line of a text file into a separate element. Empty lines are
// ignored and leading/trailing whitespace is removed.
std::vector<std::string> ReadTextFileLines(const std::string& path);

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////


template <typename... T>
std::string JoinPaths(T const&... paths) {
  boost::filesystem::path result;
  int unpack[]{0, (result = result / boost::filesystem::path(paths), 0)...};
  static_cast<void>(unpack);
  return result.string();
}
#endif // MISC_H_