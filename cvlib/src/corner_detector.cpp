/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"
 
/*
 *	. . X U X . .
 *	. X . . . X .
 *	X . . . . . X
 *	L . . C . . R
 *	X . . . . . X
 *	. X . . . X .
 *	. . X D X . .
 */

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

// Check 4 main pixels (Up, Down, Left and Right) and if may-be-corner, check other 12 pixels
void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear(); cv::Mat mat = image.getMat();
	if (mat.channels() == 3) cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
	const int thresh = 10;
	const int WS[16] = {0,3, 0,-3,1,2,3, 3, 2, 1,-1,-2,-3,-3,-2,-1}; // Width Shifts
	const int HS[16] = {3,0,-3, 0,3,2,1,-1,-2,-3,-3,-2,-1, 1, 2, 3}; // Height Shifts
	for (int w = 3; w < mat.size().width - 3; w++) { // 3 - radius of segment
		for (int h = 3; h < mat.size().height - 3; h++) {
			int cntP = 0, cntM = 0, arrI[4], cntI = 0; bool isOk = true;
			double diff, pixC = (double)mat.at<unsigned char>(h,w); // central pixel
			for (int i = 0; i < 16 && (i<4||cntM>2||cntP>2) && cntI < 4; i++) {
				diff = pixC-(double)mat.at<unsigned char>(h+HS[i],w+WS[i]);
				if (diff > thresh) cntP++; else if (-diff > thresh) cntM++;
				else {arrI[cntI] = i*(1+3*(i<4))-(i>3)*(4-(i-1)/3); cntI++;}}
			for (int i = 0; i < cntI && cntI < 4; i++) for(int j = i+1; j < cntI && isOk; j++){
				diff = std::abs(arrI[i] - arrI[j]); if (diff > 3 && diff < 13) isOk = false; }
			if ((cntP >= 12 || cntM >= 12) && isOk) keypoints.push_back(cv::KeyPoint((float)w,(float)h,3.0f)); }}
}
} // namespace cvlib
