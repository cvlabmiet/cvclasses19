/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cvlib.hpp>
#include <ctime>
#include <string>

int demo_split_and_merge(int argc, char* argv[])
{
        const cv::String keys = // clang-format off
      "{help h usage ? |      | print this message   }"
      "{video          |      | video file           }";
    // clang-format on
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Application name v1.0.0");
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    cv::VideoCapture cap;
    if(parser.has("video"))
    {
        auto video = parser.get<cv::String>("video");
        cap.open(video);
    }else
    {
        cap.open(0);
    }
    

    if (!cap.isOpened())
        return -1;

    cv::Mat frame;
    cv::Mat frame_gray;

    const auto origin_wnd = "origin";
    const auto demo_wnd = "demo";

    int stddev = 25, mean = 12;
    cv::namedWindow(demo_wnd, 1);
    cv::createTrackbar("stdev", demo_wnd, &stddev, 45);
    cv::createTrackbar("mean", demo_wnd, &mean, 50);
    int start, end;
    float dur;

    bool pause = false;


    while (cv::waitKey(30) != 27) // ESC
    {
        if(!pause)
        {
            cap >> frame;
            cv::resize(frame,frame,cv::Size(480,320));
        }
        if(cv::waitKey(30) == 112) pause = !pause;

        cv::imshow(origin_wnd, frame);

        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(frame_gray, frame_gray, cv::Size(7,7), 0, 0);
        start = clock();
        frame_gray = cvlib::split_and_merge(frame_gray, stddev, mean);
        end = clock();
        dur = static_cast<float>((end - start))/CLOCKS_PER_SEC;
        putText(frame_gray, std::string("fps: ") + std::to_string(static_cast<int>(1/dur)), cv::Point(5,50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0), 2);
        cv::imshow(demo_wnd, frame_gray);
    }

    cv::destroyWindow(origin_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
