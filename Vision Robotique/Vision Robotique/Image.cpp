#include "Image.h"

using namespace cv;

Image::Image()
{
}


Image::~Image()
{
}


void Image::median()
{
	double m = (img.rows*img.cols) / 2;

	int bin0 = 0, bin1 = 0, bin2 = 0;

	cv::Scalar med;
	med.val[0] = -1;
	med.val[1] = -1;
	med.val[2] = -1;

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	cv::Mat hist0, hist1, hist2;
	std::vector<cv::Mat> channels;
	split(img, channels);

	calcHist(&channels[0], 1, 0, cv::Mat(), hist0, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&channels[1], 1, 0, cv::Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&channels[2], 1, 0, cv::Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);

	for (int i = 0; i<256 && (med.val[0]<0 || med.val[1]<0 || med.val[2]<0); i++)
	{
		bin0 = bin0 + cvRound(hist0.at<float>(i));
		bin1 = bin1 + cvRound(hist1.at<float>(i));
		bin2 = bin2 + cvRound(hist2.at<float>(i));

		if (bin0>m && med.val[0]<0)
			med.val[0] = i;
		if (bin1>m && med.val[1]<0)
			med.val[1] = i;
		if (bin2>m && med.val[2]<0)
			med.val[2] = i;
	}
	
	_med = med;
}

Mat Image::getimg()
{
	return img;
}

Mat Image::customHSV()
{
	//Fonction qui converti l'image en HSV puis la filtre via des valeurs spécifiques.
	Mat hsv;
	img.copyTo(hsv);
	cvtColor(hsv, hsv, CV_BGR2HSV);
	int x, y;
	Vec3b pxl;
	uchar h, s, v;
	std::cout << "DEBUG : hsv.cols = " << hsv.cols << " hsv.rows = " << hsv.rows << std::endl;
	//parcours de l'image
	for (x = 0; x < hsv.rows; x++)
		for (y = 0; y < hsv.cols; y++)
		{
		pxl = hsv.at<Vec3b>(x, y);					//recupération de la valeur du pixel
		h = pxl.val[0];
		s = pxl.val[1];
		v = pxl.val[2];

		//Si l'image est trop sombre, teinte et saturation non considérables
		if (v < 40)
		{
			h = 0;
			s = 0;
		}
		//Si saturation trop faible, teinte non considérable.
		if (s < 40)
			h = 0;

		Vec3b cleared = { h, s, v };
		//std::cout << "DEBUG: h= " << (int)cleared.val[0] << " s= " << (int)cleared.val[1] << " v= " << (int)cleared.val[2] << std::endl;
		hsv.at<Vec3b>(x, y) = cleared;
		}
	//std::cout << "return ce shit" << std::endl;
	return hsv;
}

void Image::refresh(Mat frame)
{
	img = frame;
}

Scalar Image::getmedian()
{
	return _med;
}