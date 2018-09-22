/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <opencv2/opencv.hpp>

int demo_split_and_merge(int argc, char* argv[]); // lab 1
int demo_select_texture(int argc, char* argv[]); // lab 2

int main(int argc, char* argv[])
{
    cv::namedWindow("main");
    cv::Mat help = cv::Mat::zeros(300, 500, CV_8UC3);
    cv::putText(help, "Press ESC to exit", cv::Point(30, 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0), 1, CV_AA);
    cv::putText(help, "Press 1 for Lab 1 Demo (split and merge)", cv::Point(30, 50), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0), 1, CV_AA);
    cv::putText(help, "Press 2 for Lab 2 Demo (texture segmentation)", cv::Point(30, 70), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0), 1, CV_AA);
    cv::imshow("main", help);

    while (true)
    {
        int key = cv::waitKey(0);

        switch (key)
        {
            case '1':
                demo_split_and_merge(argc, argv);
                break;
            case '2':
                demo_select_texture(argc, argv);
                break;
            case 27: // ESC
                return 0;
        }
    }
}
