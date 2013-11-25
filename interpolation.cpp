#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(int argc, char* argv[]) {
	cv::Mat image;
	int scale;
	std::string image_name;
	switch(argc) {
	case 1: //no commands were given
		std::cout<<"Enter image directory from /build: ";
		std::cin>>image_name;
		image = cv::imread(image_name,CV_LOAD_IMAGE_GRAYSCALE);
		std::cout<<"Enter image scale: ";
		std::cin>>scale;
		break;
	case 2: //image given

		image = cv::imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
		std::cout<<"Enter image scale: ";
		std::cin>>scale;
		break;
	case 3: //all given
		image = cv::imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
		std::stringstream(argv[2]) >> scale;
		break;
	default: //something different
		std::cout<<"Usage: ./quantize image.type scale"<<std::endl;
		return 0;
		break;
	}

	if(image.empty()) {
		std::cout<<"Image was invalid (did you forget the ../ ?)"<<std::endl;
		return 0;
	}

	//subsample the image (I'll use OpenCV since a method isn't specified)
	cv::Mat small_image;
	cv::Size small_image_size(image.size().height/scale,image.size().width/scale);
	cv::resize(image,small_image,small_image_size);
	imshow("original",image);

	//resample back up using nearest neighbor
	cv::Mat image_nearest(image.size(),CV_8UC1,cv::Scalar(0));
	for(int y=0;y<image_nearest.size().height;y++) {
		for(int x=0;x<image_nearest.size().width;x++) {
			image_nearest.at<uchar>(y,x) = small_image.at<uchar>(y/scale,x/scale);
		}
	}
	imshow("nearest",image_nearest);
	//compute nearest neighbor error
	double error=0;
	for(int y=0;y<image_nearest.size().height;y++) {
		for(int x=0;x<image_nearest.size().width;x++) {
			error+=pow(image.at<uchar>(y,x)-image_nearest.at<uchar>(y,x),2);
		}
	}
	error /= 256.*256.;
	std::cout<<"Nearest neighbor error: "<<error<<std::endl;

	//resample back up using averaging
	cv::Mat image_averaging(image.size(),CV_8UC1,cv::Scalar(0));
	for(int y=0;y<image_averaging.size().height;y++) {
		for(int x=0;x<image_averaging.size().width;x++) {
			image_averaging.at<uchar>(y,x) = small_image.at<uchar>(y/scale,x/scale)/4. +
											 small_image.at<uchar>(y/scale,(x+1)/scale)/4. +
											 small_image.at<uchar>((y+1)/scale,x/scale)/4. +
											 small_image.at<uchar>((y+1)/scale,(x+1)/scale)/4.;
		}
	}
	//compute averaging error
	error=0;
	for(int y=0;y<image_averaging.size().height;y++) {
		for(int x=0;x<image_averaging.size().width;x++) {
			error+=pow(image.at<uchar>(y,x)-image_averaging.at<uchar>(y,x),2);
		}
	}
	error /= 256.*256.;
	std::cout<<"Averaging error: "<<error<<std::endl;


	//resample back up using bilinear

	cv::Mat image_bilinear(image.size(),CV_8UC1,cv::Scalar(0));
	for(int y=0;y<image_bilinear.size().height;y++) {
		for(int x=0;x<image_bilinear.size().width;x++) {
			int x1 = x/scale;
			int x2 = (x)/scale+1;
			int y1 = y/scale;
			int y2 = (y)/scale+1;
			double x_diff = (double(x)/double(scale) - x1);
			double y_diff = (double(y)/double(scale) - y1);

			if(!(y2<image.size().height)) {
				y2--;
			}
			if(!(x2<image.size().width)) {
				x2--;
			}

			cv::Point q11(x1,y1);
			cv::Point q12(x1,y2);
			cv::Point q21(x2,y1);
			cv::Point q22(x2,y2);

			uchar fq11 = small_image.at<uchar>(q11);
			uchar fq21 = small_image.at<uchar>(q21);
			uchar fq12 = small_image.at<uchar>(q12);
			uchar fq22 = small_image.at<uchar>(q22);
			image_bilinear.at<uchar>(y,x) = (fq11*(1-x_diff)*(1-y_diff)+
											fq21*(x_diff)*(1-y_diff)+
											fq12*(y_diff)*(1-x_diff)+
											fq22*(x_diff)*(y_diff));
		}
	}
	//compute bilinear error
	error=0;
	for(int y=0;y<image_bilinear.size().height-3;y++) {
		for(int x=0;x<image_bilinear.size().width-3;x++) {
			error+=pow(image.at<uchar>(y,x)-image_bilinear.at<uchar>(y,x),2);
		}
	}
	error /= 256.*256.;
	std::cout<<"bilinear error: "<<error<<std::endl;
	imshow("nearest",image_nearest);
	imshow("averaging",image_averaging);
	imshow("bilinear",image_bilinear);
	while(cv::waitKey(0)!='q');

	return 0;
}


