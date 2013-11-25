#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(int argc, char* argv[]) {
	cv::Mat image;
	int depth;
	std::string image_name;
	switch(argc) {
	case 1: //no commands were given
		std::cout<<"Enter image directory from /build: ";
		std::cin>>image_name;
		image = cv::imread(image_name,CV_LOAD_IMAGE_GRAYSCALE);
		std::cout<<"Enter image depth: ";
		std::cin>>depth;
		break;
	case 2: //image given

		image = cv::imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
		std::cout<<"Enter image depth: ";
		std::cin>>depth;
		break;
	case 3: //all given
		image = cv::imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
		std::stringstream(argv[2]) >> depth;
		break;
	default: //something different
		std::cout<<"Usage: ./quantize image.type depth"<<std::endl;
		return 0;
		break;
	}

	if(image.empty()) {
		std::cout<<"Image was invalid (did you forget the ../ ?)"<<std::endl;
		return 0;
	}
	else if(depth<1 || depth>255) {
		std::cout<<"Depth was invalid"<<std::endl;
		return 0;
	}
	//this could be done with simple cv::Mat arithmetic, but I'll do it
	//for each pixel for the fun of it
	int scale_factor = 256/depth;
	for(int y=0;y<image.rows;y++) {
		for(int x=0;x<image.cols;x++) {
			//integer division does the magic here
			image.at<uchar>(y,x) = int(image.at<uchar>(y,x))/scale_factor;
			image.at<uchar>(y,x) *= scale_factor;
		}
	}
	imshow("result",image);
	while(cv::waitKey(0)!='q');

	return 0;
}


