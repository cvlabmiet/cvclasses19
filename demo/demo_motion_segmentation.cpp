/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

namespace
{
void trackbar_callback_func(int threshold, void* obj)
{
	cvlib::motion_segmentation* detector = (cvlib::motion_segmentation*)obj;
	detector->setVarThreshold(threshold);
}
}; // namespace


int demo_motion_segmentation(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

	cvlib::motion_segmentation* mseg = new cvlib::motion_segmentation(); // \todo use cvlib::motion_segmentation
    const auto main_wnd = "main";
    const auto demo_wnd = "demo";

    int threshold = 20;
	int alpha = 25;
    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    cv::createTrackbar("thresh", demo_wnd, &threshold, 255, trackbar_callback_func, (void*)mseg);
	cv::createTrackbar("alpha", demo_wnd, &alpha, 100);
    cv::Mat frame;
	cv::Mat frame_gray;
    cv::Mat frame_mseg;

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
		cv::cvtColor(frame, frame_gray, CV_BGR2GRAY);
        cv::imshow(main_wnd, frame);

		mseg->setVarThreshold(threshold); // \todo use TackbarCallback
		mseg->apply(frame_gray, frame_mseg, alpha / 100.0);
        if (!frame_mseg.empty() && mseg->isinitialized())
            cv::imshow(demo_wnd, frame_mseg);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

	delete mseg;

    return 0;
}
