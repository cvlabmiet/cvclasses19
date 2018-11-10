/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-11-05
 * @author Anonymous
 */

#include <opencv2/opencv.hpp>

#include <cvlib.hpp>

int project_markup(int argc, char* argv[])
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

    auto video = parser.get<cv::String>("video");
    cv::VideoCapture cap(video);
    if (!cap.isOpened())
        return -1;

    cv::Mat frame;
    std::ofstream out(video + ".txt");
    if (!out.is_open())
        return -1;

    const auto origin_wnd = "origin";

    cv::namedWindow(origin_wnd, 1);
    while (true) // ESC
    {
        cap >> frame;
        if (frame.empty())
            break;

        cv::line(frame, cv::Point(frame.cols / 2, 0), cv::Point(frame.cols / 2, frame.rows), cv::Scalar(0, 0, 255), 2, 8);
        cv::imshow(origin_wnd, frame);

        switch (cv::waitKey(30))
        {
            case 27:
                cv::destroyWindow(origin_wnd);
                return 0;
            case ' ':
                out << cap.get(cv::CAP_PROP_POS_MSEC) << "\r\n";
                break;
            default:
                break;
        }
    }

    cv::destroyWindow(origin_wnd);

    return 0;
}
