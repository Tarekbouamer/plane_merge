/*
 * superpixels.cpp
 *
 *  Created on: 9 Sep 2019
 *      Author: tarek
 */

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


#include <eigen3/Eigen/Dense>
#include <math.h>

#include "lib/seed/SeedsRevised.h"
#include "lib/seed/Tools.h"

#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include "lib/cnpy/cnpy.h"
#include <complex>
#include <cstdlib>
#include <map>
#include <set>


#include <fstream>

#include <iostream>
#include <string>

#include "database.h"
#include "scene.h"
#include "superpixels.h"

Superpixels::Superpixels(Scene scene,
				Database db,
				std::vector< boost::filesystem::path > imagesPathVect)
{
	std::vector<std::string>::iterator imageName = db.nameList.begin();

	for(std::vector< boost::filesystem::path >::iterator imagePath = imagesPathVect.begin(); imagePath!=imagesPathVect.end(); imagePath++ )
	{
		int		superpixels 		= _superpixels;
		int 	numberOfBins 		= _numberOfBins;
		int		neighborhoodSize 	= _neighborhoodSize;
		float	minimumConfidence 	= _minimumConfidence;
		int 	iterations 			= _iterations;
		float 	spatialWeight 		= _spatialWeight;

		boost::filesystem::path csvFile(db.inputDir + "/" + *imageName + "_classes_.csv");
		boost::timer timer;
		double totalTime = 0;

		std::cout << "Frame ===> " << *imageName << std::endl;
		cv::Mat image = scene.readImages(imagePath->string());


		SEEDSRevisedMeanPixels seeds(image, superpixels, numberOfBins, neighborhoodSize, minimumConfidence, spatialWeight, SEEDSRevised::BGR);

		timer.restart();
		seeds.initialize();
		seeds.iterate(iterations);
		totalTime += timer.elapsed();

		int bgr[] = {0, 0, 204};

		cv::Mat imagecontour 	= Draw::contourImage(seeds.getLabels(), 	image , 	bgr);
		cv::Mat imageLabel 		= Draw::labelImage(seeds.getLabels(), 		image)			;
		cv::Mat imageMean 		= Draw::meanImage(seeds.getLabels(), 		image)			;

		cv::Mat imageClasses(image.rows,image.cols, CV_16UC1 );  // change it to int image better
		for ( int i=0; i< image.rows; i++)
		{
			for ( int j=0; j< image.cols; j++)
			{
				int** labels =seeds.getLabels();
				imageClasses.at< ushort >(i,j) =  (ushort)labels[i][j];
			}
		}

		cv::imwrite(	db.inputDir + "/" + *imageName + "_labels_.png",			imageLabel		);
		cv::imwrite(	db.inputDir + "/" + *imageName + "_classes_.png",			imageClasses	);
		cv::imwrite(	db.inputDir + "/" + *imageName + "_coutour_.png",			imagecontour	);


		Export::CSV(seeds.getLabels(), image.rows, image.cols, csvFile);

		imageName++;
	}
}



