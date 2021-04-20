#ifndef PLANE_SEGEMENTATION_H_
#define PLANE_SEGEMENTATION_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "types.h"
#include "lib/cnpy/cnpy.h"

class PlaneSegmentation{

public:
 	PlaneSegmentation()=default;
  	
	PlaneSegmentation(const std::string& plane_seg_path, const std::string& plane_params_path);

	std::vector<cv::Mat> Read();
  std::vector<cv::Mat> Read(const std::string& path);
  std::vector<cv::Mat> Read(const cv::Size target_size);

	void Write(const std::string& path, cv::Mat& M);

  const std::string& GetPlaneSegmentationPath() const;

  size_t GetWidth() const;
  size_t GetHeight() const;

  void Rescale(cv::Mat& M, const float factor);
  void Rescale(cv::Mat& M, const cv::Size target_size);
  void Downsize(const size_t max_width, const size_t max_height);

private:
  
  // plane_segmentation identifier
	image_t _plane_segmentation_id;		
		
	// image path realative to workspace
	std::string _plane_seg_path;
	std::string _plane_params_path;

	// Image  data
	size_t  _width, _height, _num_masks;
};

#endif //PLANE_SEGEMENTATION_H_
