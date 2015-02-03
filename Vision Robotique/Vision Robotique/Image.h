#pragma once
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class Image
{
public:
	Image();
	~Image();

	void refresh(cv::Mat frame);
	void median();
	cv::Mat getimg();
	cv::Scalar getmedian();

protected:
	cv::Mat img;
	cv::Mat customHSV();
	cv::Scalar _med;

private:
	
};

