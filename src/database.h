

#ifndef DATABASE_H_
#define DATABASE_H_


#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <string>


class Database {
public:

	std::vector<std::string> 	nameList;
	std::vector<std::string> 	suffixList;

	std::string 				inputDir;
	std::string 				ext;

	std::string 				outputDir;

	Database();

	~Database();

	bool checkFile(const std::string & file);

	std::string getExtention(const std::string & file);

	// read Frames Names List File
	void readNameList();

	// Read path of inputs depending on chosen suffix
	std::vector< boost::filesystem::path > getPathList (unsigned int suffix);

};

#endif /* DATABASE_H_ */

