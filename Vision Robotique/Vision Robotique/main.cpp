#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Computing.h"
#include "Image.h"
#include "ROI.h"

//using namespace cv;

/*
void disclaimer()
{
	std::cout << "*************************************************************" << std::endl;
	std::cout << "*********** Groupe Castille | Laurent | Douillet ************" << std::endl;
	std::cout << "*********** Tests de vision robotique via OPENCV ************" << std::endl;
	std::cout << "***********          V0.5 - 27/01/2015           ************" << std::endl;
	std::cout << "*************************************************************" << std::endl << std::endl << std::endl;
}

int showMenu()
{
	int choix;
	std::cout << "*| Liste des differents programmes de test:" << std::endl;
	std::cout << "*| 1 - Recuperation flux raspberry" << std::endl;
	std::cout << "*| 2 - Reconaissance de formes" << std::endl;
	std::cout << "*| 0 - Quitter le programme" << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << "Veillez choisir le programme a executer:" << std::endl;
	std::cin >> choix;

	return choix;
}

Mat extractROI(Mat m)
{
	int c = m.cols;
	int l = m.rows;
	int rect_largeur = (c - (c * 20 / 100));
	int rect_hauteur = (l - (l * 80 / 100));
	int x_rect = c - rect_largeur; //largeur rectangle
	int y_rect = l - rect_hauteur; //hauteur rectangle

	//std::cout << "DEBUG! l= " << l << " c = " << c << " x_rect : " << x_rect << " y_rect: " << y_rect << std::endl;
	Mat roi = m(Rect(x_rect, y_rect, rect_largeur, rect_hauteur));

	return roi;
}

Scalar median(Mat image)
{
	double m = (image.rows*image.cols) / 2;

	int bin0 = 0, bin1 = 0, bin2 = 0;

	Scalar med;
	med.val[0] = -1;
	med.val[1] = -1;
	med.val[2] = -1;

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	Mat hist0, hist1, hist2;
	std::vector<cv::Mat> channels;
	split(image, channels);

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

	return med;
}

Mat treshold(Mat hsv, Mat roi)
{
	Mat t;
	hsv.copyTo(t);

	Scalar floor = median(roi);


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

	inRange(hsv, Scalar(LowHue, LowSat, LowVal), Scalar(HighHue, HighSat, HighVal), t);

	//inRange(hsv, Scalar(10, 10, 10), Scalar(180, 255, 30), t);
	return t;
}

Mat customHSV(Mat input)
{
	//Fonction qui converti l'image en HSV puis la filtre via des valeurs spécifiques.
	Mat hsv;
	input.copyTo(hsv);
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
	std::cout << "return ce shit" << std::endl;
	return hsv;
}

*/

int testflux()
{
	// Touche clavier
	char key = -1;
	long long framenb = 0; //numero de la frame
	//Definition des matrices utilisées
	cv::Mat frame, hsv, ROIhsv, MedianCol, tree;

	cv::Scalar med;


	std::cout << "Ouverture flux" << std::endl;

	// Ouvrir le flux vidéo via réseau
	//VideoCapture cap("http://192.168.20.1:8091/webcam.flv");
	//via webcam
	cv::VideoCapture cap(0);

	//test udp
	//VideoCapture cap("udp://192.168.42.202:6666/?action=stream;type=.mjpg");
	//VideoCapture cap("udp://@236.6.6.6:6666/?action=stream;type=.mjpg");

	// Vérifier si l'ouverture du flux est ok
	if (!cap.isOpened())
	{
		printf("Ouverture du flux vidéo impossible !\n");
		return 1;
	}

	// Définition des fenêtres
	cvNamedWindow("Video raspberry", CV_WINDOW_AUTOSIZE);
	/*
	cvNamedWindow("Region of interest", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Couleur mediane", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Seuil", CV_WINDOW_AUTOSIZE);
	*/

	//initialisation des objets
	Image* capture = new Image();
	Computing* comp = new Computing();
	//ROI* Roi = new ROI(frame);

	// Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
	while (key != 'q' && key != 'Q')
	{
		cap >> frame;
		capture->refresh(frame);
		comp->refresh(frame);
		if (framenb > 5)
		{
			//application du filtre gaussien pour éviter le bruit
			GaussianBlur(frame, frame, cv::Size(5, 5), 0, 0);

			//copie de la région d'intêret
			comp->refresh(frame);

			//conversion BGR = > HSV
			/*
			cvtColor(frame, hsv, COLOR_BGR2HSV);
			cvtColor(ROI, ROIhsv, COLOR_BGR2HSV);
			*/
			//hsv = customHSV(frame);
			//ROIhsv = customHSV(ROI);

			comp->getRoiImg().copyTo(MedianCol);

			//détermination de la couleur médiane
			//Roi->median();

			std::cout << "Median: " << comp->getRoi()->getmedian().val[0] << " " << comp->getRoi()->getmedian().val[1] << " " << comp->getRoi()->getmedian().val[2] << std::endl;

			//Affiche un rectangle de la couleur mediane
			rectangle(MedianCol, cv::Point(0, 0), cv::Point(400, 200), comp->getRoi()->getmedian(), -1, 8);


			//inRange(frameHSV, Scalar(50, 50, 50), Scalar(150, 150, 150), tree);
			//tree = treshold(hsv, ROIhsv);
			tree = comp->getThreshold();

			//morphological opening (remove small objects from the foreground)
			erode(tree, tree, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
			dilate(tree, tree, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
			erode(tree, tree, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
			//  erode(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			dilate(tree, tree, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
			dilate(tree, tree, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
			//erode(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			cv::medianBlur(tree, tree, 7);



			//morphological closing (fill small holes in the foreground)
			/*
			dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			erode(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			*/

		}
		//std::cout << "sortie de boucle" << std::endl;
		//cvtColor(hsv, hsv,CV_HSV2BGR);

		// On affiche l'image dans une fenêtre
		if (framenb > 5)
		{
			cvtColor(MedianCol, MedianCol, CV_HSV2BGR);
			cv::imshow("Video raspberry", comp->getimg());
			cv::imshow("Region of interest", comp->getRoiImg());
			cv::imshow("Couleur mediane", MedianCol);
			cv::imshow("Seuil", tree);
		}

		// On attend 10ms
		key = cvWaitKey(10);
		framenb++;
	}

	//cvReleaseCapture(&cap);
	cvDestroyWindow("Video raspberry");

	return 0;
}

int main()
{
	testflux();

	return 0;
}
