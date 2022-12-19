

#ifndef DATABASE_H_
#define DATABASE_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include "base/image.h"

#include "utils/types.h"

class Database {
	public:
	
	Database()=default;
  explicit Database(const std::string& path);

	// read database txt file
	void Open(const std::string& path);
	
	// number of registred images in database
	size_t NumImages() const;
	
	// load camera parameters from txt file
	CameraParam ReadCamera(const std::string& path);
	
	// Read one object images
	Image ReadImage(const std::string& image_path, const std::string& camera_path) ;
	
	// Read All images in database (camera parameters and loading path)
	std::vector<Image> ReadAllImages() ;

  // Check whether specific object exists.
  bool ExistsImage(const image_t image_id) ;
	
	// path to database txt file
	
	std::vector<std::string> database;
	std::string workspace;
	
	std::string images_folder 			= "/images/";
	std::string models_folder 			= "/stereo/model/";
	std::string superpixels_folder 	= "/stereo/superpixels/";
	
	std::string depth_folder 				= "/stereo/depth_maps/dslr_images_undistorted/";
	std::string normal_folder 			= "/stereo/normal_maps/dslr_images_undistorted/";
	std::string quality_folder 			= "/stereo/quality_maps/dslr_images_undistorted/";
	std::string confidence_folder 	= "/stereo/confidence_maps/dslr_images_undistorted/";
	std::string plane_seg_folder 		= "/stereo/plane_segmentation/";
	std::string mseg_seg_folder 		= "/stereo/semantic_segmentation/";
	std::string mono_depth_folder 	= "/stereo/mono_depth/";

	std::string test 		= "/stereo/test/";

	// camera model
	CameraParam camera_param;

	// load database
	void Load();
 
 private:
  //class CorrespondenceGraph correspondence_graph_;

  EIGEN_STL_UMAP(image_t, class Image) images_;
};

#endif // DATABASE_H_

