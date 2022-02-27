#include <iostream>
#include <cmath>
#include "typedefs.h"
#include <opencv2/opencv.hpp>
#include "opencv2/core/types_c.h"
#include "opencv2/calib3d/calib3d_c.h"
using namespace cv;
using namespace std;

#define NUM_OF_CORNERS 38

int main() {
	Mat srcImage, resizedImage, grayImage;
	const int scale = 3;
	srcImage = imread("184608_D.jpg");
	if (!srcImage.data) {
		cout << "ERROR! Cannot read file.";
		waitKey();
		return(-1);
	}

	resize(srcImage, resizedImage, Size(srcImage.cols / scale, srcImage.rows / scale));
	cvtColor(resizedImage, grayImage, COLOR_BGR2GRAY);

	Size patternsize(CheckerboardInnerWidth[0], CheckerboardInnerHeight[0]);
	vector<Point2f> corners;
	for (int i = 0; i < 4; i++) {
		Point2f point(CheckerboardInnerWidth[i], CheckerboardInnerHeight[i]);
		corners.push_back(point);
	}
	if (corners.front().y > corners.back().y)
		reverse(corners.begin(), corners.end());

	if (!findChessboardCorners(grayImage, patternsize, corners)) {
		cout << "ERROR! Couldn't find chessboard.";
		waitKey();
		return(-1);
	}
	cornerSubPix(grayImage, corners, patternsize, Size(CheckerboardInnerWidth[3], CheckerboardInnerHeight[3]), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
	drawChessboardCorners(grayImage, patternsize, corners, true);

	int dx = corners[1].x - corners[0].x;
	int dy = corners[1].y - corners[0].y;
	double d = sqrt(pow(dx, 2) + pow(dy, 2));
	
	for (int i = 0; i <= NUM_OF_CORNERS; i++) {
		putText(grayImage, to_string(i), corners[i], 1, 1, Scalar(0,0,0), 1, 8, false);
	}

	namedWindow("Michal Wrzosek");
	moveWindow("Michal Wrzosek", 0, 0);
	imshow("Michal Wrzosek", grayImage);
	imwrite("184608_D_TopChessboardFound.jpg", grayImage);

	waitKey();
}
