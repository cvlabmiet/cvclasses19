/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <ctime>
#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

cvlib::motion_segmentation* mseg = nullptr;

void on_trackbar( int val, void* )
{
	mseg->setVarThreshold(val);
}

int demo_motion_segmentation(int argc, char* argv[])
{
	int file_name = 0;
	//const char* file_name = "D:/Games/motion_sample1.mp4";
	//const char* file_name = "D:/Games/smeshariki_sample.mp4";

    cv::VideoCapture cap(file_name);

    if (!cap.isOpened())
        return -1;

    //auto mseg = cv::createBackgroundSubtractorMOG2(); // \todo use cvlib::motion_segmentation
	mseg = new cvlib::motion_segmentation();
    const auto main_wnd = "main";
    const auto demo_wnd = "demo";
    const auto min_wnd = "min";
    const auto max_wnd = "max";
    const auto diff_wnd = "diff";

    int threshold = 10;
    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
    cv::namedWindow(min_wnd);
    cv::namedWindow(max_wnd);
    cv::namedWindow(diff_wnd);
    cv::createTrackbar("th", demo_wnd, &threshold, 500, on_trackbar);

    cv::Mat frame;
    cv::Mat frame_mseg;
	
	clock_t end;
	clock_t beg = std::clock();
	
    while (cv::waitKey(30) != 27) // ESC
    {
		cap >> frame;
		if (frame.empty())
		{
			cap.release();
			cap.open(file_name);
			cap >> frame;
		}

        cv::imshow(main_wnd, frame);

        //mseg->setVarThreshold(threshold); // \todo use TackbarCallback
        mseg->apply(frame, frame_mseg);
        if (!frame_mseg.empty())
		{
            cv::imshow(demo_wnd, frame_mseg);
            cv::imshow(min_wnd, mseg->getMin());
            cv::imshow(max_wnd, mseg->getMax());
            cv::imshow(diff_wnd, mseg->getDiff());
		}

		system("cls");
		end = std::clock();
		std::cout << "FPS: " << int( double(CLOCKS_PER_SEC) / (end - beg) ) << "\n";
		beg = end;
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);
    cv::destroyWindow(min_wnd);
    cv::destroyWindow(max_wnd);
    cv::destroyWindow(diff_wnd);

    return 0;
}