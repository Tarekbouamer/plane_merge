
#ifndef SUPERPIXELS_H_
#define SUPERPIXELS_H_

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

#include <eigen3/Eigen/Dense>
#include <math.h>

#include "lib/seed/SeedsRevised.h"
#include "lib/seed/Tools.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "database.h"


// set superpixels options 

struct SuperpixelsOptions {

	int		NumSuperpixels		=	200;
	int 	NumberOfBins 			= 5;
	int		NeighborhoodSize 	= 4;
	float	MinimumConfidence = 0.500000001;
	int 	Iterations 				= 10;
	float SpatialWeight 		= 0.25;

};


// object
struct Superpixel{

	unsigned int id;

	unsigned int numPixels;
	unsigned int numDepths;
	
	std::vector<Eigen::Vector2i> points2D;
	std::vector<Eigen::Vector3f> points3D;
	
	std::vector<cv::Point> contour2D;  // check "contour_map", 
	std::vector<cv::Point> hull2D;

	std::vector<Eigen::Vector3f> hull3D;


	std::vector<float> quality3D;
	std::vector<float> confs3D;

	// mono depth
	std::vector<Eigen::Vector3f> mono_points3D;
	std::vector<float> mono_quality3D;

	cv::Vec3b labelColor;
	cv::Mat appearanceHistogram;

	std::set<Superpixel*> neighbors;
	std::map<Superpixel*, double>  contour_map; 

	// estimation
	bool estimated_plane;
	std::tuple<Eigen::Vector3f, Eigen::Vector3f, float> estimated_plane_params;
	float estimated_plane_confidence;
	std::vector<char> estimated_plane_inlier_mask;

	// plane lbl
	bool planarlbl;
	unsigned int planarPixels;
	int plane_id;
	cv::Mat planarHistogram = cv::Mat::zeros(cv::Size(1, 255), CV_32F);	

	// semantic lbl
	std::unordered_map<int , double> semantics; 
	int num_semantic_classes = 194;
	cv::Mat semanticHistogram = cv::Mat::zeros(cv::Size(1, 194), CV_32F);	

	// estimation
	bool mono_plane;
	std::tuple<Eigen::Vector3f, Eigen::Vector3f, float> mono_plane_params;
	float mono_plane_confidence;
	std::vector<char> mono_plane_inlier_mask;

	// constructor
	Superpixel(unsigned int ID){
		id = ID;
		
		numPixels=0;
		numDepths =0;

		labelColor= cv::Vec3b(0,0,0);
		appearanceHistogram = cv::Mat::zeros(cv::Size(3, 256), CV_32F);

		planarlbl = false;
		planarPixels = 0 ;
		plane_id = -1;
		
		//
		estimated_plane = false;
		estimated_plane_confidence = 0.;

		//
		mono_plane = false;
		mono_plane_confidence = 0.;
		
		// init semantic map
		int k = 0;
		
		while(k < num_semantic_classes){	
			semantics.insert({ k, 0 });	
			k++; 
		}
	};

	bool operator()(const Superpixel& a, const Superpixel& b) const { a.estimated_plane_confidence < b.estimated_plane_confidence; }

};

// image
class SuperpixelsSegementation{

public:

	SuperpixelsSegementation()=default;

	explicit SuperpixelsSegementation(const std::string& superpixels_path_cls, const std::string& superpixels_path_lbl ); 

	cv::Mat ReadLabels(const std::string& path);
	cv::Mat ReadLabels();

	cv::Mat ReadCSV(const std::string& path, const cv::Mat label);
	cv::Mat ReadCSV();

private:
	std::string _superpixels_path_cls;
	std::string _superpixels_path_lbl;

};


 // generator
class SuperpixelsGenerator {
public:

	SuperpixelsGenerator()=default;
	explicit SuperpixelsGenerator(const std::string& database_path );

	void Run();
	
	private:
	
	Database database_;
	SuperpixelsOptions options;

};

// line segment
struct LineSegment {
  Eigen::Vector2d start;
  Eigen::Vector2d end;
};

#endif /* SUPERPIXELS_H_ */
