/*
 * scene.cpp
 *
 *  Created on: 9 Sep 2019
 *      Author: tarek
 */

#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "lib/cnpy/cnpy.h"

#include "scene.h"

Scene::Scene(){
			std::cout << "Loading All maps..."<<std::endl;
		}

Scene::~Scene(){
			std::cout << " Scene deleted...."<< std::endl;
		}

void Scene::log()
		{
			std::cout << "Image Size              = "<< image.size()             << std::endl;
			std::cout << "planeMasks Size         = "<< maskVector.size()        << std::endl;
		}

void Scene::dispFirst()
		{
			cv::namedWindow("First frame", cv::WINDOW_AUTOSIZE);
			cv::imshow("First frame", image[1]);
			cv::waitKey(0);

		}

cv::Mat Scene::readImages(const std::string& path)
		{
			unsigned int flag = 1;

			return cv::imread(path, flag);
		};

cv::Mat Scene::readCSV(const std::string& path, const cv::Mat label)
		{
			cv::Mat classes(label.size(), CV_16UC1);
			std::ifstream myfile(path);
			std::string line;
			int x=0;
			int y=0;
			while (getline(myfile, line)) {
				std::string buff;
				x=0;
				std::stringstream ss(line);
				while(getline(ss, buff, ',')){
					classes.at<ushort>(y,x)=std::atoi(&buff[0]);
					x++;
				}
				y++;
			}
			return classes;
		}

std::vector<cv::Mat > Scene::readNumpy(const std::string masksPath , const cv::Mat lablel )
		{
			std::cout<< "...................................................."	<<std::endl;

			maskVector.erase(maskVector.begin(), maskVector.end());

			cnpy::NpyArray MASK = cnpy::npy_load(masksPath);

			float *d = MASK.data<float>();
			unsigned int NP = MASK.shape[0];	unsigned int H  = MASK.shape[1];	unsigned int W  = MASK.shape[2];

			// Reshape from 1D to 2D Np*HW
			std::vector<std::vector<float>> inData;

			for (unsigned int i= 0; i<NP; i++ )
			{
				std::vector< float > rowData;

				for (unsigned int j = i*H*W; j < (i+1)*H*W; j++ )
				{
					rowData.push_back(d[j]) ;
				}

				inData.push_back(rowData);
			}

			//Reshape from 2D to 3D Np * H* W
			unsigned int f=0; unsigned int x=0; unsigned int y=0;

			for ( unsigned int p=0; p < inData.size(); p++ )
			{
				cv::Mat temp(480,640, CV_32FC1);

				for ( unsigned int hw=0; hw <inData[p].size(); hw++)
				{
					// mapping coordinates
					f = floor(hw/640);
					x = hw - f*640;
					y = f;
					temp.at<float>(y,x) = (float)inData[p][hw];
				}
				cv::resize(temp,temp,lablel.size());
				maskVector.push_back(temp);
			}

			return maskVector;
				}


void Scene::maskDisplay()
		{
			for(std::vector< cv::Mat >::iterator planeMask= maskVector.begin();planeMask!= maskVector.end(); planeMask++)
			{

			}
		}

void Scene::displayClasses( cv::Mat img)
		{
			for (int i=0; i<640; i++)
			{
				for (int j=0; j<480; j++)
				{
					if( img.at<ushort>(j,i)!= (ushort)0 )
					{
						img.at<ushort>(j,i) = (ushort)249;
					}
				}
			}
			cv::namedWindow("Classes", cv::WINDOW_AUTOSIZE);
			cv::imshow("Classes", img);

		}

void Scene::addFrame(const std::string & path, const std::string suffix,
				const std::string ext)
		{
			if (ext.compare(".png")==0) {

				if (suffix.compare("_image_.png")==0) {
					image.push_back(readImages(path));
				}

			}
			else
			{

			}

		}
