#include "mseg_segmentation.h"

#include <fstream>
#include <eigen3/Eigen/Geometry>

#include "utils/misc.h"

MsegSegmentation::MsegSegmentation(const std::string& mseg_seg_path): 
			_mseg_seg_path(mseg_seg_path) {
	
	boost::filesystem::path current_path(boost::filesystem::current_path());
	
	// TODO: modify this line
	// std::string mseg_classes = JoinPaths("/home/loc/3D/plane_merge/config/", "classes.txt");
	std::string mseg_classes = JoinPaths("/home/dl/3D/plane_merge/config/", "classes.txt");

	// readlines
	std::vector<std::string> lines = ReadTextFileLines(mseg_classes.c_str());

	// classes map
	int key = 1;
	std::unordered_map< int, std::string> mseg_classes_map; 

	for(auto& item: lines){
		mseg_classes_map.insert({ key, item });
		key++;
	}

	std::cout << mseg_classes_map.at(11)<< std::endl;

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