#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Computing.h"
#include "Image.h"
#include "ROI.h"


using namespace cv;
using namespace std;


int testflux()
{
	// Touche clavier
	char key = -1;
	long long framenb = 0; //numero de la frame

	//Definition des matrices utilisées
	cv::Mat frame, hsv, ROIhsv, MedianCol, tree, canny;

	cv::Scalar med;

	std::cout << "Ouverture flux" << std::endl;

	// Ouvrir le flux vidéo via réseau
	//VideoCapture cap("http://192.168.20.1:8091/webcam.flv")
	//test udp
	//VideoCapture cap("rtsp://192.168.0.23:8554/");
	//cv::VideoCapture cap("udp://@236.6.6.6:6666");

	//via webcam
	cv::VideoCapture cap(0);


	// Vérifier si l'ouverture du flux est ok
	if (!cap.isOpened())
	{
		printf("Ouverture du flux vidéo impossible !\n");
		return 1;
	}

	// Définition des fenêtres
	cvNamedWindow("Video raspberry", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Region of interest", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Couleur mediane", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Seuil", CV_WINDOW_AUTOSIZE);

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
			//application du filtre gaussien pour réduire le bruit
			GaussianBlur(frame, frame, cv::Size(5, 5), 0, 0);

			//copie de la région d'intêret
			comp->refresh(frame);

			//
			comp->getRoiImg().copyTo(MedianCol);

			//affichage du Median
			std::cout << "Median: " << comp->getRoi()->getmedian().val[0] << " " << comp->getRoi()->getmedian().val[1] << " " << comp->getRoi()->getmedian().val[2] << std::endl;

			//Affiche un rectangle de la couleur mediane
			rectangle(MedianCol, cv::Point(0, 0), cv::Point(400, 200), comp->getRoi()->getmedian(), -1, 8);

			//seuillage
			tree = comp->getThreshold();
			erode(tree, tree, getStructuringElement(MORPH_ERODE, Size(5, 5)));
			erode(tree, tree, getStructuringElement(MORPH_ERODE, Size(5, 5)));
		}

		// On affiche l'image dans une fenêtre
		if (framenb > 5)
		{
			cvtColor(MedianCol, MedianCol, CV_HSV2BGR);
			cv::imshow("Video raspberry", comp->getimg());
			cv::imshow("Region of interest", comp->getRoiImg());
			cv::imshow("Couleur mediane", MedianCol);
			cv::imshow("Seuil", comp->getThreshold());
		}

		// On attend 10ms
		key = cvWaitKey(40);
		framenb++;
	}

	cvDestroyWindow("Video raspberry");

	return 0;
}

int main()
{
	testflux();

	return 0;
}
