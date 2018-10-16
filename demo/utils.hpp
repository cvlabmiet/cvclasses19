/* Utilites definitions for demo applications of Computer Vision Library.
* @file
* @date 2018-10-16
* @author Anonymous
*/

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <opencv2/opencv.hpp>

#include <memory>

namespace utils
{
/// \brief Responsible for calculating instant moving average for passed value
///        Stores the history of SizeT last values
struct moving_average
{
public:
   /// \brief ctor
   /// \param length, in - the history length (moving window size)
   moving_average(int length = 30);

   /// \brief Adds new value to history and returns new calculated moving average
   double add_new_value(double val);

private:
   std::vector<double> history_; //< container of last values
   const int history_length_; //< number of values to be averaged
   int current_index_ = true; //< point to the index where new value shall be wrote/rewrote
   bool is_initialized_ = false; //< flag is true when history is complete
   double accumulator_ = 0; //< an instant sum of all values in history
};

/// \brief Calculates smoothed fps
struct fps_counter
{
public:
   /// \brief ctor
   fps_counter(int length = 30);

   /// \brief calculates smoothed fps for new frame
   double process_new_frame();

private:
   moving_average accum_; //< accumulator for average fps
   int64 last_tick_count_; //< timestamp of the last processed frame
};

/// \brief Adds FPS text on passed image by updating passed fps_counter
/// \param image, in/out - image where FPS will be printed
/// \param fps, in/out - fps_counter which is used for tracking processed frames
/// \param color, in - the color of text
void put_fps_text(cv::Mat& image, fps_counter& fps, cv::Scalar color = { 255, 0, 0 });
} // utils

#endif // __UTILS_HPP__