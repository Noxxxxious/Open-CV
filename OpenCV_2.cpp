
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

Mat grayImage, binarizedImage;
int threshold_value = 100;
const int max_value = 255;

void Threshold(int pos, void* userdata) {
	threshold(grayImage, binarizedImage, threshold_value, max_value, THRESH_BINARY);
	imshow("Binarization", binarizedImage);
}
int main()
{
	Mat srcImage, resizedImage(100, 100, srcImage.type()), blurImage, CannyImage, LaplacianImage, scaledLaplacianImage, brightImage;
	srcImage = imread( "samples/ryba.jpg" );
	if ( !srcImage.data )
	{
		cout << "Error! Cannot read source file. Press ENTER.";
		waitKey();
		return(-1);
	}

	namedWindow("Michal_Wrzosek");
	moveWindow("Michal_Wrzosek", 0, 0);
	imshow("Michal_Wrzosek", srcImage);

	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
	namedWindow("Gray_Image");
	moveWindow("Gray_Image", 300, 0);
	imshow("Gray_Image", grayImage);

	imwrite("samples/gray_image.jpg", grayImage);

	resize(srcImage, resizedImage, resizedImage.size());
	namedWindow("Resized_Image");
	moveWindow("Resized_Image", 600, 0);
	imshow("Resized_Image", resizedImage);

	blur(srcImage, blurImage, Size(5, 5));
	namedWindow("Blurred_Image");
	moveWindow("Blurred_Image", 900, 0);
	imshow("Blurred_Image", blurImage);

	Canny(srcImage, CannyImage, 50, 50);
	namedWindow("Canny_edges");
	moveWindow("Canny_edges", 1200, 0);
	imshow("Canny_edges", CannyImage);

	Laplacian(srcImage, LaplacianImage, CV_16S, 3);
	convertScaleAbs(LaplacianImage, scaledLaplacianImage);
	namedWindow("Laplacian_Image");
	moveWindow("Laplacian_Image", 1500, 0);
	imshow("Laplacian_Image", scaledLaplacianImage);

	srcImage.copyTo(brightImage);
	for (int i = 0; i < brightImage.rows; i++) {
		for (int j = 0; j < brightImage.cols; j++) {
			Vec3b pixelColor;
			pixelColor = brightImage.at<Vec3b>(Point(i, j));
			for (int k = 0; k < 3; k++) {
				if (pixelColor[k] < 155)
					pixelColor[k] += 100;
				else
					pixelColor[k] = 255;
			}
			brightImage.at<Vec3b>(Point(i, j)) = pixelColor;
		}
	}
	namedWindow("Bright_Image");
	moveWindow("Bright_Image", 900, 300);
	imshow("Bright_Image", brightImage);

	namedWindow("Binarization");
	moveWindow("Binarization", 600, 0);
	createTrackbar("Threshold_value", "Binarization", &threshold_value, max_value, Threshold);
	Threshold(threshold_value, NULL);

	waitKey();

	namedWindow("_Src_video");
	moveWindow("_Src_video", 300, 600);
	namedWindow("_Dst_video");
	moveWindow("_Dst_video", 900, 600);


	Mat srcFrame, dstFrame, scaled_dstFrame;
	VideoCapture capture("samples/Dino.avi");
	capture >> srcFrame;
	VideoWriter writer("samples/Dino2.avi", -1, 25, srcFrame.size());
	while (waitKey(4) != 27 && !srcFrame.empty()) {
		Laplacian(srcFrame, dstFrame, CV_16S, 3);
		convertScaleAbs(dstFrame, scaled_dstFrame);
		writer << dstFrame;
		imshow("_Src_video", srcFrame);
		imshow("_Dst_video", scaled_dstFrame);
		capture >> srcFrame;
	}

	const int histSize = 256;
	const int hist_w = 256;
	const int hist_h = 256;
	float range[2] = { 0, 255 };
	const float *histRange = range;
	Mat histImageGray(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0)), histogramGray;

	calcHist(&grayImage, 1, 0, Mat(), histogramGray, 1, &histSize, &histRange);
	normalize(histogramGray, histogramGray, range[0], range[1], NORM_MINMAX);

	for (int i = 0; i < histSize; i++) {
		line(histImageGray, Point(i, hist_h - 1), Point(i, hist_h - cvRound(histogramGray.at<float>(i))), Scalar(100, 100, 50));
	}
	namedWindow("Histogram_Gray");
	moveWindow("Histogram_Gray", 0, 300);
	imshow("Histogram_Gray", histImageGray);

	Mat equalizeHistImage;
	equalizeHist(grayImage, equalizeHistImage);
	namedWindow("EQ_Histogram_Gray");
	moveWindow("EQ_Histogram_Gray", 300, 300);
	imshow("EQ_Histogram_Gray", equalizeHistImage);

	const int histSize2 = 256;
	const int hist_w2 = 256;
	const int hist_h2 = 256;
	float range2[2] = { 0, 255 };
	const float* histRange2 = range2;
	Mat histImageGray2(hist_h2, hist_w2, CV_8UC3, Scalar(0, 0, 0)), histogramGray2;

	calcHist(&equalizeHistImage, 1, 0, Mat(), histogramGray2, 1, &histSize2, &histRange2);
	normalize(histogramGray2, histogramGray2, range2[0], range2[1], NORM_MINMAX);

	for (int i = 0; i < histSize; i++) {
		line(histImageGray2, Point(i, hist_h - 1), Point(i, hist_h - cvRound(histogramGray2.at<float>(i))), Scalar(100, 100, 50));
	}
	namedWindow("Histogram_Gray2");
	moveWindow("Histogram_Gray2", 600, 300);
	imshow("Histogram_Gray2", histImageGray2);

	waitKey();

	srcImage.release();
}
