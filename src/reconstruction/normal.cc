#include "normal.h"
#include <iostream>
#include <eigen3/Eigen/Geometry>

Normal::Normal(const std::string& nromal_path): _normal_path(nromal_path) {};

cv::Mat Normal::Read(const std::string& nromal_path){

	std::cout << "Going To Read File " << nromal_path << "..." << std::endl;
	std::vector<float> data_;

	std::fstream text_file;
	text_file.open(&nromal_path[0], std::fstream::in | std::fstream::binary);
	
	if(!text_file.good())
	{
		return cv::Mat();
	}

	int width, height, depth;
	char unused_char;

	text_file >> width >> unused_char >> height >> unused_char >> depth >> unused_char;
	
	std::streampos pos = text_file.tellg();
	text_file.close();
	data_.resize(width * height * depth);

	std::fstream binary_file(&nromal_path[0], std::fstream::in | std::fstream::binary);
	binary_file.seekg(pos);
	binary_file.read(reinterpret_cast<char*>(data_.data()), data_.size() * sizeof(float));
	binary_file.close();

	int index=0;

	cv::Mat M(height, width, CV_32FC3);
	
	for(int d=0; d<depth; d++){
		for(int y=0; y<height; y++){
			for(int x=0; x<width; x++){
				float val = data_[index++];
				M.at<cv::Vec3f>(y, x).val[d]=val;
			}
		}
	}
	return M;

}

cv::Mat Normal::Read(){
  return Read(_normal_path);
}

void Normal::Write(const std::string& _path, cv::Mat& M){
	
	std::cout << "Going To Write File: " << _path << "..." << std::endl;
	
	std::fstream text_file(&_path[0], std::ios_base::out);
	
	text_file << M.cols << "&" << M.rows << "&" << 3 << "&";
	text_file.close();

	std::vector<float> data_;
	data_.resize(M.rows * M.cols * 3);

	int index=0;
	for(int d=0; d<3; d++){
		for(int y=0; y<M.rows; y++){
			for(int x=0; x<M.cols; x++){
				data_[index++]=M.at<cv::Vec3f>(y, x).val[d];
			}
		}
	}

	std::fstream binary_file(&_path[0], std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	
	binary_file.write(reinterpret_cast<const char*>(data_.data()), sizeof(float) * M.cols * M.rows * 3);
  binary_file.close();
}

size_t Normal::GetWidth() const { return _width; }
size_t Normal::GetHeight() const { return _height; }

const std::string& Normal::GetNormalPath() const  { return _normal_path;  }

void Normal::Rescale(const float factor) { Rescale(factor, factor); }

void Normal::Rescale(const float factor_x, const float factor_y) {

  const size_t new_width = std::round(_width * factor_x);
  const size_t new_height = std::round(_height * factor_y);

	// TODO: rescale normal as well

  const float scale_x = new_width / static_cast<float>(_width);
  const float scale_y = new_height / static_cast<float>(_height);
  
  _width = new_width;
  _height = new_height;
}