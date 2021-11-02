/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

int demo_motion_segmentation(int argc, char* argv[])
{
    cv::VideoCapture cap(cv::CAP_V4L2);
    if (!cap.isOpened())
        return -1;

    //auto mseg = cv::createBackgroundSubtractorMOG2(); // \todo use cvlib::motion_segmentation
    cvlib::motion_segmentation* mseg = new cvlib::motion_segmentation();
    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    int threshold = 50;
    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
    cv::createTrackbar("th", demo_wnd, &threshold, 255);
    cv::Mat frame;
    cv::Mat frame_mseg;
    int key = 0;
    std::string method("Mean");

    while (key != 27) // ESC
    {
        cap >> frame;
        medianBlur( frame, frame, 3 );
        cv::GaussianBlur(frame, frame, cv::Size(5,5), 0, 0);
        cv::imshow(main_wnd, frame);

        if(key == '1')
        {
            mseg->setMethod(cvlib::BSmethod::Mean);
            method = "Mean";
        }
        if(key == '2')
        {
            mseg->setMethod(cvlib::BSmethod::G1);
            method = "G1";
        }

        mseg->setVarThreshold(threshold);
        mseg->apply(frame, frame_mseg);
        if (!frame_mseg.empty())
        {
            putText(frame_mseg, method, cv::Point(5,50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255), 2);
            cv::imshow(demo_wnd, frame_mseg);
        }
            

        key = cv::waitKey(15);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
