
#ifndef PLANE_MODEL_H_
#define PLANE_MODEL_H_

#include <fstream>
#include <iostream>
#include <string>

#include <vector>
#include <tuple>

#include <eigen3/Eigen/Dense>

#include "optim/ransac.h"
#include "optim/loransac.h"


class PlaneModel{

public:
	// The 3D points
	typedef Eigen::Vector3f X_t;
	typedef float Y_t;

	// The 3D point and Uncerainties
	typedef std::tuple<X_t, X_t, Y_t> M_t;
	
	// The minimum number of samples needed to estimate a plane.
	static const int kMinNumSamples = 3;

	// Estimator
	static std::vector<M_t> Estimate(const std::vector<X_t>& points3D, const std::vector<Y_t>& qual3D);

	// residuals
	static void Residuals(const std::vector<X_t>& points3D, 
												const std::vector<Y_t>& qual3D,	
												const M_t& proj_matrix, 
												std::vector<double>* residuals);
};


#endif //PLANE_MODEL_H_
