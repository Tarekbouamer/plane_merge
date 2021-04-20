#ifndef QUALITY_H_
#define QUALITY_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "types.h"

class Quality{

public:
 	Quality()=default;
  	
	Quality(const std::string& qulaity_path);

	cv::Mat Read();
  cv::Mat Read(const std::string& path);

	void Write(const std::string& path, cv::Mat& M);

  const std::string& GetQualityPath() const;

  size_t GetWidth() const;
  size_t GetHeight() const;

  void Rescale(const float factor);
  void Rescale(const float factor_x, const float factor_y);
  void Downsize(const size_t max_width, const size_t max_height);

private:
  
  // quality identifier
	image_t _quality_id;		
		
	// image path realative to workspace
	std::string _quality_path;

	// Image  data
	size_t  _width, _height, _channels;
};

#endif //QUALITY_H_
