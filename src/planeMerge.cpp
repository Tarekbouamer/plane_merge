//============================================================================
// Name        : planeMerge.cpp
// Author      : Tarek Bouamer
// Version     :
// Copyright   : ICG TU GRAZ
// Description :
//============================================================================

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

#include <boost/filesystem.hpp>
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

#include "IO/paramsCam.h"
#include <fstream>

#include <iostream>
#include <string>

#include "database.h"
#include "scene.h"
#include "superpixels.h"



struct Superpixel{

	int id;
	int assignedClass;
	cv::Mat pos;
	cv::Vec3b label;
	unsigned int numPixels;
	std::vector<int> hist;   // nb classes + non-planes  (N+1)

	Superpixel(int ID, int row, int high, int P)
	{
		label= cv::Vec3b(0,0,0);
		assignedClass = -1;
		id =ID;
		pos = cv::Mat::zeros(row, high, CV_32SC1);
		numPixels=0;
		std::vector<int> arr(P+1,0);
		hist = arr;
	}
};


std::vector<cv::Mat> mergeSuper(
									const cv::Mat& classes,
									const cv::Mat& labels,
									std::vector<cv::Mat>& planes
									)
{
	const int numSuperpixels= *(std::max_element(classes.begin<ushort>(),classes.end<ushort>())) +1 ;
	std::vector<Superpixel*> listSuperpixels(numSuperpixels);

	std::cout << "	numSuperpixels  	" << numSuperpixels 	<< std::endl;
	std::cout << "	rows			  	" << classes.rows		<< std::endl;
	std::cout << "	cols			  	" << classes.cols 		<< std::endl;
	std::cout << "	num planes		  	" << planes.size() 		<< std::endl;


	// init list

	for (int i=0; i< numSuperpixels; i++)
		listSuperpixels[i] = new Superpixel(i, classes.rows, classes.cols, planes.size());

	for(int row=0; row< classes.rows; row++)
	{
		for(int col=0; col< classes.cols;col++)
		{
			listSuperpixels[classes.at<ushort>(row,col)]->numPixels++;
			listSuperpixels[classes.at<ushort>(row,col)]->label = labels.at<cv::Vec3b>(row,col);
			listSuperpixels[classes.at<ushort>(row,col)]->pos.at<ushort>(row,col)=255;
		}
	}

	std::vector<cv::Mat>::iterator plane = planes.begin();
	// 	// fill  histogram of plane

	int planeIdx = 0;
	for (; plane != planes.end(); ++plane  )
	{
		for(int row=0; row< plane->rows; row++)
		{
			for(int col=0; col< plane->cols;col++)
			{
				if(plane->at<float>(row,col)!=0)
				{
					listSuperpixels[classes.at<ushort>(row,col)]->hist[planeIdx]++;
				}
			}
		}

		planeIdx++;
	}
	// 	// fill  histogram of non planar

	for (int i=0; i< numSuperpixels; i++)
	{
		int sum = 0;
		for (std::vector<int>::iterator iter = listSuperpixels[i]->hist.begin();
				iter != listSuperpixels[i]->hist.end();
				++iter)
		{
			sum += *iter;
		}

		listSuperpixels[i]->hist[planes.size()]= listSuperpixels[i]->numPixels - sum;
	}

	std::vector<int> 		planeID(planes.size(),-1);
	std::vector<cv::Vec3b> 	planergb(planes.size(),(0,0,0));

	for (int i=0; i< numSuperpixels; i++)
	{
		int maxIDX = std::max_element(listSuperpixels[i]->hist.begin(),listSuperpixels[i]->hist.end()) - listSuperpixels[i]->hist.begin();
		double score = listSuperpixels[i]->hist[maxIDX]/ (double) listSuperpixels[i]->numPixels;

		if ( maxIDX< planes.size() && score > 0.70)
		{
			listSuperpixels[i]->assignedClass=maxIDX;
			if (planeID[maxIDX] == -1)
			{
				planeID[maxIDX]=listSuperpixels[i]->id;
				planergb[maxIDX]=listSuperpixels[i]->label;
			}
		}
		else
		{
			listSuperpixels[i]->assignedClass=planes.size();
		}
	}

	cv::Mat newClasses	= classes;
	cv::Mat newLabels	= labels;

	std::vector<cv::Mat> results;


	for(int row=0; row< newClasses.rows; row++)
	{
		for(int col=0; col< newClasses.cols;col++)
		{
			if (listSuperpixels[classes.at<ushort>(row,col)]->assignedClass < planes.size())
			{
				newClasses.at<ushort>(row,col)	= planeID[listSuperpixels[classes.at<ushort>(row,col)]->assignedClass];
				newLabels.at<cv::Vec3b>(row,col) 	= planergb[listSuperpixels[classes.at<ushort>(row,col)]->assignedClass];
			}

		}
	}
	results.push_back(newClasses);
	results.push_back(newLabels);



	for (int i=0; i< numSuperpixels; i++)
		delete listSuperpixels[i];


	return results;
}




void doSuperMerge(	Scene& scene,
					Database& db,

					std::vector< boost::filesystem::path >& imagesPathVect	,
					std::vector< boost::filesystem::path >& masksPathVect	,
					std::vector< boost::filesystem::path >& labelsPathVect	,
					std::vector< boost::filesystem::path >& classesPathVect

					)
{

	std::vector< boost::filesystem::path >::iterator mask		= masksPathVect.begin()		;
	std::vector< boost::filesystem::path >::iterator label	 	= labelsPathVect.begin()	;
	std::vector< boost::filesystem::path >::iterator classe 	= classesPathVect.begin()	;

	std::vector<std::string>::iterator imageName = db.nameList.begin();


	std::vector<cv::Mat> 	maskImage;
	cv::Mat					labelImage;
	cv::Mat					classeImage;

	std::vector<cv::Mat> results;


	for(; mask != masksPathVect.end(); ++mask)
	{

		labelImage		= scene.readImages(		label->string()						);
		classeImage		= scene.readCSV(		classe->string(), 	labelImage		);
		maskImage 		= scene.readNumpy(		mask->string(), 	labelImage		) ;


		results = mergeSuper(classeImage,labelImage,maskImage);


		cv::imwrite(db.outputDir + "/" + *imageName +"_labels_.png", results[1]);

		boost::filesystem::fstream csvFile;
		csvFile.open((db.outputDir + "/" + *imageName +".csv").c_str(), boost::filesystem::ofstream::out);

		assert(csvFile);

		for (int i = 0; i < results[0].rows; i++) {
			for (int j = 0; j < results[0].cols; j++) {
				csvFile << results[0].at<ushort>(i,j);

				if (j < results[0].cols - 1) {
					csvFile << ",";
				}
			}

			csvFile << "\n";
		}

		csvFile.close();

		results.clear();

		++label;
		++classe;
		++imageName;
	}

}



int main(int argc, char** argv){
	/*
	 * The module reads automatcilly from Input folder the inputs (1) plane masks (.npy) and superpixels (.csv) and create
	 * new plane segmentation (.csv) in Outputs folder
	 * Superpixel.h load RGB images (.JPG) and generate superpixels.
	 * Database.h reads   imageNamesList.txt and load the corresponding maps directories (RGB ,  Masks , superpixels , depth , normal, quality, ....)
	 * Scene.h load images as Mat format
    */

	Scene scene;
	Database db;
	ParamsCam paramsCam;

	std::vector< boost::filesystem::path > imagesPathVect;
	std::vector< boost::filesystem::path > masksPathVect;
	std::vector< boost::filesystem::path > labelsPathVect;
	std::vector< boost::filesystem::path > classesPathVect;

	db.inputDir = "../Inputs";
	db.outputDir = "../Outputs";

	db.readNameList();


	imagesPathVect 		= db.getPathList(0); 	// images only
	masksPathVect  		= db.getPathList(1); 	// masks only npy




	Superpixels(scene, db, imagesPathVect);    // superpixel   SEEED

	labelsPathVect	  	= db.getPathList(2); 	// labels (superpixel)  RGB  only
	classesPathVect  	= db.getPathList(3); 	// classes (superpixel) Grey only

	std::vector< cv::Mat > classesImages;


	std::cout << 	"paths loaded" 									<< std::endl;
	std::cout << 	"imagesPathVect" 	<< imagesPathVect.size()	<< std::endl;
	std::cout << 	"masksPathVect" 	<< masksPathVect.size()		<< std::endl;
	std::cout << 	"labelsPathVect" 	<< labelsPathVect.size()	<< std::endl;
	std::cout << 	"classesPathVect" 	<< classesPathVect.size()	<< std::endl;





	doSuperMerge(scene, db, imagesPathVect,masksPathVect, labelsPathVect, classesPathVect);




	std::cout << " Done .... " << std::endl;

	return 0;

}

