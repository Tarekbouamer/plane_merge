/*
 * ImgReader.h
 *
 *  Created on: Dec 22, 2017
 *      Author: dekuhnan
 */

#ifndef IO_IMGREADER_H_
#define IO_IMGREADER_H_

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

class ImgIO{
public:
	cv::Mat read(std::string filename);
	cv::Mat read2(std::string filename);
	void write(std::string filename, cv::Mat& M);
	void write2(std::string filename, cv::Mat& M);
};

#endif /* IO_IMGREADER_H_ */
