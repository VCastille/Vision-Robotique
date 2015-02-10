#include "Computing.h"
#include "ROI.h"
#include <vector>

using namespace cv;
using namespace std;

Computing::Computing()
{
	_roi = new ROI(img);
}


Computing::~Computing()
{
}

Mat Computing::getThreshold()
{
	return thresholded;
}

cv::Mat Computing::getRoiImg()
{
	return _roi->getimg();
}

ROI* Computing::getRoi()
{
	return _roi;
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
	bitwise_not(t, t);
	//inRange(hsv, Scalar(10, 10, 10), Scalar(180, 255, 30), t);
	thresholded = t;
}

void Computing::fillPath()
{
	for (int x = 0; x < thresholded.rows; x++)
		for (int y = 0; y < thresholded.cols; y++)
		{
		if (thresholded.at<Vec3b>(y, x).val[0] == 255)
			//cout << "c'est la mer noire" << endl;
			path[x][y] = 1;
		else if (thresholded.at<Vec3b>(y, x).val[0] == 0)
			//cout << "c'est la magie de l'homme blanc" << endl;
			path[x][y] = 0;
		else
			//cout << "bugged as hell" << endl;
			path[x][y] = -1;
		}
		
}


void Computing::customthreshold()
{
	//Fonction qui seuille l'image en prenant en compte des paramètres spéciaux sur le format HSV.
	Mat tree;
	img.copyTo(tree);
	//cvtColor(tree, tree, CV_HSV2BGR);
	int x, y;
	Vec3b pxl;
	uchar h, s, v;
	//std::cout << "DEBUG : hsv.cols = " << .cols << " hsv.rows = " << hsv.rows << std::endl;
	
	//parcours de l'image
	for (x = 0; x < tree.rows; x++)
		for (y = 0; y < tree.cols; y++)
		{
		pxl = tree.at<Vec3b>(x, y);					//recupération de la valeur du pixel
		h = pxl.val[0];
		s = pxl.val[1];
		v = pxl.val[2];

		
		//Si l'image est trop sombre, teinte et saturation non considérables
		//Donc si le pixel est sombre et que la couleur recherchée ne l'est pas: obstacle.
		if (v < 30 && _med.val[2] > 40)
		{
			tree.at<Vec3b>(x, y).val[0] = 0;
			tree.at<Vec3b>(x, y).val[1] = 0;
			tree.at<Vec3b>(x, y).val[2] = 0;
			pxl.val[0] = 0;
			pxl.val[1] = 0;
			pxl.val[2] = 0;
		}
		//Si saturation trop faible, teinte non considérable.
		else if (s < 30 && (pxl.val[2] - 15 <= _med.val[2] && pxl.val[2] + 15 >= _med.val[2]))
		{
			pxl.val[0] = 0;
			pxl.val[1] = 0;
			pxl.val[2] = 0;
		}
		else if ((pxl.val[0] - 15 <= _med.val[0] && pxl.val[0] + 15 >= _med.val[0]) && (pxl.val[2] - 15 <= _med.val[2] && pxl.val[2] + 15 >= _med.val[2]))
		{
			pxl.val[0] = 0;
			pxl.val[1] = 0;
			pxl.val[2] = 0;
		}
		else
		{
			pxl.val[0] = 255;
			pxl.val[1] = 255;
			pxl.val[2] = 255;
		}
		
		Vec3b cleared = { h, s, v };
		//std::cout << "DEBUG: h= " << (int)cleared.val[0] << " s= " << (int)cleared.val[1] << " v= " << (int)cleared.val[2] << std::endl;
		tree.at<Vec3b>(x, y) = cleared;
		
		}
	//std::cout << "return ce shit" << std::endl;
	thresholded = tree;
}


void Computing::refresh(cv::Mat frame)
{
	//redimensionnement du vecteur
	if (path.size() == 0)
	{
		path.resize(frame.rows);
		for (auto& vec : path)
			vec.resize(frame.cols);
		cout << "DEBUG: resize: " << frame.cols << " x " << frame.rows << endl;
	}

	frame.copyTo(img);

	//cvtColor(img, img, CV_BGR2HSV);
	img = customHSV();

	_roi->refresh(img);
	
	//customthreshold();
	threshold();
	
	//fillPath();
		
}