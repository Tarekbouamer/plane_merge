
#ifndef SUPERPIXELS_H_
#define SUPERPIXELS_H_

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

class Superpixels {

private:
	int		_superpixels 		= 200;
	int 	_numberOfBins 		= 5;
	int		_neighborhoodSize 	= 1;
	float	_minimumConfidence 	= 0.1;
	int 	_iterations 		= 2;
	float 	_spatialWeight 		= 0.25;

public:

	Superpixels(Scene scene,
			Database db,
			std::vector< boost::filesystem::path > imagesPathVect);



};




#endif /* SUPERPIXELS_H_ */
