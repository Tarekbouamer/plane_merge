#ifndef SIMPLE_MERGE_H_
#define SIMPLE_MERGE_H_

#include <fstream>
#include <iostream>
#include <string>

#include "database.h"
#include "superpixels.h"

#include "plane_estimator.h"

#include "random.h"

#include "misc.h"

typedef std::vector<Superpixel*> superpixels_t;


struct SimpleMergeOptions
{
	
};


class SimpleMerge {
public:

	std::vector<cv::Mat > image;
	std::vector<cv::Mat > maskVector;

	SimpleMerge() = default;
	explicit SimpleMerge(const std::string&);


	void ComputeUncertaintyMap(cv::Mat& DepthMap, cv::Mat& Quality, cv::Mat& UncertaintyMap);


	void LoadSuperpixelList(Image& item, superpixels_t& list_supepixels);


	void AssignNeighbouring(const cv::Mat& M, superpixels_t& list_supepixels);


	void EstimatePlaneParameters(superpixels_t& list_supepixels);


	void SemanticLabel(Image& item, cv::Mat& cls, superpixels_t& list_supepixels);


	superpixels_t HasPlanes(superpixels_t& list_supepixels);


	superpixels_t GetPlanarCandidates(superpixels_t& list_supepixels);


	void PlaneLabel(Image& item, cv::Mat& cls, superpixels_t& list_supepixels);


	void WriteNewLabels(Image& item,  cv::Mat& cls, cv::Mat& lbl, superpixels_t& list_supepixels);


	std::vector<cv::Vec3b> GenerateRandomColors();

	void Run();

private:
	Database _database;
	SimpleMergeOptions options;

};


#endif // SIMPLE_MERGE_H_
