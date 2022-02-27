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
	srcImage = imread("184608_G.jpg");
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

	if (!findChessboardCorners(grayImage, patternsize, corners)) {
		cout << "ERROR! Couldn't find chessboard.";
		waitKey();
		return(-1);
	}
	cornerSubPix(grayImage, corners, Size(5,5), Size(-1, -1), TermCriteria());
	if (corners.front().y < corners.back().y)
		reverse(corners.begin(), corners.end());
	drawChessboardCorners(grayImage, patternsize, corners, true);
	imwrite("184608_G_ChessboardFound.jpg", grayImage);

	int dx = corners[1].x - corners[0].x;
	int dy = corners[1].y - corners[0].y;
	double d = sqrt(pow(dx, 2) + pow(dy, 2));
	
	for (int i = 0; i <= NUM_OF_CORNERS; i++) {
		putText(grayImage, to_string(i), corners[i], 1, 1, Scalar(0,0,0), 1, 8, false);
	}
	imwrite("184608_G_TopChessboardFound.JPG", grayImage);

	float reverseScale = 100.0f;
	vector<Point2f> templateCorners;
	for (int i = 0; i < patternsize.height; i++) {
		for (int j = 0; j < patternsize.width; j++) {
			Point2f coord;
			coord.x = CheckerboardLTCoordinatesWithMargin[0].x + (j + 1) * CHECKERBOARD_FIELD_SIZE;
			coord.y = CheckerboardLTCoordinatesWithMargin[0].y + (i + 1) * CHECKERBOARD_FIELD_SIZE;
			templateCorners.push_back(coord * reverseScale);
		}
	}

	Mat homography = findHomography(corners, templateCorners, CV_RANSAC);
	Mat dewarpedImage(srcImage.size(), srcImage.type());
	warpPerspective(resizedImage, dewarpedImage, homography, srcImage.size());
	imwrite("184608_G_Dewarped.jpg", dewarpedImage);

	Mat dewarpedROI = dewarpedImage(Rect(reverseScale * TemplateLT, reverseScale * TemplateRB));
	Mat debugImage = dewarpedROI.clone();
	vector<Point2f>dewarpedCorners;
	perspectiveTransform(corners, dewarpedCorners, homography);
	for (int i = 0; i < 39; i++) {
		circle(debugImage, dewarpedCorners[i], 8, Scalar(255, 0, 0), 2);
	}
	imwrite("184608_G_Dewarped_ROI.jpg", debugImage);

	namedWindow("Michal Wrzosek");
	moveWindow("Michal Wrzosek", 0, 0);
	imshow("Michal Wrzosek", debugImage);

	waitKey();
}
