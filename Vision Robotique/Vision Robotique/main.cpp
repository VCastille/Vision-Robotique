#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;


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

	Scalar average = median(roi);

	int LowHue = average.val[0] - 20;
	int HighHue = average.val[0] + 20;

	int LowSat;
	if (average.val[1] < 20)
		LowSat = 0;
	else
		LowSat = 10;

	int HighSat;
	if (average.val[2] >= 225)
		HighSat = 255;
	else
		HighSat = 200;

	int LowVal;
	int HighVal;

	if (average.val[2] < 20)
	{
		LowVal = 0;
		HighVal = 35;
		//si la couleur est noire : on accepte toutes les teintes.
		LowHue = 0;
		HighHue = 255;

	}
	else if (average.val[2] > 220)
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




	/*
	int LowSat = average.val[1]-75;
	int HighSat = average.val[1]+75;
	int LowVal = average.val[2]-150;
	int HighVal = average.val[2]+150;
	*/

	if (LowHue < 0)
		LowHue = 0;
	if (LowSat < 0)
		LowSat = 0;
	if (LowVal < 0)
		LowVal = 0;

	std::cout << "Zone ROI: h = " << average.val[0] << " s = " << average.val[1] << " v = " << average.val[2] << std::endl;
	std::cout << "test valeurs: " << LowHue << " " << LowSat << " " << LowVal << std::endl;

	inRange(hsv, Scalar(LowHue, LowSat, LowVal), Scalar(HighHue, HighSat, HighVal), t);

	//inRange(hsv, Scalar(10, 10, 10), Scalar(180, 255, 30), t);
	return t;
}


int testflux()
{
	// Touche clavier
	char key = -1;
	long long framenb = 0; //numero de la frame
	//Definition des matrices utilisées
	Mat frame;
	Mat ROI;
	Mat hsv;    //Matrice dans l'espace HSV: permet de mieux gérer les différences de lumière
	Mat ROIhsv;
	Mat MedianCol;
	Mat tree;

	Scalar med;


	std::cout << "Ouverture flux" << std::endl;

	// Ouvrir le flux vidéo via réseau
	//VideoCapture cap("http://192.168.20.1:8091/webcam.flv");
	//via webcam
	VideoCapture cap(0);

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

	// Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
	while (key != 'q' && key != 'Q')
	{
		cap >> frame;

		
		if (framenb > 5)
		{
			//application du filtre gaussien pour éviter le bruit
			GaussianBlur(frame, frame, Size(5, 5), 0, 0);

			//copie de la région d'intêret
			frame.copyTo(ROI);
			ROI = extractROI(ROI);

			//conversion BGR = > HSV
			cvtColor(frame, hsv, COLOR_BGR2HSV);
			cvtColor(ROI, ROIhsv, COLOR_BGR2HSV);
			ROIhsv.copyTo(MedianCol);

			//détermination de la couleur médiane
			med = median(MedianCol);
			std::cout << "Median: " << med.val[0] << " " << med.val[1] << " " << med.val[2] << std::endl;

			//Affiche un rectangle de la couleur mediane
			rectangle(MedianCol, Point(0, 0), Point(400, 200), med, -1, 8);


			//inRange(frameHSV, Scalar(50, 50, 50), Scalar(150, 150, 150), tree);
			tree = treshold(hsv, ROIhsv);
			//morphological opening (remove small objects from the foreground)
			erode(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			erode(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//  erode(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//erode(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//dilate(tree, tree, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			medianBlur(tree, tree, 7);



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
			imshow("Video raspberry", frame);
			imshow("Region of interest", ROI);
			imshow("Couleur mediane", MedianCol);
			imshow("Seuil", tree);
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
	disclaimer();

	testflux();

	return 0;
}
