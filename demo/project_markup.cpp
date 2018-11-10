/* Demo application for Computer Vision Library.
* @file
* @date 2018-11-05
* @author Anonymous
*/

#include <opencv2/opencv.hpp>

#include <cvlib.hpp>

int project_markup(int argc, char* argv[])
{
   const cv::String keys =
      "{help h usage ? |      | print this message   }"
      "{video          |      | video file           }";
   cv::CommandLineParser parser(argc, argv, keys);
   parser.about("Application name v1.0.0");
   if (parser.has("help"))
   {
      parser.printMessage();
      return 0;
   }

   cv::VideoCapture cap(parser.get<cv::String>("video"));
   if (!cap.isOpened())
      return -1;

   cv::Mat frame;

   const auto origin_wnd = "origin";

   cv::namedWindow(origin_wnd, 1);
   while (true) // ESC
   {
      cap >> frame;
      cv::imshow(origin_wnd, frame);
      std::cout << cap.get(CV_CAP_PROP_FRAME_COUNT) << "\r";

      switch (cv::waitKey(30))
      {
      case 27:
         return 0;
      case ' ':
         std::cout << cap.get(CV_CAP_PROP_FRAME_COUNT) << "\n";
         break;
      default:
         break;
      }
   }

   cv::destroyWindow(origin_wnd);

   return 0;
}
