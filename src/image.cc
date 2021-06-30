#include "image.h"
#include <iostream>

Image::Image(const std::string& image_path,  const std::string& camera_path,
						 const size_t width, const size_t height,
             
             const Eigen::Matrix3f K, const Eigen::Matrix3f R, const Eigen::Vector3f T): 
              _image_path(image_path),
              _camera_path(camera_path),
              _width(width),
              _height(height),
              _K(K),
              _R(R),
              _T(T) {
  
  // compose projection matrix and its inverse            
  
  _P = ComposeProjectionMatrix(_R, _T);
  _inv_P = InvertProjectionMatrix(_P);
  
  };


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

const Eigen::Matrix3f Image::GetR() const { return _R; }
const Eigen::Vector3f Image::GetT() const { return _T; }

const Eigen::Matrix3f Image::GetK() const { return _K; }

const Eigen::Matrix3x4f Image::GetP() const { return _P; }
const Eigen::Matrix3x4f Image::GetInvP() const { return _inv_P; }

Eigen::Matrix3x4f Image::ComposeProjectionMatrix(const Eigen::Matrix3f& R, const Eigen::Vector3f& T) {
  Eigen::Matrix3x4f proj_matrix;
  
  proj_matrix.leftCols<3>() = R;
  proj_matrix.rightCols<1>() = T;
  
  return proj_matrix;
}

Eigen::Matrix3x4f Image::InvertProjectionMatrix(const Eigen::Matrix3x4f& proj_matrix) {
  Eigen::Matrix3x4f inv_proj_matrix;
  
  inv_proj_matrix.leftCols<3>() = proj_matrix.leftCols<3>().transpose();
  inv_proj_matrix.rightCols<1>() = ProjectionCenterFromMatrix(proj_matrix);
  
  return inv_proj_matrix;
}

Eigen::Vector3f Image::ProjectionCenterFromMatrix(const Eigen::Matrix3x4f& proj_matrix) {
  
  return -proj_matrix.leftCols<3>().transpose() * proj_matrix.rightCols<1>();
}

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
