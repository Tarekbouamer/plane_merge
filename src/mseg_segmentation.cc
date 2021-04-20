#include "mseg_segmentation.h"

#include <fstream>
#include "misc.h"
#include <eigen3/Eigen/Geometry>

MsegSegmentation::MsegSegmentation(const std::string& mseg_seg_path): 
			_mseg_seg_path(mseg_seg_path) {
	
	boost::filesystem::path current_path(boost::filesystem::current_path());
	
	// TODO: modify this line
	std::string mseg_classes = JoinPaths("/home/torres/3D/planeMerge/config", "classes.txt");

	// readlines
	std::vector<std::string> lines = ReadTextFileLines(mseg_classes.c_str());

	// classes map
	int key = 0;
	std::unordered_map< int, std::string> mseg_classes_map; 

	for(auto& item: lines){
		mseg_classes_map.insert({ key, item });
		key++;
	}

	std::cout << mseg_classes_map.at(11)<< std::endl;

	// boost::filesystem::path full_path(boost::filesystem::current_path());
	// std::cout << "constructor" << std::endl;

	// if (file.is_open()) {

  // 	std::string line;
    
	// 	std::cout << "lines" << std::endl;

	// 	while (std::getline(file, line)) {

	// 		std::cout << line << std::endl;
  //   }
		
	// 	//
  //   file.close();
	// }
}
		


cv::Mat MsegSegmentation::Read(const std::string& mseg_seg_path){

	std::cout << "Going To Read File " << mseg_seg_path  << std::endl;

	cv::Mat msk = cv::imread(mseg_seg_path, cv::IMREAD_GRAYSCALE) ;
	
	// assign
	_width = msk.cols;
	_height= msk.rows;

	return msk;
}


cv::Mat MsegSegmentation::Read(){
  return Read(_mseg_seg_path);
}


cv::Mat MsegSegmentation::Read(const cv::Size target_size){

	cv::Mat M = Read(_mseg_seg_path);
	
	Rescale(M , target_size); 

  return M;
}


void MsegSegmentation::Write(const std::string& _path, cv::Mat& M){

	std::logic_error("Function not yet implemented");

}


size_t MsegSegmentation::GetWidth() const { 	return _width; 	}
size_t MsegSegmentation::GetHeight() const { 	return _height; }


const std::string& MsegSegmentation::GetMsegSegmentationPath() const  { return _mseg_seg_path;  }


void MsegSegmentation::Rescale(cv::Mat& M, const float factor) { Rescale(M, cv::Size(factor, factor)); }


void MsegSegmentation::Rescale(cv::Mat& M, const cv::Size target_size) {
	
	// resize map 
	cv::resize(M, M, target_size);

	// new dims

  _width = M.cols;
  _height = M.rows;
}