/*
 * paramsCam.h
 *
 *  Created on: Dec 5, 2017
 *      Author: dekuhnan
 */

#ifndef PARAMSCAM_H_
#define PARAMSCAM_H_

#include <iostream>
#include <sstream>
#include <fstream>

#include <eigen3/Eigen/Dense>


class ParamsCam{
public:
	/** Reads camera calibration: camera matrix, rotation, position, image width/height */
	bool readCamConfig(const std::string& filename);
	void getCamConfig(Eigen::Vector3d& _cameraT, Eigen::Matrix3d& _cameraR, Eigen::Matrix3d& _cameraA,
			Eigen::Matrix3d& _cameraAInv, int& id);
	Eigen::Vector3d getCamT();
private:
	Eigen::Vector3d cameraT;
	Eigen::Matrix3d cameraR;
	Eigen::Matrix3d cameraA;
	Eigen::Matrix3d cameraAInv;
	int id;
};

#endif /* PARAMSCAM_H_ */
