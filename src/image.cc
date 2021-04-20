#include "image.h"
#include <iostream>

Image::Image(const std::string& image_path,  const std::string& camera_path,
						 const size_t width, const size_t height,
             
             const Eigen::Matrix3d K, const Eigen::Matrix3d R, const Eigen::Vector3d T): 
              _image_path(image_path),
              _camera_path(camera_path),
              _width(width),
              _height(height),
              _K(K),
              _R(R),
              _T(T) {};


cv::Mat Image::Read(){

  unsigned int flag = 1;
	
	cv::Mat img = cv::imread(_image_path, flag); // TODO:set the flags
  
	_channels = img.channels();

	return img;
}

size_t Image::GetWidth() const { return _width; }
size_t Image::GetHeight() const { return _height; }
size_t Image::GetChannels() const { return _channels; }

const std::string& Image::GetImagePath() const  { return _image_path;  }
const std::string& Image::GetCameraPath() const { return _camera_path; }

const Eigen::Matrix3d Image::GetR() const { return _R; }
const Eigen::Vector3d Image::GetT() const { return _T; }
const Eigen::Matrix3d Image::GetK() const { return _K; }

// const float* Image::GetP() const { return P_; }
// const float* Image::GetInvP() const { return inv_P_; }
// const float* Image::GetViewingDirection() const { return &R_[6]; }

void Image::Rescale(const float factor) { Rescale(factor, factor); }

void Image::Rescale(const float factor_x, const float factor_y) {

  const size_t new_width = std::round(_width * factor_x);
  const size_t new_height = std::round(_height * factor_y);

	// TODO rescale image as well

  const float scale_x = new_width / static_cast<float>(_width);
  const float scale_y = new_height / static_cast<float>(_height);
  
	_K(0) *= scale_x;
  _K(2) *= scale_x;
  _K(4) *= scale_y;
  _K(5) *= scale_y;
  
	// ComposeProjectionMatrix(K_, R_, T_, P_);
  // ComposeInverseProjectionMatrix(K_, R_, T_, inv_P_);

  _width = new_width;
  _height = new_height;
}
