#ifndef MONO_DEPTH_H_
#define MONO_DEPTH_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <boost/filesystem.hpp>

#include "utils/types.h"


class MonoDepth{

public:
 	MonoDepth()=default;
  	
	MonoDepth(const std::string& mono_depth_path);

	cv::Mat Read();
  cv::Mat Read(const std::string& path);
  cv::Mat Read(const cv::Size target_size);

	void Write(const std::string& path, cv::Mat& M);

  const std::string& GetMonoDepthPath() const;

  size_t GetWidth() const;
  size_t GetHeight() const;

  void Rescale(cv::Mat& M, const float factor);
  void Rescale(cv::Mat& M, const cv::Size target_size);
  void Downsize(const size_t max_width, const size_t max_height);

private:
  
  // mono depth identifier
	image_t _mono_depth_id;		
		
	// image path realative to workspace
	std::string _mono_depth_path;

	// Image  data
	size_t  _width, _height, _num_masks;
};

#endif //MONO_DEPTH_H_
