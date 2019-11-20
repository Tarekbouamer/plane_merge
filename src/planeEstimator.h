/*
 * planeEstimator.h
 *
 *  Created on: Apr 2, 2019
 *      Author: dekuhnan
 */

#ifndef MODULES_DEPTHCOMPLETION_PLANEESTIMATOR_H_
#define MODULES_DEPTHCOMPLETION_PLANEESTIMATOR_H_

#include <eigen3/Eigen/Dense>

#include <colmap/optim/ransac.h>
#include <colmap/optim/loransac.h>

#include <colmap/util/logging.h>

#include <vector>
#include <tuple>


class PlaneEstimator{
public:
	// The 3D point clouds
	typedef Eigen::Vector3f X_t;
	// The 3D uncertainty
	typedef float Y_t;

	// The minimum number of samples needed to estimate a plane.
	static const int kMinNumSamples = 3;

	// The 3D point clouds and uncerainties
	typedef std::tuple<X_t, X_t, Y_t> M_t;

	static std::vector<M_t> Estimate(const std::vector<X_t>& points3D,
			const std::vector<Y_t>& qual3D);

	static void Residuals(const std::vector<X_t>& points3D,
			const std::vector<Y_t>& qual3D,
			const M_t& proj_matrix, std::vector<double>* residuals);
};


#endif /* MODULES_DEPTHCOMPLETION_PLANEESTIMATOR_H_ */
