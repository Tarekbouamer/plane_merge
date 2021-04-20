#include "quality.h"
#include <iostream>
#include <eigen3/Eigen/Geometry>

Quality::Quality(const std::string& quality_path): _quality_path(quality_path) {};

cv::Mat Quality::Read(const std::string& quality_path){

  	std::cout << "Going To Read File " << quality_path  << std::endl;
	
  std::vector<float> data_;
	std::fstream txt_file;

	txt_file.open(&quality_path[0], std::fstream::in | std::fstream::binary);
	
  if(!txt_file.good())
	{
		std::cout << " File Damaged "<< quality_path << std::endl;
		return cv::Mat();
	}

	int width, height, depth;
	char unused_char;

	txt_file >> width >> unused_char >> height >> unused_char >> depth >> unused_char;
	
	std::streampos pos = txt_file.tellg();
	txt_file.close();
	
  data_.resize(width * height * depth);

	std::fstream binary_file(&quality_path[0], std::fstream::in | std::fstream::binary);
	
  binary_file.seekg(pos);
	binary_file.read(reinterpret_cast<char*>(data_.data()), data_.size() * sizeof(float));
	binary_file.close();

	int index=0;

	cv::Mat M(height, width, CV_32FC1);
	
	for(int y=0; y<height; y++){
    for(int x=0; x<width; x++){
			
      float val = data_[index++];
			
      M.at<float>(y, x)=val;
		
    }
	}

	return M;

}

cv::Mat Quality::Read(){
  return Read(_quality_path);
}

void Quality::Write(const std::string& _path, cv::Mat& M){

	std::cout << "Going To Write File: " << _path  << std::endl;

	std::fstream text_file(&_path[0], std::ios_base::out);

	text_file << M.cols << "&" << M.rows << "&" << 1 << "&";
	text_file.close();

	std::vector<float> data_;
	data_.resize(M.rows * M.cols * 1);

	int index=0;

	for(int y=0; y<M.rows; y++){
		for(int x=0; x<M.cols; x++){
			data_[index++]=M.at<float>(y,x);
		}
	}

	std::fstream binary_file(&_path[0], std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	binary_file.write(reinterpret_cast<const char*>(data_.data()), sizeof(float) * M.cols * M.rows * 1);
    binary_file.close();
}

size_t Quality::GetWidth() const { return _width; }
size_t Quality::GetHeight() const { return _height; }

const std::string& Quality::GetQualityPath() const  { return _quality_path;  }

void Quality::Rescale(const float factor) { Rescale(factor, factor); }

void Quality::Rescale(const float factor_x, const float factor_y) {

  const size_t new_width = std::round(_width * factor_x);
  const size_t new_height = std::round(_height * factor_y);

	// TODO: rescale depth as well

  const float scale_x = new_width / static_cast<float>(_width);
  const float scale_y = new_height / static_cast<float>(_height);
  
  _width = new_width;
  _height = new_height;
}