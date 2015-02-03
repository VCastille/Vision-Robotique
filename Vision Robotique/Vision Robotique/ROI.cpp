#include "ROI.h"


ROI::ROI(cv::Mat frame)
{
	//extraction du ROI via l'image de la camera
	extractROI(frame);
}


ROI::~ROI()
{
}

void ROI::refresh(cv::Mat frame)
{
	extractROI(frame);
	median();
	std::cout << "DEBUG: getmedian = " << getmedian().val[0] << " : " << getmedian().val[1] << " : " << getmedian().val[2] << std::endl;

}

void ROI::extractROI(cv::Mat m)
{
	//cv::Mat m = frame;
	int c = m.cols;
	int l = m.rows;
	int rect_largeur = (c - (c * 20 / 100));
	int rect_hauteur = (l - (l * 80 / 100));
	int x_rect = c - rect_largeur; //largeur rectangle
	int y_rect = l - rect_hauteur; //hauteur rectangle

	cv::Mat roi = m(cv::Rect(x_rect, y_rect, rect_largeur, rect_hauteur));
	img = roi;
}
