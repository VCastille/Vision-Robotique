#pragma once
#include "Image.h"
#include "ROI.h"

class Computing : public Image
{
public:
	Computing();
	~Computing();
	cv::Mat getThreshold();
	cv::Mat getRoiImg();
	void refresh(cv::Mat frame);
	ROI* getRoi();

private:	
	void threshold();
	ROI* _roi;
	cv::Mat thresholded;
	
};

