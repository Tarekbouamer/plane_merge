#ifndef COST_H_
#define COST_H_

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

#include "superpixels.h"
#include "mseg_segmentation.h"


std::map<int, std::string> planarities = {
  
  {36, "building"}, 
  {37, "ceiling"}, 
  
  {44, "floor"}, 
  
  {81, "door"}, 
  
  {86, "mirror"}, 
  {87, "whiteboard"}, 
  {88, "shelf"}, 
  {80, "stairs"}, 
  {90, "escalator"}, 
  {96, "platform"}, 
  {99, "road"},

  {101, "sidewalk_pavement"},
  {102, "runway"},
  {103, "terrain"},
  {104, "book"},
  {105, "box"},

  {113, "painting"},

  {136, "traffic_sign"},
  
  {145, "box"},
  
  {192, "wall"},
  {193, "window"},
  {194, "window_blind"},
  
  };


double PlanarSmoothness(Superpixel* a, Superpixel* b)
{
	Eigen::Vector3f a_normal  = std::get<1>(a->estimated_plane_params);
	Eigen::Vector3f b_normal  = std::get<1>(b->estimated_plane_params);

	Eigen::Vector3f step_vector = std::get<0>(b->estimated_plane_params) - std::get<0>(a->estimated_plane_params);
	
  step_vector = step_vector.normalized();

	double p_smoothness = std::max(abs(a_normal.dot(step_vector)), abs(b_normal.dot(step_vector))) / abs(a_normal.dot(b_normal));

	return p_smoothness;
}


double PlanarMonoSmoothness(Superpixel* a, Superpixel* b)
{
	Eigen::Vector3f a_normal  = std::get<1>(a->mono_plane_params);
	Eigen::Vector3f b_normal  = std::get<1>(b->mono_plane_params);

	Eigen::Vector3f step_vector = std::get<0>(b->mono_plane_params) - std::get<0>(a->mono_plane_params);
	
  step_vector = step_vector.normalized();

	double p_smoothness = std::max(abs(a_normal.dot(step_vector)), abs(b_normal.dot(step_vector))) / abs(a_normal.dot(b_normal));

	return p_smoothness;
}




double unary_term(Superpixel* s, Superpixel* plane){

  // inlier distance thd
  double tau = 0.001;
  double sigma_distance = 500;

  Eigen::Vector3f p_point   =   std::get<0>(plane->estimated_plane_params);
  Eigen::Vector3f normal    =   std::get<1>(plane->estimated_plane_params);

  double total_distance = 0.0;
  double U = 0.0;
  
  // support points 
  double num_support = (double) s->points3D.size() /  (double) s->points2D.size();
  
  for (unsigned int i=0; i< s->points3D.size(); i++)
  {
    // distance point to plane
    double dist = fabs(p_point.dot(normal) -  s->points3D[i].dot(normal)) / (s->quality3D[i])  ;

		total_distance +=  exp( - pow(dist, 2) / (2 * pow(tau, 2) ) );
  }

  U = plane->estimated_plane_confidence * exp(total_distance/sigma_distance);

  return U ;
}


double local_term(Superpixel* s, Superpixel* n){

  // 1. appearance distance
  double lamda_appearance = 0.1;
  double appearance_distance  = cv::compareHist(s->appearanceHistogram, n->appearanceHistogram, CV_COMP_BHATTACHARYYA);
  double V1 = exp(- appearance_distance / lamda_appearance) ;


  // 3. plane smoothness
  double lamda_smoothness = 0.001;
  double smoothness = 100000.0;

  if (s->estimated_plane && n->estimated_plane){
    smoothness = PlanarSmoothness(s, n);
  }
    
  double V3 = exp(- smoothness / lamda_smoothness) ;


  // 4. mono plane smoothness
  double lamda_mono_smoothness = 0.001;
  double mono_smoothness = 100000.0;

  if (s->mono_plane && n->mono_plane){
    mono_smoothness = PlanarMonoSmoothness(s, n);
  }
    
  double V4 = exp(- mono_smoothness / lamda_mono_smoothness) ;
  

  // 5. weights connections
  double lamda_contour = 0.2;
  double sum_weights = 0.0;

  for (auto& c: s->contour_map){
    sum_weights += c.second;
  }

  double W;

  if (sum_weights > 0.0)
    W = 1 - exp(-(1.0/(sum_weights) * (s->contour_map[n]))/lamda_contour);
  else
    W = 0.0;
  
  double V = 1 * W *(V1);

  if (V < 0.0){
    V = 0.0;
  }

  return V;
}

#endif // COST_H_

