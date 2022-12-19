#ifndef CONFIDENCE_H_
#define CONFIDENCE_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "utils/types.h"

class Confidence{

public:
 	Confidence()=default;
  	
	Confidence(const std::string& confidence_path);

	cv::Mat Read();
  cv::Mat Read(const std::string& path);

	void Write(const std::string& path, cv::Mat& M);

  const std::string& GetConfidencePath() const;

  size_t GetWidth() const;
  size_t GetHeight() const;

  void Rescale(const float factor);
  void Rescale(const float factor_x, const float factor_y);
  void Downsize(const size_t max_width, const size_t max_height);

private:
  
  // confidence identifier
	image_t _confidence_id;		
		
	// image path realative to workspace
	std::string _confidence_path;

	// Image  data
	size_t  _width, _height, _channels;
};

#endif //CONFIDENCE_H_
