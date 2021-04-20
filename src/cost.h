#ifndef COST_H_
#define COST_H_

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

#include "superpixels.h"



double unary_term(Superpixel* s, Superpixel* plane){

  // overlapping coefficient 
  double lamda_smoothness = 0.1;

  double p_smoothness;
  
  if (s->estimated_plane){
    
    Eigen::Vector3f s_normal      = std::get<1>(s->estimated_plane_params);
    Eigen::Vector3f plane_normal  = std::get<1>(plane->estimated_plane_params);

    Eigen::Vector3f step_vector = std::get<0>(s->estimated_plane_params) - std::get<0>(plane->estimated_plane_params);
    
    step_vector = step_vector.normalized();

    p_smoothness = std::max(abs(s_normal.dot(step_vector)), abs(plane_normal.dot(step_vector))) / abs(s_normal.dot(plane_normal));
  }
  else{
    p_smoothness = 999;
  }
  
	return p_smoothness;
}

double local_term(Superpixel* s, Superpixel* n){

  // appearance distance
  double lamda_appearance = 0.2;
  double lamda_semantic = 0.6;

  double appearance_distance = cv::compareHist(s->appearanceHistogram, n->appearanceHistogram, CV_COMP_BHATTACHARYYA);

  double semantic_distance = cv::compareHist(s->semanticHistogram, n->semanticHistogram, CV_COMP_BHATTACHARYYA);
  
  double V1, V2;

  if (appearance_distance > 2 *lamda_appearance) { V1 = 0; } 
  else{ V1 = 2.0 * exp(-appearance_distance / lamda_appearance) ; }

  if (semantic_distance > 0.4) { V2 = 0; } 
  else{ V2 = 2.0 * exp(-semantic_distance / lamda_semantic) ; }
   
  double V = V1 + V2;

  //std::cout << appearance_distance << "  " << V1 << "  " << semantic_distance << "  " << V2 <<" "<< V  << std::endl;

  if (V< 0)
    V=0;

  return V;
}

#endif // COST_H_

