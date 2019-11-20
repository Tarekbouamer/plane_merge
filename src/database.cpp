
#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <string>

#include "database.h"

Database::Database() {
		suffixList.push_back(".JPG");                      //0

		suffixList.push_back("_plane_masks_.npy");         //1

		suffixList.push_back("_labels_.png");               //2
		suffixList.push_back("_classes_.csv");                      //3

		suffixList.push_back(".JPG.geometric.bin");                      //4
		suffixList.push_back(".JPG.geometric.bin");                      //5
		suffixList.push_back(".JPG.geometric.bin");                      //6

		suffixList.push_back("_param.txt");                      //7
	}

Database::~Database(){
	std::cout << " Delete Database"<< std::endl;
}

bool Database::checkFile(const std::string & file){
	boost::filesystem::path bfile(file);
	return boost::filesystem::exists(file);
}

std::string Database::getExtention(const std::string & file) {
	/*   input :: file string
	 *   output:: extension  string
	 */
	boost::filesystem::path bfile(file);
	return boost::filesystem::extension(bfile);
}

// read Frames Names List File
void Database::readNameList()
{
	std::string namesListPath = inputDir + "/imageNamesList.txt";

	std::ifstream nameFile;       /// read frame names list
	nameFile.open(namesListPath.c_str());

	if (nameFile.is_open())
	{
		std::string buff;
		while (getline(nameFile, buff))
		{
			nameList.push_back(buff);
		}
	}
	nameFile.close();

	std::sort(nameList.begin(), nameList.end()); // sort name list
}

// Read path of inputs depending on chosen suffix
std::vector< boost::filesystem::path > Database::getPathList (unsigned int suffix)
{
	std::vector< std::string >::iterator name;
	std::vector< boost::filesystem::path > tempList;
	for (name= nameList.begin(); name!=nameList.end(); name++)
	{
		if (suffix <= 3)
			ext =inputDir + "/";

		if(suffix == 4)
			ext =inputDir + "/" + "depth_maps/dslr_images_undistorted" + "/";

		if(suffix == 5)
			ext =inputDir + "/" + "normal_maps/dslr_images_undistorted" + "/";

		if(suffix == 6)
			ext =inputDir + "/" + "quality_maps/dslr_images_undistorted" + "/";

		if(suffix == 7)
			ext =inputDir + "/" "dslr_images_undistorted_";

		boost::filesystem::path currentFile(ext + *name + suffixList[suffix]);


		if (boost::filesystem::exists(currentFile))
		{
			tempList.push_back(currentFile);
		}
	}
	return tempList;
}





