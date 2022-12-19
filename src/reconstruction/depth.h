#ifndef DEPTH_H_
#define DEPTH_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "utils/types.h"

class Depth{

public:
 	Depth()=default;
  	
	Depth(const std::string& depth_path);

	cv::Mat Read();
  cv::Mat Read(const std::string& path);

	void Write(const std::string& path, cv::Mat& M);

  const std::string& GetDepthPath() const;

  size_t GetWidth() const;
  size_t GetHeight() const;

  void Rescale(const float factor);
  void Rescale(const float factor_x, const float factor_y);
  void Downsize(const size_t max_width, const size_t max_height);

private:
  
  // depth identifier
	image_t _depth_id;		
		
	// image path realative to workspace
	std::string _depth_path;

	// Image  data
	size_t  _width, _height, _channels;
};

#endif //DEPTH_H_
