#include <fstream>
#include <iostream>
#include <string>
#include <complex>
#include <cstdlib>
#include <map>
#include <set>
#include <math.h>

#include <glog/logging.h>

#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <eigen3/Eigen/Dense>

#include "lib/seed/SeedsRevised.h"
#include "lib/seed/Tools.h"
#include "lib/cnpy/cnpy.h"

#include "reconstruction/option_manager.h"
#include "utils/logging.h"

#include "reconstruction/database.h"
#include "exe/simple_merge.h"
#include "semantics/superpixels.h"


int RunDatabase(int argc, char** argv)
{
	OptionManager options;

	options.Parse(argc, argv);

  std::cout << *options.database_path << std::endl;
  
	Database database(*options.database_path);

	std::cout << "database  " << database.NumImages() << std::endl;

	for (auto& it : database.database) {
    	std::cout << it << std::endl;
	}
	return EXIT_SUCCESS;
}


int RunSEED(int argc, char** argv)
{
	OptionManager options;
	options.Parse(argc, argv);
  
	SuperpixelsGenerator superpixels_generator(*options.database_path);
  superpixels_generator.Run();

	return EXIT_SUCCESS;
}


int RunSimpleMerge(int argc, char** argv)
{
	OptionManager options;
	options.Parse(argc, argv);
  
  // generate superpixels first 
	SuperpixelsGenerator superpixels_generator(*options.database_path);
  superpixels_generator.Run();

  // run simple merge
  SimpleMerge simple_merge(*options.database_path);
  simple_merge.Run();
	return EXIT_SUCCESS;
}


typedef std::function<int(int, char**)> command_func_t;

int ShowHelp(const std::vector<std::pair<std::string, command_func_t>>& commands) {
  std::cout << "-- Multi View Depth Completion and Scene Parsing " << std::endl;

  std::cout << "Usage:" << std::endl;
  std::cout << "  SceneParser [command] [options]" << std::endl << std::endl;

  std::cout << "Documentation:" << std::endl;
  std::cout << "  Not yet !!!!!!!!!!!!" << std::endl << std::endl;

  std::cout << "Example usage:" << std::endl;
  std::cout << "  SceneParser help [ -h, --help ]" << std::endl;
  std::cout << "  SceneParser database " << std::endl;

  std::cout << "  help" << std::endl;
  for (const auto& command : commands) {
    std::cout << "  " << command.first << std::endl;
  }
  std::cout << std::endl;

  return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv) {
  // Initialize Google's logging library.
  InitializeGlog(argv);

  std::vector<std::pair<std::string, command_func_t>> commands;

	// Commands parser

	commands.emplace_back("database", 	&RunDatabase);
	commands.emplace_back("superpixels", &RunSEED);
	commands.emplace_back("simple_merge", &RunSimpleMerge);

	if (argc == 1) {
    return ShowHelp(commands);
  }

  const std::string command = argv[1];
  if (command == "help" || command == "-h" || command == "--help") {
    return ShowHelp(commands);
  } else {
    command_func_t matched_command_func = nullptr;
    for (const auto& command_func : commands) {
      if (command == command_func.first) {
        matched_command_func = command_func.second;
        break;
      }
    }
    if (matched_command_func == nullptr) {
      std::cerr << "ERROR: Command not recognized. To list the available commands, run `SceneParser help`." << std::endl;
      return EXIT_FAILURE;
    } else {
      int command_argc = argc - 1;
      char** command_argv = &argv[1];
      command_argv[0] = argv[0];
      return matched_command_func(command_argc, command_argv);
    }
  }

  return ShowHelp(commands);
}

