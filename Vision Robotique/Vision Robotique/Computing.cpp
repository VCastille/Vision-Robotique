#include "Computing.h"

using namespace cv;

Computing::Computing()
{
	_roi = new ROI(img);
}


Computing::~Computing()
{
}

Mat Computing::getThreshold()
{
	threshold();
	return thresholded;
}

cv::Mat Computing::getRoi()
{
	return _roi->getimg();
}

void Computing::threshold()
{
	Mat t;
	img.copyTo(t);

	Scalar floor = _roi->getmedian();


	int LowHue = floor.val[0] - 20;
	int HighHue = floor.val[0] + 20;

	int LowSat;
	int HighSat;

	int LowVal;
	int HighVal;

	if (floor.val[1] < 20)
		LowSat = 0;
	else
		LowSat = 10;

	if (floor.val[2] >= 225)
		HighSat = 255;
	else
		HighSat = 200;

	if (floor.val[2] < 20)
	{
		LowVal = 0;
		HighVal = 35;
		//si la couleur est noire : on accepte toutes les teintes.
		LowHue = 0;
		HighHue = 255;

	}
	else if (floor.val[2] > 220)
	{
		LowVal = 220;
		HighVal = 255;
		//si la couleur est blanche : on accepte toutes les teintes.
		LowHue = 0;
		HighHue = 255;

	}
	else
	{
		LowVal = 25;
		HighVal = 225;
	}


	if (LowHue < 0)
		LowHue = 0;
	if (LowSat < 0)
		LowSat = 0;
	if (LowVal < 0)
		LowVal = 0;

	std::cout << "Zone ROI: h = " << floor.val[0] << " s = " << floor.val[1] << " v = " << floor.val[2] << std::endl;
	std::cout << "test valeurs: " << LowHue << " " << LowSat << " " << LowVal << std::endl;

	inRange(img, Scalar(LowHue, LowSat, LowVal), Scalar(HighHue, HighSat, HighVal), t);

	//inRange(hsv, Scalar(10, 10, 10), Scalar(180, 255, 30), t);
	thresholded = t;
}

void Computing::refresh(cv::Mat frame)
{
	img = frame;
	
}