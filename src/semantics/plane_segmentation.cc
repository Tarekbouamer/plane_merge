#include "plane_segmentation.h"
#include <iostream>
#include <eigen3/Eigen/Geometry>

PlaneSegmentation::PlaneSegmentation(const std::string& plane_seg_path): 
			_plane_seg_path(plane_seg_path){

	cnpy::NpyArray np_data = cnpy::npy_load(plane_seg_path);

	float *data = np_data.data<float>();
				
	_num_masks	= np_data.shape[0];	
	_height 		= np_data.shape[1];	
	_width  		= np_data.shape[2];

}

std::vector<cv::Mat> PlaneSegmentation::Read(const std::string& plane_seg_path){

	std::cout << "Going To Read File " << plane_seg_path  << std::endl;
	
	cnpy::NpyArray np_data = cnpy::npy_load(plane_seg_path);

	uchar *data = np_data.data<uchar>();
	
	unsigned int num_masks 	= np_data.shape[0];	
	unsigned int height  		= np_data.shape[1];	
	unsigned int width  		= np_data.shape[2];

	// assign
	_width = width;
	_height= height;
	_num_masks = num_masks;

	// Reshape from 1D to 2D num_masks * (H W)

	std::vector<std::vector<float>> data1D;

	for (unsigned int n= 0; n <num_masks; n++ )
	{
		std::vector< float > row_data;
		
		for (unsigned int k = n * height * width; k < ( n +1 ) * height * width; k++ ){
			
			row_data.push_back(data[k]) ;
		}

		data1D.push_back(row_data);
	}
	
	// list of masks 
	std::vector<cv::Mat> planes;

	//Reshape from 2D to 3D Np * H* W

	unsigned int x=0; 
	unsigned int y=0;

	for ( unsigned int k=0; k < data1D.size(); k++ )
	{
		cv::Mat M(height, width, CV_8UC1);

		for ( unsigned int hw=0; hw <data1D[k].size(); hw++)
		{
			// 2D coordinates
			y = floor( hw / width);
			x = hw - y * width;
			
			M.at<uchar>(y,x) = (uchar) data1D[k][hw] * 255;
		}
		planes.push_back(M);	
	}

	return planes;
}

std::vector<cv::Mat> PlaneSegmentation::Read(){
  return Read(_plane_seg_path);
}

std::vector<cv::Mat> PlaneSegmentation::Read(const cv::Size target_size){

	std::vector<cv::Mat> items = Read(_plane_seg_path);
	
	for (auto& M : items){ 	Rescale(M , target_size);	}

  return items;
}
void PlaneSegmentation::Write(const std::string& _path, cv::Mat& M){

	std::logic_error("Function not yet implemented");

}

size_t PlaneSegmentation::GetWidth() const { return _width; }
size_t PlaneSegmentation::GetHeight() const { return _height; }

const std::string& PlaneSegmentation::GetPlaneSegmentationPath() const  { return _plane_seg_path;  }

void PlaneSegmentation::Rescale(cv::Mat& M, const float factor) { Rescale(M, cv::Size(factor, factor)); }

void PlaneSegmentation::Rescale(cv::Mat& M, const cv::Size target_size) {
	
	// resize map 
	cv::resize(M, M, target_size);

	// new dims

  _width = M.cols;
  _height = M.rows;
}