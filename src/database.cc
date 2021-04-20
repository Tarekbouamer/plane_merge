#include <algorithm>

#include "database.h"
#include "misc.h"

Database::Database(const std::string& path){
	workspace = GetParentPath(path); 
	Open(path); 
	
}

void Database::Open(const std::string& path){ 
	
	database = ReadTextFileLines(path);
	std::sort(database.begin(), database.end()); 

}

size_t Database::NumImages() const { return database.size(); }


// TODO: return camera struct

CameraParam Database::ReadCamera(const std::string& path)  {
	
	std::cout << "Going To Read Camera Parameters: " << path << std::endl;
	
	CameraParam _camera_param;
	
	std::ifstream inStream(path.c_str());
	std::string buffer;
	std::string tester;
	
	if(!inStream.good()){
		std::cout << " Error ReadCamera: " << path << " : Not Found" << std::endl;
	}
	
	std::string tempID = path.substr(path.find_last_of("/\\") + 1);
	std::stringstream stm(tempID.substr(0,tempID.size()-10));
	
	bool setCamA = false;
	bool setCamR = false;
	bool setCamT = false;

	bool setCamW = false;
	bool setCamH = false;

	while(inStream.good()){
		getline(inStream, buffer, '\n');
		std::istringstream raut(buffer.substr(0,1));
		
		tester = "#";
		if(raut.str().compare(tester)==0){
			continue;
		}
		else
		{
			int mode = 0;
			int tempNr=buffer.find("=",1);
			std::stringstream stm(buffer.substr(0,tempNr));
			
			std::string tester1="camera.T";
			std::string tester2="camera.R";
			std::string tester3="camera.A";
			
			std::string tester4="camera.width";
			std::string tester5="camera.height";


			if(stm.str().compare(tester1)==0) mode=1;
			else if(stm.str().compare(tester2)==0) 	mode=2;
			else if(stm.str().compare(tester3)==0) 	mode=3;
			else if(stm.str().compare(tester4)==0) 	mode=4;
			else if(stm.str().compare(tester5)==0) 	mode=5;

			// Translation 
			if(mode==1)
			{
				tempNr=buffer.find("[",1)+1;
				
				for(int i=0; i<3; i++){
					std::string searchChar = " ";
					if(i==2) searchChar = "]";
					int tempNr2=buffer.find(searchChar,tempNr);
					std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr));
					
					stm1 >> _camera_param.T[i];
					
					tempNr=tempNr2+1;
				}
				setCamT=true;
			}
			
			// Rotation 
			if(mode==2){
				
				int i=0;
				int j=0;
				
				unsigned int endNr=buffer.find("]",tempNr)+1;
				tempNr=buffer.find("[",tempNr)+1;
				
				while(buffer.find(" ",tempNr)<endNr&&buffer.find(" ",tempNr)!=std::string::npos){
					
					while(buffer.find(" ",tempNr)<buffer.find(";",tempNr)){
						int tempNr2=buffer.find(" ",tempNr);
						std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr));
						stm1 >> _camera_param.R(i,j++);
						tempNr=tempNr2+1;
					}
					
					unsigned int tempNr2=buffer.find(" ",tempNr);
					
					if(tempNr2==std::string::npos||tempNr2>endNr){
						break;
					}
					
					std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr-1));
					
					stm1 >> _camera_param.R(i++,j);
					tempNr=tempNr2+1;
					j=0;
				}
				std::stringstream stm1(buffer.substr(tempNr,endNr-tempNr-1));
				
				stm1 >> _camera_param.R(i,j);
				setCamR=true;
			}
			
			/// intrinsics
			if(mode==3){
				int i=0;
				int j=0;
				
				unsigned int endNr=buffer.find("]",tempNr)+1;
				tempNr=buffer.find("[",tempNr)+1;
				
				while(buffer.find(" ",tempNr)<endNr&&buffer.find(" ",tempNr)!=std::string::npos){
				
					while(buffer.find(" ",tempNr)<buffer.find(";",tempNr)){
						int tempNr2=buffer.find(" ",tempNr);
						std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr));
						
						stm1 >> _camera_param.A(i,j++);
						tempNr=tempNr2+1;
					}
					
					unsigned int tempNr2=buffer.find(" ",tempNr);
					
					if(tempNr2==std::string::npos||tempNr2>endNr){
						break;
					}
					
					std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr-1));
					stm1 >> _camera_param.A(i++,j);
					tempNr=tempNr2+1;
					j=0;
				}
				std::stringstream stm1(buffer.substr(tempNr,endNr-tempNr-1));
				
				stm1 >> _camera_param.A(i,j);
				setCamA=true;
			}
		
			// width 
			if(mode==4)
			{
				std::string searchChar = " ";
				int tempNr2=buffer.find(searchChar, tempNr + 1);
				std::stringstream stm1(buffer.substr(tempNr +1 ,tempNr2-tempNr + 1));
				stm1 >> _camera_param.W;
				setCamW=true;
			}

			// height 
			if(mode==5)
			{
				std::string searchChar = " ";
				int tempNr2=buffer.find(searchChar, tempNr +1);
				std::stringstream stm1(buffer.substr(tempNr + 1,tempNr2-tempNr + 1));
				stm1 >> _camera_param.H;
				setCamH=true;
			}
		}
	}
	
	if(!setCamT){ 
		std::cout << " Error ReadCamera 'camera.T' " << path << "'." << std::endl;
	}
	if(!setCamR){
		std::cout << " Error ReadCamera 'camera.R' " << path << "'." << std::endl;
	}
	if(!setCamA){
		std::cout << " Error ReadCamera 'camera.A' " << path << "'." << std::endl;
	}
	if(!setCamW){
		std::cout << " Error ReadCamera 'camera.width' " << path << "'." << std::endl;
	}	
	if(!setCamH){
		std::cout << " Error ReadCamera 'camera.hight' " << path << "'." << std::endl;
	}

	_camera_param.Inv_A = _camera_param.A.inverse();
	return _camera_param;
}


Image Database::ReadImage(const std::string& image_path, const std::string& camera_path)  {
		
	// Read Camera parameters
	CameraParam params = ReadCamera(camera_path);

	Image _image =  Image(image_path, camera_path, params.W, params.H, params.A, params.R, params.T);
	
	return _image;
}


std::vector<Image> Database::ReadAllImages()  {
  
	std::vector<Image> images;
 
  images.reserve(NumImages());

	for (auto& img : database){

		std::string image_path = JoinPaths(workspace, images_folder, img);
		std::string camera_path = JoinPaths(workspace, models_folder, GetPathBaseName(img) + ".txt");

		images.push_back(ReadImage(image_path, camera_path));
	}

  return images;
}


void Database::Load() {

  //////////////////////////////////////////////////////////////////////////////
  // Load images
  //////////////////////////////////////////////////////////////////////////////

  std::cout << "Loading images..." << std::endl;

  std::unordered_set<image_t> image_ids;

  const std::vector<class Image> images = ReadAllImages();

}



















  //   // Determines for which images data should be loaded.
  //   if (image_names.empty()) {
  //     for (const auto& image : images) {
  //       image_ids.insert(image.ImageId());
  //     }
  //   } else {
  //     for (const auto& image : images) {
  //       if (image_names.count(image.Name()) > 0) {
  //         image_ids.insert(image.ImageId());
  //       }
  //     }
  //   }

  //   // Collect all images that are connected in the correspondence graph.
  //   std::unordered_set<image_t> connected_image_ids;
  //   connected_image_ids.reserve(image_ids.size());
  //   for (size_t i = 0; i < image_pair_ids.size(); ++i) {
  //     if (UseInlierMatchesCheck(two_view_geometries[i])) {
  //       image_t image_id1;
  //       image_t image_id2;
  //       Database::PairIdToImagePair(image_pair_ids[i], &image_id1, &image_id2);
  //       if (image_ids.count(image_id1) > 0 && image_ids.count(image_id2) > 0) {
  //         connected_image_ids.insert(image_id1);
  //         connected_image_ids.insert(image_id2);
  //       }
  //     }
  //   }

  //   // Load images with correspondences and discard images without
  //   // correspondences, as those images are useless for SfM.
  //   images_.reserve(connected_image_ids.size());
  //   for (const auto& image : images) {
  //     if (image_ids.count(image.ImageId()) > 0 &&
  //         connected_image_ids.count(image.ImageId()) > 0) {
  //       images_.emplace(image.ImageId(), image);
  //       const FeatureKeypoints keypoints =
  //           database.ReadKeypoints(image.ImageId());
  //       const std::vector<Eigen::Vector2d> points =
  //           FeatureKeypointsToPointsVector(keypoints);
  //       images_[image.ImageId()].SetPoints2D(points);
  //     }
  //   }

  //   std::cout << StringPrintf(" %d in %.3fs (connected %d)", images.size(),
  //                             timer.ElapsedSeconds(),
  //                             connected_image_ids.size())
  //             << std::endl;
  // }

  // //////////////////////////////////////////////////////////////////////////////
  // // Build correspondence graph
  // //////////////////////////////////////////////////////////////////////////////

  // timer.Restart();
  // std::cout << "Building correspondence graph..." << std::flush;

  // for (const auto& image : images_) {
  //   correspondence_graph_.AddImage(image.first, image.second.NumPoints2D());
  // }

  // size_t num_ignored_image_pairs = 0;
  // for (size_t i = 0; i < image_pair_ids.size(); ++i) {
  //   if (UseInlierMatchesCheck(two_view_geometries[i])) {
  //     image_t image_id1;
  //     image_t image_id2;
  //     Database::PairIdToImagePair(image_pair_ids[i], &image_id1, &image_id2);
  //     if (image_ids.count(image_id1) > 0 && image_ids.count(image_id2) > 0) {
  //       correspondence_graph_.AddCorrespondences(
  //           image_id1, image_id2, two_view_geometries[i].inlier_matches);
  //     } else {
  //       num_ignored_image_pairs += 1;
  //     }
  //   } else {
  //     num_ignored_image_pairs += 1;
  //   }
  // }

  // correspondence_graph_.Finalize();

  // // Set number of observations and correspondences per image.
  // for (auto& image : images_) {
  //   image.second.SetNumObservations(
  //       correspondence_graph_.NumObservationsForImage(image.first));
  //   image.second.SetNumCorrespondences(
  //       correspondence_graph_.NumCorrespondencesForImage(image.first));
  // }

  // std::cout << StringPrintf(" in %.3fs (ignored %d)", timer.ElapsedSeconds(),
  //                           num_ignored_image_pairs)
  //           << std::endl;


