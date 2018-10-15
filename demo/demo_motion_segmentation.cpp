/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Dmitrij Vukalov
 */

#include <chrono>
#include <ctime>
#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

using namespace std::chrono;


int demo_motion_segmentation(int argc, char* argv[])
{
    //const cv::String filename = "D:/ClangFormat/Video2.mp4";
    const int filename = 0;
    //cv::VideoCapture cap(0);
    cv::VideoCapture cap(filename);
    if (!cap.isOpened())
        return -1;

    // auto mseg = cv::createBackgroundSubtractorMOG2(); // \todo use cvlib::motion_segmentation
    auto mseg = cvlib::motion_segmentation();
    const auto main_wnd = "main";
    const auto demo_wnd = "demo";

    int threshold = 50;
    int alpha = 50;

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
    cv::createTrackbar("th", demo_wnd, &threshold, 255);
    cv::createTrackbar("alpha", demo_wnd, &alpha, 100);

    cv::Mat frame;
    cv::Mat frame_mseg;

    high_resolution_clock::time_point t1, t2;
    duration<float> duration;

    clock_t end;
    clock_t beg = std::clock();

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;

        if (frame.empty())
        {
            cap.release();
            //cap.open(0);
            cap.open(filename);
        }
        else
        {
            //t1 = high_resolution_clock::now();
            mseg.setThreshold(threshold);

            
            mseg.setAlpha(alpha);
            
            ////duration = (t2 - t1);

            cv::imshow(main_wnd, frame);
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
           
            mseg.apply(frame, frame_mseg, 0);
            
            //t2 = high_resolution_clock::now();
            //duration = (t2 - t1);
            
            end = std::clock();
            
           
            if (!frame_mseg.empty())
                cv::imshow(demo_wnd, frame_mseg);
            
            system("cls");
            std::cout << "FPS" << int(double(CLOCKS_PER_SEC) / (end - beg)) << std::endl;
            beg = end;

            // mseg->setVarThreshold(threshold); // \todo use TackbarCallback
            // mseg->apply(frame, frame_mseg);
            
        }
            
        // std::cout << "FPS" << cap.get(CV_CAP_PROP_FPS) << std::endl;
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
