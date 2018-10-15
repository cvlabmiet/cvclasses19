/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <iostream>

typedef unsigned char uchar;

namespace cvlib
{
motion_segmentation::motion_segmentation()
	: mMu(0) , mCounter(0) {}

/* MinMax */
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double)
{
	if (mCounter > 1e2) mCounter = 0;

	if (_image.empty()) return;
	cv::Mat image = _image.getMat();
	cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
	//bool first = false;

	// first initialize
	/*if (mMax.empty()) image.copyTo(mMax);
	if (mMin.empty()) image.copyTo(mMin);
	if (mPrevFrame.empty()) mPrevFrame = cv::Mat::zeros(image.size(), CV_8UC1);
	if (mDiff.empty()) 
	{		
		first = true;
		image.copyTo(mDiff);
	}*/
	
	if (!mCounter)
	{
		image.copyTo(mMax);
		image.copyTo(mMin);
		mDiff = cv::Mat::zeros(image.size(), CV_8UC1);
		mPrevFrame = cv::Mat::zeros(image.size(), CV_8UC1);
	}
	
	mMu = 0;
	cv::Mat fgmask = cv::Mat::zeros(image.size(), CV_8UC1);
	
	// for fast median
	double cnt = 0;
	double sum = 0;
	double del = 0;

	for (int i = 0; i < image.size().width; i++)
	{
		for (int j = 0; j < image.size().height; j++)
		{
			uchar pix = image.at<uchar>(j,i);
			mMax.at<uchar>(j,i) = std::max(mMax.at<uchar>(j,i), pix);
			mMin.at<uchar>(j,i) = std::min(mMin.at<uchar>(j,i), pix);
			
			if (!mCounter) mDiff.at<uchar>(j,i) = std::max(mDiff.at<uchar>(j,i), (uchar)std::abs(double(pix) - double(mPrevFrame.at<uchar>(j,i))));

			mPrevFrame.at<uchar>(j,i) = pix;

			// count fast median
			/*{
				cnt++;
				sum += mDiff.at<uchar>(j,i);
				del = sum / (cnt*cnt);
				if (del > 0.1)
				{
					if (mMu > (double)mDiff.at<uchar>(j,i)) mMu -= del;
					else mMu += del;
				}
			}*/
		}
	}
	//mMu = std::round(mMu);
	
	// quick mat sort
	cv::Mat tmp;
	cv::sort(mDiff, tmp, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
	mMu = tmp.at<uchar>(tmp.size().height/2, tmp.size().width/2);

	for (int i = 0; i < image.size().width; i++)
	{
		for (int j = 0; j < image.size().height; j++)
		{
			double pix = (double)image.at<uchar>(j,i);
			bool cond1 = std::abs((double)mMax.at<uchar>(j,i) - pix) > mThreshold*mMu;
			bool cond2 = std::abs((double)mMin.at<uchar>(j,i) - pix) > mThreshold*mMu;
			fgmask.at<uchar>(j,i) = (cond1 || cond2) * 255;
		}
	}
	
	fgmask.copyTo(_fgmask);
	mCounter++;
	
    // \todo implement bg model updates
}
} // namespace cvlib
