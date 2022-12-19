#ifndef OPTION_MANAGER_H_
#define OPTION_MANAGER_H_

#include <memory>

#include <boost/program_options.hpp>

class OptionManager {
  public:
  
    OptionManager(bool add_project_options = true);

    void Reset();

    void Parse(const int argc, char** argv);
    bool Read(const std::string& path);

    std::shared_ptr<std::string> project_path;
    std::shared_ptr<std::string> workspace_path;
    std::shared_ptr<std::string> database_path;

  private:
    std::shared_ptr<boost::program_options::options_description> desc_;

};

#endif  // OPTION_MANAGER_H
