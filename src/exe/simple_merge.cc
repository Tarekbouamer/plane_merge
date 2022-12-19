#include <algorithm>
#include <random>
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "simple_merge.h"
#include "reconstruction/depth.h"
#include "reconstruction/normal.h"
#include "reconstruction/quality.h"
#include "reconstruction/confidence.h"

#include "lib/graphcuts/GCoptimization.h"
#include "lib/graphcuts/graph.h"

#include "semantics/plane_segmentation.h"
#include "semantics/mseg_segmentation.h"
#include "semantics/mono_depth.h"

#include "base/line3D.h"

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
  
  std::cout << "_database.workspace"      << _database.workspace    << std::endl;
  std::cout << "_database.depth_folder"   << _database.depth_folder << std::endl;
  std::cout << "item_depth"   << item_depth << std::endl;

  cv::Mat depth_map = Depth(item_depth).Read();

  // read quality
  std::string  item_quality = JoinPaths(_database.workspace, _database.quality_folder, item_name + ".JPG.geometric.bin");
  cv::Mat quality_map = Quality(item_quality).Read();

  // compute  uncertanity map
	cv::Mat uncertainty_map = cv::Mat(quality_map.rows, quality_map.cols, CV_32FC1, INFINITY);  
  ComputeUncertaintyMap(depth_map, quality_map, uncertainty_map);    
  
    // read mono depth
  std::string  item_mono_depth   = JoinPaths(_database.workspace, _database.mono_depth_folder, item_name + ".pfm");
  cv::Mat mono_depth = MonoDepth(item_mono_depth).Read();
  
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

      // mono depth (in fact disparity !! )
      float pixelmonoD  = 1 / ( 0.001 * mono_depth.at<float>(row, col) ) ;

      // homogeneous coordinates 
      Eigen::Vector3f coord(float(row), float(col), 1.0);

      // project
      Eigen::Vector3f point = item.GetK().inverse().cast<float>() * coord * pixelmonoD ;
      
      list_supepixels[cls.at<ushort>(row, col)]->mono_points3D.push_back(point);
      list_supepixels[cls.at<ushort>(row, col)]->mono_quality3D.push_back(1.0);

      //  geo model 
      if (depth_map.at<float>(row, col) != INFINITY && depth_map.at<float>(row, col) != 0.0){
        
        //depth 
        float pixelD = depth_map.at<float>(row, col);

        // homogeneous coordinates 
        Eigen::Vector3f coord(float(row), float(col), 1.0);

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

  float avg_pixles = 0.0;
  
  for (int id = 0; id < list_supepixels.size(); id++){
    avg_pixles += ( list_supepixels[id]->numPixels) / ( (float) (list_supepixels.size()) );
  }

  std::cout << "2d pixels count = " << avg_pixles << std::endl;
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
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y - 1, x - 1)];
          
          // append neighbors
          current.neighbors.insert(key);

          // add 2d point to contour 
          current.contour2D.push_back(cv::Point(x, y));
          
          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }
				}
			} //

			if (y - 1 > 0){
        if (cls.at<unsigned short>(y - 1, x) != id){
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y - 1, x)];
          
          current.neighbors.insert(key);
          
          current.contour2D.push_back(cv::Point(x, y));

          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }				
        }
			} //

			if (x + 1 < cls.cols && y - 1 > 0){
				if (cls.at<unsigned short>(y - 1, x + 1) != id){
                   
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y - 1, x + 1)];
          
          current.neighbors.insert(key);

          current.contour2D.push_back(cv::Point(x, y));

          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }	
				}
			} //

			if (x - 1 > 0){
				if (cls.at<unsigned short>(y, x - 1) != id){
          
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y, x - 1)];
          
          current.neighbors.insert(key);

          current.contour2D.push_back(cv::Point(x, y));

          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }						
        }
			} //

			if (x + 1 < cls.cols){
				if (cls.at<unsigned short>(y, x + 1) != id){
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y, x + 1)];
          
          current.neighbors.insert(key);
          
          current.contour2D.push_back(cv::Point(x, y));

          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }
				}
			} //

			if (x - 1 > 0 && y + 1 < cls.rows){
				if (cls.at<unsigned short>(y + 1, x - 1) != id){
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y + 1, x - 1)];
          
          current.neighbors.insert(key);
          
          current.contour2D.push_back(cv::Point(x, y));
          
          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }
        }
			} //

			if (y + 1 < cls.rows){
				if (cls.at<unsigned short>(y + 1, x) != id){
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y + 1, x)];
          
          current.neighbors.insert(key);

          current.contour2D.push_back(cv::Point(x, y));

          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }
				}
			} //

			if (x + 1 < cls.cols && y + 1 < cls.rows){
				if (cls.at<unsigned short>(y + 1, x + 1) != id){
          Superpixel* key  = list_supepixels[cls.at<unsigned short>(y + 1, x + 1)];
          
          current.neighbors.insert(key);

          current.contour2D.push_back(cv::Point(x, y));
         
          if (  current.contour_map.find(key) == current.contour_map.end() ) 
          {
            current.contour_map.insert({key, 1.0});
          } 
          else 
          {
            current.contour_map[key]++;
          }
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
	ransac_options.min_num_trials 	= 200;
	ransac_options.max_num_trials 	= 20000;
  ransac_options.min_inlier_ratio 	= 0.2;
  
	RANSAC<PlaneModel, MEstimatorSupportMeasurer, RandomSampler> ransac(ransac_options);

  // iterate over all superpixels

  for (auto& s :list_supepixels){

    // don't estimate plane for not well supported superpixels
    float num_support = (float) s->points3D.size() /  (float) s->points2D.size();

    if (num_support > 0.3){
      
      auto report = ransac.Estimate(s->points3D, s->quality3D);

      if (report.success){
        
        float confidence = (float)  report.support.num_inliers / s->points3D.size();


        if (confidence>0.5){

          s->estimated_plane = report.success;

          s->estimated_plane_params = report.model;

          s->estimated_plane_confidence = confidence;

          s->estimated_plane_inlier_mask = report.inlier_mask;

        }
 
      }

    }
  }
}


void SimpleMerge::ProjectHullToPlane(Superpixel* s, const Eigen::Matrix3f inv_K){

  const float max_depth = 100;

  for (auto& h_point2d : s->hull2D ){
    
    // starting point (origin)
    Eigen::Vector3f T0(0.0, 0.0, 0.0);

    // hull point
    Eigen::Vector3f T1(h_point2d.y, h_point2d.x, 1.0);
    T1 = inv_K * T1 * max_depth;

    // ray 
    Line3D ray = Line3D(T0, T1);

    // plane
    Eigen::Vector3f p_point   = std::get<0>(s->estimated_plane_params);
    Eigen::Vector3f p_normal  = std::get<1>(s->estimated_plane_params);
    
    // intersection 
    float k =  p_normal.dot(p_point - ray.GetBase()) / p_normal.dot(ray.GetDirection());

    Eigen::Vector3f p_intersection = ray.GetBase() + (k * ray.GetDirection());
    
    // 
    s->hull3D.push_back(p_intersection);

  }

}


void SimpleMerge::MonteCarloPertutbation(Image& item, superpixels_t& list_supepixels){

  for (auto& s: list_supepixels){
    
    // if we have an estimate
    if (s->estimated_plane){

      // create convex hull
      cv::Mat hull = cv::Mat(s->contour2D);
      cv::convexHull(s->contour2D, s->hull2D, false);
      
      const Eigen::Matrix3f inv_K = item.GetK().inverse().cast<float>();
      
      // project hull to plane
      ProjectHullToPlane(s, inv_K);


    }
  }

}


void SimpleMerge::EstimateMonoPlaneParameters(superpixels_t& list_supepixels){
  
  // init ransac
  RANSACOptions ransac_options;

	ransac_options.max_error 			  = 1.0;
	ransac_options.confidence 			= 0.99;
	ransac_options.min_num_trials 	= 50;
	ransac_options.max_num_trials 	= 300;
  ransac_options.min_inlier_ratio 	= 0.001;
  
	RANSAC<PlaneModel, MEstimatorSupportMeasurer, RandomSampler> ransac(ransac_options);

  // iterate over all superpixels

  for (auto& s :list_supepixels){
  
    auto report = ransac.Estimate(s->mono_points3D, s->mono_quality3D);

    if (report.success){

      s->mono_plane = report.success;

      s->mono_plane_params = report.model;

      s->estimated_plane_confidence = (float)  report.support.num_inliers / (float) s->mono_points3D.size();

      s->mono_plane_inlier_mask = report.inlier_mask;

    }
    else{
      std::cout << s->id  << std::endl;
    }
  }

}


std::vector<cv::Vec3b> SimpleMerge::GenerateRandomColors(){
  
  // generate random colors
  std::vector<cv::Vec3b> colors;

  for (int r=0; r< 3; r++ ) {
    for (int g=0; g< 6; g++ ) {
      for (int b=0; b< 4; b++ ) {

        cv::Vec3b color = cv::Vec3b( r*64, g*42, b*64) ;

        colors.push_back(color);

      }
    }
  }

  auto rng = std::default_random_engine {};
  std::shuffle(std::begin(colors), std::end(colors), rng);
  colors.insert(std::end(colors),std::begin(colors), std::end(colors));
  std::shuffle(std::begin(colors), std::end(colors), rng);
  colors.insert(std::end(colors),std::begin(colors), std::end(colors)); 


  return colors;
}


void SimpleMerge::LoadSemanticLabel(Image& item, cv::Mat& cls, superpixels_t& list_supepixels){
  
  // image name
  std::string item_name = GetPathBaseName(item.GetImagePath());
  
  // read mseg  && resize
  std::string  item_mseg_seg   = JoinPaths(_database.workspace, _database.mseg_seg_folder, item_name + ".png");

  std::cout << item_name << std::endl;

  MsegSegmentation _mseg = MsegSegmentation(item_mseg_seg);
  cv::Mat semantics = _mseg.Read();

  std::cout << semantics.size() << std::endl;


  // Set semantics labesl for each superpixels

  for (int row = 0; row < semantics.rows; row++){
	  
    for (int col = 0; col < semantics.cols; col++){

      cv::Vec3b lbl  = semantics.at<ushort>(row, col) + 1;
      
      list_supepixels[cls.at<ushort>(row, col)]->semanticHistogram.at<float>(lbl.val[0], 0) += 1.0;

    }
  }

  // normalize
  for (int id = 0; id < list_supepixels.size(); id++){
    cv::normalize(list_supepixels[id]->semanticHistogram, list_supepixels[id]->semanticHistogram, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  }
}


void SimpleMerge::LoadPlaneLabel(Image& item, cv::Mat& cls, superpixels_t& list_supepixels){
  // image name
  std::string item_name = GetPathBaseName(item.GetImagePath());
  
  // read planes  && resize
  std::string  item_plane_seg   = JoinPaths(_database.workspace, _database.plane_seg_folder, item_name + ".masks.npy");
  std::string  item_plane_prams = JoinPaths(_database.workspace, _database.plane_seg_folder, item_name + ".params.npy");

  PlaneSegmentation _planes = PlaneSegmentation(item_plane_seg, item_plane_prams);
  cv::Size img_size(item.GetWidth(), item.GetHeight());
  std::vector<cv::Mat> plane_seg = _planes.Read(img_size);

  // Set semantics labesl for each superpixels
  for (int k=0; k<plane_seg.size(); k++){

    for (int row = 0; row < plane_seg[k].rows; row++){
      
      for (int col = 0; col < plane_seg[k].cols; col++){

        cv::Vec3b lbl  = plane_seg[k].at<float>(row, col);

        list_supepixels[cls.at<ushort>(row, col)]->planarHistogram.at<float>(k, 0) += lbl[0];
        list_supepixels[cls.at<ushort>(row, col)]->planarHistogram.at<float>(k, 1) += lbl[0];   
        list_supepixels[cls.at<ushort>(row, col)]->planarHistogram.at<float>(k, 2) += lbl[0];

        list_supepixels[cls.at<ushort>(row, col)]->planarPixels += lbl[0];

      }
    }

  }

  // normalize
  for (int id = 0; id < list_supepixels.size(); id++){
    cv::normalize(list_supepixels[id]->planarHistogram, list_supepixels[id]->planarHistogram, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  }
}


superpixels_t SimpleMerge::GetPlanarCandidates(superpixels_t& list_supepixels){
  
  superpixels_t candidates;

  for (auto& s : list_supepixels){

    if (s->estimated_plane ){
      candidates.push_back(s);
    }
  }

  std::sort(candidates.begin(), candidates.end(), [](Superpixel* a, Superpixel* b){ return a->estimated_plane_confidence < b->estimated_plane_confidence; });

  return candidates;
}


void SimpleMerge::PlaneLabel(Image& item, cv::Mat& cls, superpixels_t& list_supepixels, cv::Mat& lbl){

  std::vector<cv::Vec3b> color_map = GenerateRandomColors();

  // get planar candidates
  superpixels_t plane_candidates = GetPlanarCandidates(list_supepixels);
  std::cout << "Num Plane Candidates " << plane_candidates.size() << std::endl;
  
  int plane_id = 0;

  // loop over each plane candidates
  for (auto& plane: plane_candidates){
    
    // std::cout << plane->id << "       " << plane->estimated_plane_confidence * 100 << std::endl;

    if (plane->planarlbl)   // already taken by previous plane
        continue;
 
    // Initializing the problem graph for the graph-cut algorithm.
		typedef Graph<double,double,double> GraphType;
    GraphType * problem = new GraphType( list_supepixels.size(),  list_supepixels.size()); 

    // add nodes
		for(int i=0; i<list_supepixels.size(); i++)
    {
      problem->add_node();
    }

    // Set unary term 
    for(auto& s : list_supepixels){

      if (s->planarlbl)   // already taken 
        continue;

      if (s->id == plane->id)
      {
        problem->add_tweights( s->id, unary_term(s, s), 9999999999);
      }
      else
      {
        if (s->estimated_plane)
        {
          problem->add_tweights( s->id, unary_term(s, s), unary_term(s, plane));
        }
        else
        { 
          problem->add_tweights( s->id, 1.00,  unary_term(s, plane) );
        }
      }
    }

    // //Set local_term 
    // for(auto& s : list_supepixels){
      
    //   if (s->planarlbl)   // already taken 
    //     continue;

    //   for (auto& n : s->neighbors){
    //     problem->add_edge(s->id, n->id, local_term(s, n), local_term(n, s));
    //   }
    // }
		
    // Run the standard st-graph-cut algorithm
    double flow = problem->maxflow();

    for(auto& s : list_supepixels){
        
      if (s->planarlbl)   // already taken !
        continue;

      if (problem->what_segment(s->id) == Graph<double, double, double>::SINK && flow >50){
          
        s->planarlbl = true;
        s->plane_id = plane_id;
        s->labelColor = color_map.at(plane_id);
      }
      else{
        s->labelColor = cv::Vec3b(255, 255, 255);
      }
    }
    
    // delete graph
    delete problem;

    // new id ++
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
  
  for (int row = 0; row < cls.rows; row++){
		for (int col = 0; col < cls.cols; col++){
			newlbl.at<cv::Vec3b>(row, col) = list_supepixels[cls.at<ushort>(row, col)]->labelColor;
		}
	}
  
  
  cv::Vec3b color_c = cv::Vec3b( 0, 0, 0 );
  cv::Vec3b color_h = cv::Vec3b( 0, 0, 255 );

  for (auto& c: list_supepixels[110]->contour2D){

    // newlbl.at<cv::Vec3b>(c.y, c.x) = color_c;
  }
  
  for (auto& c: list_supepixels[110]->hull2D){

    newlbl.at<cv::Vec3b>(c.y, c.x) = color_h;
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
    std::cout << "EstimatePlaneParameters" << std::endl;
    EstimatePlaneParameters(listSuperpixels);

    // planar quality using monte carlo perturbation
    // std::cout << "MonteCarloPertutbation" << std::endl;
    // MonteCarloPertutbation(item, listSuperpixels) ;

    // find mono planar supports 
    // std::cout << "EstimateMonoPlaneParameters" << std::endl;
    // EstimateMonoPlaneParameters(listSuperpixels); 

    // Semantic lables map
    std::cout << "LoadSemanticLabel" << std::endl;
    LoadSemanticLabel(item, cls, listSuperpixels);

    // Plane lables map
    std::cout << "LoadPlaneLabel" << std::endl;
    LoadPlaneLabel(item, cls, listSuperpixels);

    // plane merge 
    PlaneLabel(item, cls, listSuperpixels, lbl);

    // write results
    std::cout << "write new image" << std::endl;
    WriteNewLabels(item, cls, lbl, listSuperpixels);

  } // All images



} // Run 


