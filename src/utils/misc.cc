#include "misc.h"
#include <cstdarg>
#include <boost/algorithm/string.hpp>


bool ExistsFile(const std::string& path) {
  return boost::filesystem::is_regular_file(path);
}


bool ExistsDir(const std::string& path) {
  return boost::filesystem::is_directory(path);
}


bool ExistsPath(const std::string& path) {
  return boost::filesystem::exists(path);
}


void CreateDirIfNotExists(const std::string& path) {
  if (!ExistsDir(path)) 
  {
    boost::filesystem::create_directory(path);
  }
}


std::vector<std::string> ReadTextFileLines(const std::string& path) {
  std::ifstream file(path);

  std::string line;
  std::vector<std::string> lines;
  
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    lines.push_back(line);
  }
  
  return lines;
}


std::vector<std::string> GetFileList(const std::string& path) {
  std::vector<std::string> file_list;
  for (auto it = boost::filesystem::directory_iterator(path);
       it != boost::filesystem::directory_iterator(); ++it) {
    if (boost::filesystem::is_regular_file(*it)) {
      const boost::filesystem::path file_path = *it;
      file_list.push_back(file_path.string());
    }
  }
  return file_list;
}


std::vector<std::string> GetDirList(const std::string& path) {
  std::vector<std::string> dir_list;
  for (auto it = boost::filesystem::directory_iterator(path);
       it != boost::filesystem::directory_iterator(); ++it) {
    if (boost::filesystem::is_directory(*it)) {
      const boost::filesystem::path dir_path = *it;
      dir_list.push_back(dir_path.string());
    }
  }
  return dir_list;
}


std::string GetPathBaseName(const std::string& path) {
  boost::filesystem::path _path(path);
  
  return _path.stem().c_str();
}

std::string GetParentPath(const std::string& path){
  boost::filesystem::path _path(path);

  return _path.parent_path().c_str();
}