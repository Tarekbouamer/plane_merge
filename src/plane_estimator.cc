#include "plane_estimator.h"

std::vector<PlaneModel::M_t> PlaneModel::Estimate(const std::vector<X_t>& points3D, const std::vector<Y_t>& qual3D) {

	if (points3D.size() != 3){	std::cout << "Error" <<std::endl;	};

	if (qual3D.size() != 3){ 	std::cout << "Error" <<std::endl; 	};

	X_t normal = (points3D[1]-points3D[0]).cross(points3D[2] - points3D[0]).normalized();

	std::vector<M_t> models(1);
	
	std::get<0>(models[0]) = points3D[0];
	std::get<1>(models[0]) = normal;
	std::get<2>(models[0]) = (qual3D[0] + qual3D[1] + qual3D[2]) / 3.0;

	return models;
}

void PlaneModel::Residuals(	const std::vector<X_t>& points3D, 
														const std::vector<Y_t>& qual3D,	
														const M_t& proj_matrix, 
														std::vector<double>* residuals) {

	if (points3D.size() != residuals->size()){	std::cout << "Error" <<std::endl;		};

	for(unsigned int i=0; i<points3D.size(); i++){
		(*residuals)[i] = fabs(std::get<1>(proj_matrix).dot(points3D[i]-std::get<0>(proj_matrix)) /(qual3D[i])  );
	}
}

