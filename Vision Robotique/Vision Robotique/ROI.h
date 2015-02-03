#pragma once
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Image.h"

class ROI : public Image
{
public:
	ROI(cv::Mat frame);
	~ROI();
	void refresh(cv::Mat frame);

private:
	void extractROI(cv::Mat frame);
};

