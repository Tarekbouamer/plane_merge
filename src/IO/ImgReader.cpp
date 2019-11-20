/*
 * ImgReader.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: dekuhnan
 */

#include "ImgReader.h"
#include <iostream>
/*Reading a one-chanel float image from txt file*/
cv::Mat
ImgIO::read(std::string filename){
	std::cout << "Going to read file " << filename << "..." << std::endl;
	std::vector<float> data_;

	std::fstream text_file;
	text_file.open(&filename[0], std::fstream::in | std::fstream::binary);
	if(!text_file.good())
	{
		std::cout << "$$$$$   File DAMAGED ======> "<< std::endl;
		return cv::Mat();
	}
	int width, height, depth;

	char unused_char;
	text_file >> width >> unused_char >> height >> unused_char >> depth >> unused_char;
	//std::cout << width << " " << height << " " << depth << std::endl;
	std::streampos pos = text_file.tellg();
	text_file.close();
	data_.resize(width * height * depth);

	std::fstream binary_file(&filename[0], std::fstream::in | std::fstream::binary);
	binary_file.seekg(pos);
	binary_file.read(reinterpret_cast<char*>(data_.data()), data_.size() * sizeof(float));
	binary_file.close();

	int index=0;
	cv::Mat M(height, width, CV_32FC1);
	for(int y=0; y<height; y++){
		for(int x=0; x<width; x++){
			float val = data_[index++];
			M.at<float>(y, x)=val;
		}
	}
	return M;
}

/*Reading a three-chanel float image from txt file*/
cv::Mat
ImgIO::read2(std::string filename){
	std::cout << "Going to read file " << filename << "..." << std::endl;
	std::vector<float> data_;

	std::fstream text_file;
	text_file.open(&filename[0], std::fstream::in | std::fstream::binary);
	if(!text_file.good())
	{
		return cv::Mat();
	}

	int width, height, depth;

	char unused_char;
	text_file >> width >> unused_char >> height >> unused_char >> depth >> unused_char;
	//std::cout << width << " " << height << " " << depth << std::endl;
	std::streampos pos = text_file.tellg();
	text_file.close();
	data_.resize(width * height * depth);

	std::fstream binary_file(&filename[0], std::fstream::in | std::fstream::binary);
	binary_file.seekg(pos);
	binary_file.read(reinterpret_cast<char*>(data_.data()), data_.size() * sizeof(float));
	binary_file.close();

	int index=0;
	cv::Mat M(height, width, CV_32FC3);
	for(int d=0; d<depth; d++){
		for(int y=0; y<height; y++){
			for(int x=0; x<width; x++){
				float val = data_[index++];
				M.at<cv::Vec3f>(y, x).val[d]=val;
			}
		}
	}
	return M;
}

/*Writing a one-chanel float image to txt file*/
void
ImgIO::write(std::string filename, cv::Mat& M){
	std::cout << "Going to write to file: " << filename << "..." << std::endl;
	std::fstream text_file(&filename[0], std::ios_base::out);
	text_file << M.cols << "&" << M.rows << "&" << 1 << "&";
	text_file.close();

	std::vector<float> data_;
	data_.resize(M.rows * M.cols * 1);

	int index=0;
	for(int y=0; y<M.rows; y++){
		for(int x=0; x<M.cols; x++){
			data_[index++]=M.at<float>(y,x);
		}
	}

	std::fstream binary_file(&filename[0], std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	binary_file.write(reinterpret_cast<const char*>(data_.data()), sizeof(float) * M.cols * M.rows * 1);
    binary_file.close();
}

/*Writing a one-chanel float image to txt file*/
void
ImgIO::write2(std::string filename, cv::Mat& M){
	std::cout << "Going to write to file: " << filename << "..." << std::endl;
	std::fstream text_file(&filename[0], std::ios_base::out);
	text_file << M.cols << "&" << M.rows << "&" << 3 << "&";
	text_file.close();

	std::vector<float> data_;
	data_.resize(M.rows * M.cols * 3);

	int index=0;
	for(int d=0; d<3; d++){
		for(int y=0; y<M.rows; y++){
			for(int x=0; x<M.cols; x++){
				data_[index++]=M.at<cv::Vec3f>(y, x).val[d];
			}
		}
	}

	std::fstream binary_file(&filename[0], std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	binary_file.write(reinterpret_cast<const char*>(data_.data()), sizeof(float) * M.cols * M.rows * 3);
    binary_file.close();
}
