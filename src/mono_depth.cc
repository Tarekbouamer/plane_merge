#include "mono_depth.h"

#include <iostream>
#include <fstream>
#include "misc.h"
#include <eigen3/Eigen/Geometry>


MonoDepth::MonoDepth(const std::string& mono_depth_path): 
			_mono_depth_path(mono_depth_path) {};
		

cv::Mat MonoDepth::Read(const std::string& mono_depth_path){

	std::cout << "Going To Read File " << mono_depth_path  << std::endl;

  //Open binary file
  std::ifstream pfmFile(mono_depth_path.c_str(),  std::ios::in | std::ios::binary);

  cv::Mat imagePFM;

  //If file correctly openened
  if(pfmFile)
  {
    // Read the type of file plus the 0x0a UNIX return character at the end
    char type[3];
    pfmFile.read(type, 3*sizeof(char));

    // Read the width and height
    unsigned int width(0), height(0);
    pfmFile >> width >> height;

    // Read the 0x0a UNIX return character at the end
    char endOfLine;
    pfmFile.read(&endOfLine, sizeof(char));

    int numberOfComponents(0);
    // The type gets the number of color channels
    if(type[1] == 'F')
    {
      imagePFM = cv::Mat(height, width, CV_32FC3);
      numberOfComponents = 3;
    }
    else if(type[1] == 'f')
    {
      imagePFM = cv::Mat(height, width, CV_32FC1);
      numberOfComponents = 1;
    }

    //TODO Read correctly depending on the endianness
    //Read the endianness plus the 0x0a UNIX return character at the end
    //Byte Order contains -1.0 or 1.0
    
		char byteOrder[4];
    pfmFile.read(byteOrder, 4*sizeof(char));

    //Find the last line return 0x0a before the pixels of the image
    char findReturn = ' ';
    while(findReturn != 0x0a)
    {
      pfmFile.read(&findReturn, sizeof(char));
    }

    //Read each RGB colors as 3 floats and store it in the image.
    float *color = new float[numberOfComponents];
    
		for(unsigned int i = 0 ; i<height ; ++i)
    {
      for(unsigned int j = 0 ; j<width ; ++j)
      {
        pfmFile.read((char*) color, numberOfComponents*sizeof(float));

        //In the PFM format the image is upside down
        if(numberOfComponents == 3)
        {
          //OpenCV stores the color as BGR
          imagePFM.at<cv::Vec3f>(height-1-i,j) = cv::Vec3f(color[2], color[1], color[0]);
        }
        else if(numberOfComponents == 1)
        {
          //OpenCV stores the color as BGR
          imagePFM.at<float>(height-1-i,j) = color[0];
        }
      }
    }

    delete[] color;

    //Close file
    pfmFile.close();
  }
  else
  {
    std::cerr << "Could not open the file : " << mono_depth_path << std::endl;
	}

	// assign
	_width = imagePFM.cols;
	_height= imagePFM.rows;

  return imagePFM;
}


cv::Mat MonoDepth::Read(){
  return Read(_mono_depth_path);
}


cv::Mat MonoDepth::Read(const cv::Size target_size){

	cv::Mat M = Read(_mono_depth_path);
	
	Rescale(M , target_size); 

  return M;
}


void MonoDepth::Write(const std::string& _path, cv::Mat& M){

	std::logic_error("Function not yet implemented");

}

size_t MonoDepth::GetWidth() const { 	return _width; 	}
size_t MonoDepth::GetHeight() const { 	return _height; }


const std::string& MonoDepth::GetMonoDepthPath() const  { return _mono_depth_path;  }


void MonoDepth::Rescale(cv::Mat& M, const float factor) { Rescale(M, cv::Size(factor, factor)); }


void MonoDepth::Rescale(cv::Mat& M, const cv::Size target_size) {
	
	// resize map 
	cv::resize(M, M, target_size);

	// new dims

  _width = M.cols;
  _height = M.rows;
}