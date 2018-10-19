/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"

const int gThresh = 10;
const int gWS[16] = {0,0,3,-3,1,2,3,3,2,1,-1,-2,-3,-3,-2,-1}; // Width Shifts
const int gHS[16] = {3,-3,0,0,1,2,3,-3,-2,-1,1,2,3,-3,-2,-1}; // Height Shifts
typedef unsigned char uchar;
 
/*
 *	. . * U * . .
 *	. * . . . * .
 *	* . . . . . *
 *	L . . C . . R
 *	* . . . . . *
 *	. * . . . * .
 *	. . * D * . .
 */

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();	
	cv::Mat mat = image.getMat();
	cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
	
	/*for (int w = 3; w < mat.size().width - 3; w++) { // 3 - radius of segment
		for (int h = 3; h < mat.size().height - 3; h++) {
			int cnt = 0;
			double pixC = (double)mat.at<Pixel>(h,w); // central pixel
			// Check 4 main pixels
			if (std::abs(pixC-(double)mat.at<Pixel>(h,w+3)) > gThresh) cnt++; // right pixel
			if (std::abs(pixC-(double)mat.at<Pixel>(h,w-3)) > gThresh) cnt++; // left pixel
			if (std::abs(pixC-(double)mat.at<Pixel>(h+3,w)) > gThresh) cnt++; // down pixel
			if (std::abs(pixC-(double)mat.at<Pixel>(h-3,w)) > gThresh) cnt++; // up pixel
			
			if (cnt > 2) { // if may-be-corner, check other 12 pixels
				// first quarter
				if (std::abs(pixC-(double)mat.at<Pixel>(h-3,w+1)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h-2,w+2)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h-1,w+3)) > gThresh) cnt++;
				// second quarter
				if (std::abs(pixC-(double)mat.at<Pixel>(h+1,w+3)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h+2,w+2)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h+3,w+1)) > gThresh) cnt++;
				// third quarter
				if (std::abs(pixC-(double)mat.at<Pixel>(h+3,w-1)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h+2,w-2)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h+1,w-3)) > gThresh) cnt++;
				// forth quarter
				if (std::abs(pixC-(double)mat.at<Pixel>(h-1,w-3)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h-2,w-2)) > gThresh) cnt++;
				if (std::abs(pixC-(double)mat.at<Pixel>(h-3,w-1)) > gThresh) cnt++;
				// final check
				if (cnt > 12) keypoints.push_back(cv::KeyPoint(w,h,3));
			}
		}
	}*/
	
	for (int w = 3; w < mat.size().width - 3; w++) { // 3 - radius of segment
		for (int h = 3; h < mat.size().height - 3; h++) {
			int cnt = 0;
			double pixC = (double)mat.at<uchar>(h,w); // central pixel
			for (int i = 0; i < 4; i++) { // Check 4 main pixels (Up, Down, Left and Right)
				if (std::abs(pixC-(double)mat.at<uchar>(h+gHS[i],w+gWS[i])) > gThresh) cnt++;
			}
			if (cnt > 2) { // if may-be-corner, check other 12 pixels
				for (int i = 4; i < 16; i++) {
					if (std::abs(pixC-(double)mat.at<uchar>(h+gHS[i],w+gWS[i])) > gThresh) cnt++;
				}
				if (cnt > 12) keypoints.push_back(cv::KeyPoint((float)w,(float)h,3.0f));
			}
		}
	}
}
} // namespace cvlib
