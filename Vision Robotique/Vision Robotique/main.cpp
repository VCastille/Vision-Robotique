#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Computing.h"
#include "Image.h"
#include "ROI.h"

//using namespace cv;

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

			comp->getRoiImg().copyTo(MedianCol);

			//détermination de la couleur médiane
			//Roi->median();

			std::cout << "Median: " << comp->getRoi()->getmedian().val[0] << " " << comp->getRoi()->getmedian().val[1] << " " << comp->getRoi()->getmedian().val[2] << std::endl;

			//Affiche un rectangle de la couleur mediane
			rectangle(MedianCol, cv::Point(0, 0), cv::Point(400, 200), comp->getRoi()->getmedian(), -1, 8);

			//seuillage
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
