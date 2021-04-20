#include <algorithm>
#include <random>
#include <thread>

#include "simple_merge.h"
#include "depth.h"
#include "normal.h"
#include "quality.h"
#include "confidence.h"

#include "graph.h"
#include "plane_segmentation.h"
#include "mseg_segmentation.h"

#include "cost.h"




SimpleMerge::SimpleMerge(const std::string& database_path ){
  _database = Database(database_path);
}


void SimpleMerge::ComputeUncertaintyMap(cv::Mat& DepthMap, cv::Mat& QualityMap, cv::Mat& UncertaintyMap){
  
  // sqrt(2) x D(i, j)^2 x Q(i, j)
  
  for (int x = 0; x < QualityMap.cols; x++){
    
    for (int y = 0; y < QualityMap.rows; y++){
      
      float pixelD = DepthMap.at<float>(y, x);

      if (pixelD != INFINITY && pixelD != 0){

        UncertaintyMap.at<float>(y, x) = sqrt(2) * pixelD * pixelD / QualityMap.at<float>(y, x);
			}
		}
  }
}


void SimpleMerge::LoadSuperpixelList(Image& item, superpixels_t& list_supepixels){

  // image name
  std::string item_name = GetPathBaseName(item.GetImagePath());

  // read rgb image
  cv::Mat rgb_image = item.Read();

  // read depth
  std::string  item_depth = JoinPaths(_database.workspace, _database.depth_folder, item_name + ".JPG.geometric.bin");
  cv::Mat depth_map = Depth(item_depth).Read();

  std::cout <<  depth_map.size() << "   " << depth_map.channels() << std::endl;

  // read quality
  std::string  item_quality = JoinPaths(_database.workspace, _database.quality_folder, item_name + ".JPG.geometric.bin");
  cv::Mat quality_map = Quality(item_quality).Read();

  std::cout <<  quality_map.size() << "   " << quality_map.channels() <<  std::endl;

  // compute  uncertanity map
	cv::Mat uncertainty_map = cv::Mat(quality_map.rows, quality_map.cols, CV_32FC1, INFINITY);  
  ComputeUncertaintyMap(depth_map, quality_map, uncertainty_map);    
  
  // read superpixels
  std::string  item_superpixels_cls = JoinPaths(_database.workspace, _database.superpixels_folder, item_name + ".csv");
  std::string  item_superpixels_lbl = JoinPaths(_database.workspace, _database.superpixels_folder, item_name + ".png");
    
  SuperpixelsSegementation _superpixels = SuperpixelsSegementation(item_superpixels_cls, item_superpixels_lbl);

  cv::Mat lbl = _superpixels.ReadLabels();
  cv::Mat cls = _superpixels.ReadCSV();

  for (int row = 0; row < cls.rows; row++){

    for (int col = 0; col < cls.cols; col++){

      // color for superpixel 
      list_supepixels[cls.at<ushort>(row, col)]->labelColor = lbl.at<cv::Vec3b>(row, col);
     
      // 2D points   
      list_supepixels[cls.at<ushort>(row, col)]->points2D.push_back(Eigen::Vector2i(row, col));
      
      // num 2D points  
      list_supepixels[cls.at<ushort>(row, col)]->numPixels++;
      
      //  geo model 
      if (depth_map.at<float>(row, col) != INFINITY && depth_map.at<float>(row, col) != 0.0){
          
        // homogeneous coordinates 
        const Eigen::Vector3f coord(float(row), float(col), 1.0);
          
        const float pixelD = depth_map.at<float>(row, col);
        
        // 3D points  
        list_supepixels[cls.at<ushort>(row, col)]->points3D.push_back(item.GetK().inverse().cast<float>() * coord * pixelD);
        
        // 3D quality
        list_supepixels[cls.at<ushort>(row, col)]->quality3D.push_back(uncertainty_map.at<float>(row, col));
        
        // num 3D points
        list_supepixels[cls.at<ushort>(row, col)]->numDepths++;
      }

      //  appearance model 
      cv::Vec3b color = rgb_image.at<cv::Vec3b>(row, col);

      list_supepixels[cls.at<ushort>(row, col)]->appearanceHistogram.at<float>(color.val[0], 0) += 1.0;
      list_supepixels[cls.at<ushort>(row, col)]->appearanceHistogram.at<float>(color.val[1], 1) += 1.0;
      list_supepixels[cls.at<ushort>(row, col)]->appearanceHistogram.at<float>(color.val[2], 2) += 1.0;
    }
  }

  for (int id = 0; id < list_supepixels.size(); id++){
    cv::normalize(list_supepixels[id]->appearanceHistogram, list_supepixels[id]->appearanceHistogram, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  }
}


void SimpleMerge::AssignNeighbouring(const cv::Mat& cls, superpixels_t& list_supepixels){
  //Check eight neighboring pixels for potential neighboring cluster

  for (int x = 0; x < cls.cols; x++){
		for (int y = 0; y < cls.rows; y++){
			
      //Get reference to cluster
			const int& id = cls.at<unsigned short>(y, x);
			
      Superpixel &current = *(list_supepixels[id]); 

			if (x - 1 > 0 && y - 1 > 0){
        if (cls.at<unsigned short>(y - 1, x - 1) != id){
          current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y - 1, x - 1)]);
				}
			} //

			if (y - 1 > 0){
        if (cls.at<unsigned short>(y - 1, x) != id){
					current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y - 1, x)]);
				}
			} //

			if (x + 1 < cls.cols && y - 1 > 0){
				if (cls.at<unsigned short>(y - 1, x + 1) != id){
					current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y - 1, x + 1)]);
				}
			} //

			if (x - 1 > 0){
				if (cls.at<unsigned short>(y, x - 1) != id){
					current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y, x - 1)]);
				}
			} //

			if (x + 1 < cls.cols){
				if (cls.at<unsigned short>(y, x + 1) != id){
					current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y, x + 1)]);
				}
			} //

			if (x - 1 > 0 && y + 1 < cls.rows){
				if (cls.at<unsigned short>(y + 1, x - 1) != id){
					current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y + 1, x - 1)]);
				}
			} //

			if (y + 1 < cls.rows){
				if (cls.at<unsigned short>(y + 1, x) != id){
					current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y + 1, x)]);
				}
			} //

			if (x + 1 < cls.cols && y + 1 < cls.rows){
				if (cls.at<unsigned short>(y + 1, x + 1) != id){
					current.neighbors.insert(list_supepixels[cls.at<unsigned short>(y + 1, x + 1)]);
				}
			} //

		}
	}
}


void SimpleMerge::EstimatePlaneParameters(superpixels_t& list_supepixels){
  // init ransac
  RANSACOptions ransac_options;

	ransac_options.max_error 			  = 1.0;
	ransac_options.confidence 			= 0.99;
	// ransac_options.min_num_trials 	= 200;
	// ransac_options.max_num_trials 	= 20000;
	ransac_options.min_inlier_ratio 	= 0.9;
  
	RANSAC<PlaneModel, MEstimatorSupportMeasurer, RandomSampler> ransac(ransac_options);

  // iterate over all superpixels

  for (auto& s :list_supepixels){

    // don't estimate plane for not well supported superpixels
    float num_support = (float) s->points3D.size() /  (float) s->points2D.size();

    if (num_support > 0.7){
      
      auto report = ransac.Estimate(s->points3D, s->quality3D);

      if (report.success){
        
        float confidence = (float)  report.support.num_inliers / (float) s->points3D.size();

        if (confidence>0.9){

          s->estimated_plane = report.success;

          s->estimated_plane_params = report.model;

          s->estimated_plane_confidence = (float)  report.support.num_inliers / (float) s->points3D.size();

          s->estimated_plane_inlier_mask = report.inlier_mask;

          // s->labelColor = cv::Vec3b( 255, 255, 255);

        }
 
      }

    }
  }
}


std::vector<cv::Vec3b> SimpleMerge::GenerateRandomColors(){
  
  // generate random colors
  std::vector<cv::Vec3b> colors;

  for (int r=0; r< 4; r++ ) {
    for (int g=0; g< 8; g++ ) {
      for (int b=0; b< 4; b++ ) {

        cv::Vec3b color = cv::Vec3b( r*64, g*32, b*64) ;

        colors.push_back(color);

      }
    }
  }

  auto rng = std::default_random_engine {};
  std::shuffle(std::begin(colors), std::end(colors), rng);

  colors.insert(std::end(colors),std::begin(colors), std::end(colors)); 
  std::shuffle(std::begin(colors), std::end(colors), rng);


  return colors;
}


void SimpleMerge::SemanticLabel(Image& item, cv::Mat& cls, superpixels_t& list_supepixels){
  // image name
  std::string item_name = GetPathBaseName(item.GetImagePath());
  
  // read mseg  && resize
  std::string  item_mseg_seg   = JoinPaths(_database.workspace, _database.mseg_seg_folder, item_name + ".png");

  MsegSegmentation _mseg = MsegSegmentation(item_mseg_seg);
  cv::Mat semantics = _mseg.Read();


  // Set semantics labesl for each superpixels

  for (int row = 0; row < semantics.rows; row++){
	  
    for (int col = 0; col < semantics.cols; col++){

      cv::Vec3b lbl  = semantics.at<ushort>(row, col);

      list_supepixels[cls.at<ushort>(row, col)]->semanticHistogram.at<float>(lbl.val[0], 0) += 1.0;
      list_supepixels[cls.at<ushort>(row, col)]->semanticHistogram.at<float>(lbl.val[0], 1) += 1.0;   
      list_supepixels[cls.at<ushort>(row, col)]->semanticHistogram.at<float>(lbl.val[0], 2) += 1.0;   

    }
  }

  // normalize

  for (int id = 0; id < list_supepixels.size(); id++){
    cv::normalize(list_supepixels[id]->semanticHistogram, list_supepixels[id]->semanticHistogram, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  }
}


float PlanarSmoothness(Superpixel* a, Superpixel* b)
{
	Eigen::Vector3f a_normal  = std::get<1>(a->estimated_plane_params);
	Eigen::Vector3f b_normal  = std::get<1>(b->estimated_plane_params);

	Eigen::Vector3f step_vector = std::get<0>(a->estimated_plane_params) - std::get<0>(b->estimated_plane_params);
	
  step_vector = step_vector.normalized();

	float p_smoothness = std::max(abs(a_normal.dot(step_vector)), abs(b_normal.dot(step_vector))) / abs(a_normal.dot(b_normal));

	return p_smoothness;
}


superpixels_t SimpleMerge::HasPlanes(superpixels_t& list_supepixels){
  
  superpixels_t has_planes;

  for (auto& s : list_supepixels){

    float overlapped = (float) s->planarPixels / (float) s->points2D.size();
    

    if (s->estimated_plane &&  overlapped > 0.7){

      // empty   
      if (has_planes.size() == 0){ has_planes.push_back(s); }

      // add after comparision
      else{

        bool add = true;
        
        // test co planarities smoothness  withh accepted ones
        for (auto& h: has_planes){
      
          float p_smoothness = PlanarSmoothness(s, h);
          
          // already there
          if (p_smoothness < 2){
            add = false;
            break;
          }
        }

        // add to hypothesis list
        if (add)
          has_planes.push_back(s);
      }
    }
  }

  // accepted planes
  return has_planes;
}


superpixels_t SimpleMerge::GetPlanarCandidates(superpixels_t& list_supepixels){
  
  superpixels_t candidates;

  for (auto& s : list_supepixels){

    if (s->estimated_plane ){
      candidates.push_back(s);
    }
  }

  return candidates;
}


void SimpleMerge::PlaneLabel(Image& item, cv::Mat& cls, superpixels_t& list_supepixels){

  // image name
  std::string item_name = GetPathBaseName(item.GetImagePath());
  
  // read planes  && resize
  std::string  item_plane_seg   = JoinPaths(_database.workspace, _database.plane_seg_folder, item_name + ".masks.npy");
  std::string  item_plane_prams = JoinPaths(_database.workspace, _database.plane_seg_folder, item_name + ".params.npy");

  PlaneSegmentation _planes = PlaneSegmentation(item_plane_seg, item_plane_prams);
  cv::Size img_size(item.GetWidth(), item.GetHeight());
  std::vector<cv::Mat> plane_seg = _planes.Read(img_size);

  int plane_id = 0;

  std::vector<cv::Vec3b> color_map = GenerateRandomColors();

  // get planar candidates
  superpixels_t plane_candidates = GetPlanarCandidates(list_supepixels);
  std::cout << "Num Plane Candidates " << plane_candidates.size() << std::endl;

  // loop over each plane
  for (auto& plane: plane_candidates){

    if (plane->planarlbl)   // already taken by previous plane
        continue;
 
    // Initializing the problem graph for the graph-cut algorithm.
		typedef Graph<double,double,double> GraphType;
    
    GraphType * problem = new GraphType( list_supepixels.size(),  list_supepixels.size()); 

		for(int i=0; i<list_supepixels.size(); i++){
      problem->add_node();
    }

    // Set unary term 
    for(auto& s : list_supepixels){

      // unary cost function 
      double U = unary_term(s, plane);

      if (s->estimated_plane){

        if (U <0.08)
          problem->add_tweights( s->id, 0, 999); //unary weight
        else
          problem->add_tweights( s->id, 999, 0); //unary weight
      }else{
        problem->add_tweights( s->id, 0, 0); //unary weight
      }   
    }
    

    // cut maxflow
    int flow = problem->maxflow();
    for(auto& s : list_supepixels){
        
      if (s->planarlbl)   // already taken !
        continue;

      if (problem->what_segment(s->id) == GraphType::SINK){
          
        s->planarlbl = true;
        s->plane_id = plane_id;
        s->labelColor = color_map.at(plane_id);
      }

      if (problem->what_segment(s->id) == GraphType::SOURCE){
          
        s->labelColor = cv::Vec3b(255, 255, 255);
      }
        
        
    }
    
    // delete graph
    delete problem;

    // reset plane histogram to zero
    for(auto& s : list_supepixels){
      s->planarPixels = 0;
    }

    plane_id ++;
 
	} // plane candidates
} 


void SimpleMerge::WriteNewLabels(Image& item, cv::Mat& cls, cv::Mat& lbl, superpixels_t& list_supepixels){
  // image name
  std::string item_name = GetPathBaseName(item.GetImagePath());

  // write output
  std::string  item_superpixels_dir = JoinPaths(_database.workspace, _database.test);
  std::string  item_superpixels_lbl = JoinPaths(_database.workspace, _database.test, item_name + ".png");

  // create directory 
  CreateDirIfNotExists(item_superpixels_dir);

  // create output labels
  
  cv::Mat newlbl = lbl;
  
  std::cout << "newLabels" << newlbl.size()<< std::endl;

  for (int row = 0; row < cls.rows; row++){
		for (int col = 0; col < cls.cols; col++){
			newlbl.at<cv::Vec3b>(row, col) = list_supepixels[cls.at<ushort>(row, col)]->labelColor;
		}
	}
  
  cv::imwrite(item_superpixels_lbl, newlbl);

}


void SimpleMerge::Run(){

  // read all images
  std::vector<Image> images = _database.ReadAllImages();

  for (auto& item: images){

    // image name
    std::string item_name = GetPathBaseName(item.GetImagePath());

    // read superpixels
    std::string  item_superpixels_cls = JoinPaths(_database.workspace, _database.superpixels_folder, item_name + ".csv");
    std::string  item_superpixels_lbl = JoinPaths(_database.workspace, _database.superpixels_folder, item_name + ".png");
    
    SuperpixelsSegementation _superpixels = SuperpixelsSegementation(item_superpixels_cls, item_superpixels_lbl);

    cv::Mat lbl = _superpixels.ReadLabels();
    cv::Mat cls = _superpixels.ReadCSV();

    std::cout <<  lbl.size() << "   " << lbl.channels() <<  std::endl;
    std::cout <<  cls.size() << "   " << cls.channels() <<  std::endl;

    // begin 
  
    // we allocate superpixels for each image in vector in the heap 
    // we initilize each superpixel with appearance and geo modesl

    const int num_superpixels = *(std::max_element(cls.begin<ushort>(), cls.end<ushort>())) + 1;		

    std::vector<Superpixel*> listSuperpixels(num_superpixels);

    for (unsigned int id = 0; id < num_superpixels; id++){ listSuperpixels[id] = new Superpixel(id); }

    // load initial models
    LoadSuperpixelList(item, listSuperpixels);

    // assign to each superpixel all neighbors 
    AssignNeighbouring(cls , listSuperpixels); 

    // find planar supports 
    EstimatePlaneParameters(listSuperpixels); 

    // Semantic lables map
    SemanticLabel(item, cls, listSuperpixels);

    // // plane merge 
    PlaneLabel(item, cls, listSuperpixels);

    // write results
    std::cout << "write new image" << std::endl;
    WriteNewLabels(item, cls, lbl, listSuperpixels);

  } // All images



} // Run 


