/*
 * scene.h
 *
 *  Created on: 9 Sep 2019
 *      Author: tarek
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

class Scene {
public:
	std::vector<cv::Mat > image;
	std::vector<cv::Mat > maskVector;

	Scene();

	~Scene();

	void log();

	void dispFirst();

	cv::Mat readImages(const std::string& path);

	cv::Mat readCSV(const std::string& path, const cv::Mat label);

	std::vector<cv::Mat > readNumpy(const std::string masksPath , const cv::Mat lablel );

	void maskDisplay();

	void displayClasses( cv::Mat img);

	void addFrame(const std::string & path, const std::string suffix,
			const std::string ext);

};


#endif /* SCENE_H_ */
