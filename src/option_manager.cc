#include <iostream>

#include "option_manager.h"

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "misc.h"

namespace config = boost::program_options;
namespace fs = boost::filesystem;

OptionManager::OptionManager(bool add_project_options) {
  
  project_path.reset(new std::string());
  workspace_path.reset(new std::string());
  database_path.reset(new std::string());
  
  Reset();

  desc_->add_options()("help,h", "");

  if (add_project_options) {
    desc_->add_options()("project_path", config::value<std::string>());
  }
}

void OptionManager::Reset() {
  desc_.reset(new boost::program_options::options_description());
}

void OptionManager::Parse(const int argc, char** argv) {
  config::variables_map vmap;

  try {
    config::store(config::parse_command_line(argc, argv, *desc_), vmap);

    if (vmap.count("help")) 
    {
      std::cout << "Options can be specified via --project_path" << std::endl;
      
      std::cout << *desc_ << std::endl;
      
      exit(EXIT_SUCCESS);
    }

    if (vmap.count("project_path")) 
    {
      *project_path = vmap["project_path"].as<std::string>();

      if (!ExistsDir(*project_path))
      {
        std::cerr << "ERROR: project_path not found :" << *project_path << "." << std::endl;
        exit(EXIT_FAILURE);
      }

      const std::string  stereo_path= "/dense/stereo";
      *workspace_path = JoinPaths(*project_path, stereo_path);
      
      if (!ExistsDir(*workspace_path))
      {
        std::cerr << "ERROR: workspace_path folder not found :" << *workspace_path << "." << std::endl;
        exit(EXIT_FAILURE);
      }

      const std::string db_path= "/dense/database.txt";
      *database_path = JoinPaths(*project_path, db_path);

      if (!ExistsFile(*database_path))
      {
        std::cerr << "ERROR: database_path file not found :" << *database_path << "." << std::endl;
        exit(EXIT_FAILURE);
      }
      
    } 
    else 
    {
      vmap.notify();
    }
  } catch (std::exception& exc) 
  {
    std::cerr << "ERROR: Failed to parse options - " << exc.what() << "." << std::endl;
    exit(EXIT_FAILURE);
  } catch (...) {
    
    std::cerr << "ERROR: Failed to parse options for unknown reason." << std::endl;
    
    exit(EXIT_FAILURE);
  }

}

