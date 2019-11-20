
#include "paramsCam.h"

bool
ParamsCam::readCamConfig(const std::string& filename){
	std::cout << "Going to read calibration in file: " << filename << "..." << std::endl;
	std::ifstream inStream(filename.c_str());
	std::string buffer;
	std::string tester;
	if(!inStream.good()){
		std::cout << "error in readCamConfig: file " << filename << " not found" << std::endl;
		return false;
	}
	std::string tempID = filename.substr(filename.find_last_of("/\\") + 1);
	std::stringstream stm(tempID.substr(0,tempID.size()-10));
	stm >> id;
	//std::cout << id << std::endl;
	bool setCamA = false;
	bool setCamR = false;
	bool setCamT = false;
	while(inStream.good()){
		getline(inStream,buffer,'\n');
		std::istringstream raut(buffer.substr(0,1));
		tester = "#";
		if(raut.str().compare(tester)==0){
			continue;
		}
		else{
			int mode = 0;
			int tempNr=buffer.find("=",1);
			std::stringstream stm(buffer.substr(0,tempNr));
			std::string tester1="camera.T";
			std::string tester2="camera.R";
			std::string tester3="camera.A";
			if(stm.str().compare(tester1)==0) mode=1;
			else if(stm.str().compare(tester2)==0) 	mode=2;
			else if(stm.str().compare(tester3)==0) 	mode=3;
			if(mode==1){
				tempNr=buffer.find("[",1)+1;
				for(int i=0; i<3; i++){
					std::string searchChar = " ";
					if(i==2) searchChar = "]";
					int tempNr2=buffer.find(searchChar,tempNr);
					std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr));
					stm1 >> cameraT[i];
					tempNr=tempNr2+1;
				}
				setCamT=true;
			}
			if(mode==2){
				int i=0;
				int j=0;
				unsigned int endNr=buffer.find("]",tempNr)+1;
				tempNr=buffer.find("[",tempNr)+1;
				while(buffer.find(" ",tempNr)<endNr&&buffer.find(" ",tempNr)!=std::string::npos){
					while(buffer.find(" ",tempNr)<buffer.find(";",tempNr)){
						int tempNr2=buffer.find(" ",tempNr);
						std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr));
						stm1 >> cameraR(i,j++);
						tempNr=tempNr2+1;
					}
					unsigned int tempNr2=buffer.find(" ",tempNr);
					if(tempNr2==std::string::npos||tempNr2>endNr){
						break;
					}
					std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr-1));
					stm1 >> cameraR(i++,j);
					tempNr=tempNr2+1;
					j=0;
				}
				std::stringstream stm1(buffer.substr(tempNr,endNr-tempNr-1));
				stm1 >> cameraR(i,j);
				setCamR=true;
			}
			if(mode==3){
				int i=0;
				int j=0;
				unsigned int endNr=buffer.find("]",tempNr)+1;
				tempNr=buffer.find("[",tempNr)+1;
				while(buffer.find(" ",tempNr)<endNr&&buffer.find(" ",tempNr)!=std::string::npos){
					while(buffer.find(" ",tempNr)<buffer.find(";",tempNr)){
						int tempNr2=buffer.find(" ",tempNr);
						std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr));
						stm1 >> cameraA(i,j++);
						tempNr=tempNr2+1;
					}
					unsigned int tempNr2=buffer.find(" ",tempNr);
					if(tempNr2==std::string::npos||tempNr2>endNr){
						break;
					}
					std::stringstream stm1(buffer.substr(tempNr,tempNr2-tempNr-1));
					stm1 >> cameraA(i++,j);
					tempNr=tempNr2+1;
					j=0;
				}
				std::stringstream stm1(buffer.substr(tempNr,endNr-tempNr-1));
				stm1 >> cameraA(i,j);
				setCamA=true;
			}
		}
	}
	if(!setCamT){
		std::cout << "error in readCamConfig: could not read 'camera.T' from '" <<
		filename << "'." << std::endl;
		return false;
	}
	if(!setCamR){
		std::cout << "error in readCamConfig: could not read 'camera.R' from '" <<
		filename << "'." << std::endl;
		return false;
	}
	if(!setCamA){
		std::cout << "error in readCamConfig: could not read 'camera.A' from '" <<
		filename << "'." << std::endl;
		return false;
	}
	cameraAInv=cameraA.inverse();
	return true;
}

void
ParamsCam::getCamConfig(Eigen::Vector3d& _cameraT, Eigen::Matrix3d& _cameraR,
		Eigen::Matrix3d& _cameraA, Eigen::Matrix3d& _cameraAInv, int& _id){
	_cameraT=cameraT;
	_cameraR=cameraR;
	_cameraA=cameraA;
	_cameraAInv=cameraAInv;
	_id=id;
}

Eigen::Vector3d
ParamsCam::getCamT(){
	return cameraT;
}
