#pragma once
#include "Image.h"
#include "ROI.h"
#include <vector>

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
	void fillPath();
	void customthreshold();

	ROI* _roi;

	cv::Mat thresholded;
	cv::Mat _hsv;

	std::vector < std::vector<int> > path;
	
};

