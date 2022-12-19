#ifndef IMAGE_H_
#define IMAGE_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <eigen3/Eigen/Geometry>

#include "utils/types.h"

struct CameraParam
{
	size_t W;
	size_t H;

	Eigen::Vector3f T;
	Eigen::Matrix3f R;
	Eigen::Matrix3f A;
	Eigen::Matrix3f Inv_A;
	
	int id;
};

class Image{
	public:
 		Image()=default;
  	
		Image(const std::string& image_path, const std::string& camera_path, 
					const size_t width, const size_t height,
        	const Eigen::Matrix3f K, const Eigen::Matrix3f R, const Eigen::Vector3f T);
		

		cv::Mat Read();

  	const std::string& GetImagePath() const;
  	const std::string& GetCameraPath() const;
		
		size_t GetWidth() const;
		size_t GetHeight() const;
		size_t GetChannels() const;

		const Eigen::Matrix3f GetR() const;
		const Eigen::Vector3f GetT() const;
		const Eigen::Matrix3f GetK() const;
		
		const Eigen::Matrix3x4f GetP() const;
		const Eigen::Matrix3x4f GetInvP() const;
		
		Eigen::Matrix3x4f ComposeProjectionMatrix(const Eigen::Matrix3f& R, const Eigen::Vector3f& T);
		Eigen::Matrix3x4f InvertProjectionMatrix(const Eigen::Matrix3x4f& proj_matrix);;
		Eigen::Vector3f ProjectionCenterFromMatrix(const Eigen::Matrix3x4f& proj_matrix);

		const float* GetViewingDirection() const;

		void Rescale(const float factor);
		void Rescale(const float factor_x, const float factor_y);


	private:
		// image identifier
		image_t _image_id;		
		
		// image path realative to workspace
		std::string _image_path;

		// camera (model) path realative to workspace
		std::string _camera_path;

		// The number of 3D points observed
		point3D_t _num_points3D;

		// Image  data
		size_t  _width, _height, _channels;

		Eigen::Matrix3f _K;
		Eigen::Matrix3f _R;
		Eigen::Vector3f _T;

		Eigen::Matrix3x4f _P;
		Eigen::Matrix3x4f _inv_P;

};
#endif //IMAGE_H_
