#include "superpixels.h"
#include "misc.h"

// image

SuperpixelsSegementation::SuperpixelsSegementation(const std::string& superpixels_path_cls, const std::string& superpixels_path_lbl): 
      _superpixels_path_cls(superpixels_path_cls),
      _superpixels_path_lbl(superpixels_path_lbl) 
      {};


cv::Mat SuperpixelsSegementation::ReadLabels(const std::string& path)
{
	unsigned int flag = 1;
	return cv::imread(path, flag);
};

cv::Mat SuperpixelsSegementation::ReadLabels()
{
	return ReadLabels(_superpixels_path_lbl);
};

cv::Mat SuperpixelsSegementation::ReadCSV(const std::string& path, const cv::Mat label)
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

cv::Mat SuperpixelsSegementation::ReadCSV()
{
  cv::Mat label = ReadLabels();

  return ReadCSV(_superpixels_path_cls, label);
}


// Generator 

SuperpixelsGenerator::SuperpixelsGenerator(const std::string& database_path ){
  database_ = Database(database_path);
}

void SuperpixelsGenerator::Run(){
  
  // create directory for superpixels in workspace
  std::string ws_folder = JoinPaths(database_.workspace, database_.superpixels_folder);
  CreateDirIfNotExists(ws_folder);

  // read all images
  std::vector<Image> images = database_.ReadAllImages();

  // generate superpixels all images

  for (auto& it : images){
    
    std::string img_name = GetPathBaseName(it.GetImagePath());

    cv::Mat img = it.Read();
  
		SEEDSRevisedMeanPixels seeds(img, options.NumSuperpixels,     options.NumberOfBins,   options.NeighborhoodSize, 
                                      options.MinimumConfidence,  options.SpatialWeight,  SEEDSRevised::BGR);

		seeds.initialize();
		seeds.iterate(options.Iterations);

		int bgr[] = {0, 0, 204};

		cv::Mat labels = Draw::labelImage(seeds.getLabels(), img);

    // write to workspace
    std::string csv_file = JoinPaths(ws_folder, img_name + ".csv");
    std::string lbl_file = JoinPaths(ws_folder, img_name + ".png");

		cv::imwrite(lbl_file, labels);
    
    Export::CSV(seeds.getLabels(), img.rows, img.cols, csv_file);

  }

}