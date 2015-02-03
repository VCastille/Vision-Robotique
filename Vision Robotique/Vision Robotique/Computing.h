#pragma once
#include "Image.h"
#include "ROI.h"

class Computing : public Image
{
public:
	Computing();
	~Computing();
	cv::Mat getThreshold();
	cv::Mat getRoi();
	void refresh(cv::Mat frame);

private:	
	void threshold();
	ROI* _roi;
	cv::Mat thresholded;
	
};

