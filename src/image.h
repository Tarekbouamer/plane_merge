#ifndef IMAGE_H_
#define IMAGE_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <eigen3/Eigen/Geometry>

#include "types.h"

struct CameraParam
{
	size_t W;
	size_t H;

	Eigen::Vector3d T;
	Eigen::Matrix3d R;
	Eigen::Matrix3d A;
	Eigen::Matrix3d Inv_A;
	
	int id;
};

class Image{
	public:
 		Image()=default;
  	
		Image(const std::string& image_path, const std::string& camera_path, 
					const size_t width, const size_t height,
        	const Eigen::Matrix3d K, const Eigen::Matrix3d R, const Eigen::Vector3d T);
		

		cv::Mat Read();

  	const std::string& GetImagePath() const;
  	const std::string& GetCameraPath() const;
		
		size_t GetWidth() const;
		size_t GetHeight() const;
		size_t GetChannels() const;

		const Eigen::Matrix3d GetR() const;
		const Eigen::Vector3d GetT() const;
		const Eigen::Matrix3d GetK() const;
		
		const float* GetP() const;
		const float* GetInvP() const;
		
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

		Eigen::Matrix3d _K;
		Eigen::Matrix3d _R;
		Eigen::Vector3d _T;

		float P_[12];
		float inv_P_[12];

};


// void ComputeRelativePose(const float R1[9], const float T1[3],
//                          const float R2[9], const float T2[3], float R[9],
//                          float T[3]);

// void ComposeProjectionMatrix(const float K[9], const float R[9],
//                              const float T[3], float P[12]);

// void ComposeInverseProjectionMatrix(const float K[9], const float R[9],
//                                     const float T[3], float inv_P[12]);

// void ComputeProjectionCenter(const float R[9], const float T[3], float C[3]);

// void RotatePose(const float RR[9], float R[9], float T[3]);

#endif //IMAGE_H_
