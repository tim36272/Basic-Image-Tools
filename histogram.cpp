#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include "opencv2/opencv.hpp"


int main(int argc, char* argv[]) {
	cv::Mat image;
	std::string image_name;
	switch(argc) {
	case 1: //no commands were given
		std::cout<<"Enter image directory from /build: ";
		std::cin>>image_name;
		image = cv::imread(image_name,CV_LOAD_IMAGE_GRAYSCALE);
		break;
	case 2: //image given
		image = cv::imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
		break;
	default: //something different
		std::cout<<"Usage: ./histogram image.type"<<std::endl;
		return 0;
		break;
	}

	if(image.empty()) {
		std::cout<<"Image was invalid (did you forget the ../ ?)"<<std::endl;
		return 0;
	}
	if(image.cols*image.rows > 65536) {
		std::cout<<"WARNING: histogram overflow may occur"<<std::endl;
	}

	std::vector<cv::Mat> planes;
	cv::split(image,planes);

	//build a histogram
	int bins = 256;
	cv::Mat histogram(cv::Size(1,bins),CV_16UC1,cv::Scalar(0));
	//for each element in the input, add to a bin in the histogram
	//this can be done faster with direct data access
	for(int y=0;y<image.rows;y++) {
		for(int x=0;x<image.cols;x++) {
			int this_color = planes[0].at<uchar >(y,x);
			histogram.at<int16_t>(this_color) = histogram.at<int16_t>(this_color) +1;
		}
	}
	double max;
	double min;
	double dummy;
	cv::Point dummy_p;
	cv::minMaxLoc(histogram,&dummy,&max,&dummy_p,&dummy_p);
	//show originalhistogram
	cv::Mat original_visible_histogram(bins,bins,CV_8UC1,cv::Scalar(0));
	for(int i=0;i<bins;i++) {
		cv::rectangle(original_visible_histogram,cv::Point(i,bins-1),cv::Point(i,(bins-1-histogram.at<int16_t>(i)*255/max)),cv::Scalar(128),-1);
	}
	imshow("original histogram",original_visible_histogram);
	//compute cdf
	cv::Mat_<int> cdf(1,bins,0);
	cdf.at<int>(0) = histogram.at<int16_t>(0);
	for(int i=1;i<bins;i++) {
		cdf.at<int>(i) = cdf.at<int>(i-1)+histogram.at<int16_t>(i);
	}

	//get histogram min


	cv::minMaxLoc(histogram,&min,&dummy,&dummy_p,&dummy_p,cv::Mat());
std::cout<<"Min: "<<min<<std::endl;
	//equalize the historgram
	for(int i=0;i<bins;i++) {
		histogram.at<int16_t>(i) = (cdf.at<int>(i)-min)/(image.rows*image.cols-min)*255+0.5;
	}

	//modify the image
	for(int y=0;y<image.rows;y++) {
		for(int x=0;x<image.cols;x++) {
			uchar input_val = planes[0].at<uchar>(y,x);
			planes[0].at<uchar>(y,x) = histogram.at<int16_t>(input_val);
		}
	}

	std::cout<<"Histogram: "<<histogram<<std::endl;
	std::cout<<"cdf: "<<cdf<<std::endl;
	//show new histogram
	cv::Mat new_histogram(cv::Size(1,bins),CV_16UC1,cv::Scalar(0));
	//for each element in the input, add to a bin in the histogram
	//this can be done faster with direct data access
	for(int y=0;y<image.rows;y++) {
		for(int x=0;x<image.cols;x++) {
			int this_color = planes[0].at<uchar >(y,x);
			new_histogram.at<int16_t>(this_color) = new_histogram.at<int16_t>(this_color) +1;
		}
	}
	cv::minMaxLoc(new_histogram,&dummy,&max,&dummy_p,&dummy_p);
	cv::Mat visible_histogram(bins,bins,CV_8UC1,cv::Scalar(0));
	for(int i=0;i<bins;i++) {
		cv::rectangle(visible_histogram,cv::Point(i,bins-1),cv::Point(i,(bins-new_histogram.at<int16_t>(i)*255./max)),cv::Scalar(128),-1);
	}
	imshow("histogram",visible_histogram);
	imshow("result",planes[0]);
	while(cv::waitKey(0)!='q');
/*
 *  for generating the histogram with OpenCV
 */
	/*
	int channels[] = {0};
	int histSize[] = {256};
    float franges[] = { 0, 256 };
    const float* ranges[] = { franges };
    cv::Mat cv_histogram;
	cv::calcHist(&(planes[0]),1,channels,cv::Mat(),cv_histogram,1,histSize,ranges);
	std::cout<<std::endl<<"CV histam: "<<cv_histogram<<std::endl;
	*/
	return 0;
}


