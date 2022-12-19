#ifndef MSEG_SEGEMENTATION_H_
#define MSEG_SEGEMENTATION_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <boost/filesystem.hpp>

#include "utils/types.h"


class MsegSegmentation{

public:
 	MsegSegmentation()=default;
  	
	MsegSegmentation(const std::string& mseg_seg_path);

	cv::Mat Read();
  cv::Mat Read(const std::string& path);
  cv::Mat Read(const cv::Size target_size);

	void Write(const std::string& path, cv::Mat& M);

  const std::string& GetMsegSegmentationPath() const;

  size_t GetWidth() const;
  size_t GetHeight() const;

  void Rescale(cv::Mat& M, const float factor);
  void Rescale(cv::Mat& M, const cv::Size target_size);
  void Downsize(const size_t max_width, const size_t max_height);

private:
  
  // plane_segmentation identifier
	image_t _mseg_segmentation_id;		
		
	// image path realative to workspace
	std::string _mseg_seg_path;

	// Image  data
	size_t  _width, _height, _num_masks;
};

#endif //MSEG_SEGEMENTATION_H_
