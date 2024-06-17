#include <time.h>

#include <iostream>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc.hpp>  // Blur
using namespace std;
using namespace cv;
const int max_frames = 100;
double average(vector<double>& v) {
  double res = 0;
  for (size_t i = 0; i < max_frames; i++) {
    res += v[i];
  }
  return res / max_frames;
}
int main(int argc, char* argv[]) {
  timespec start, end;
  timespec com_st, com_end;
  VideoCapture cap(0);
  vector<double> read_time(max_frames);
  vector<double> convert_time(max_frames);
  vector<double> show_time(max_frames);
  vector<double> frame_time(max_frames);
  Mat src;
  Mat dst;
  int num_frames = 0;
  double num_sec = 0;
  clock_gettime(CLOCK_MONOTONIC_RAW, &com_st);
  while (num_frames < max_frames) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    cap.read(src);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    num_sec =
        end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec);
    read_time[num_frames] = num_sec;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    medianBlur(src, dst, 31);
    flip(dst, dst, 0);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    num_sec =
        end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec);
    convert_time[num_frames] = num_sec;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    imshow("Blur&Flip", dst);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    num_sec =
        end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec);
    show_time[num_frames] = num_sec;
    frame_time[num_frames] = read_time[num_frames] + convert_time[num_frames] +
                             show_time[num_frames];
    char c = waitKey(1);
    if (c == 'z') break;
    num_frames++;
  }
  clock_gettime(CLOCK_MONOTONIC_RAW, &com_end);
  double common_sec = com_end.tv_sec - com_st.tv_sec +
                      0.000000001 * (com_end.tv_nsec - com_st.tv_nsec);
  cout << "Read time: " << average(read_time) << " sec. "
       << average(read_time) / average(frame_time) * 100 << "%" << endl;
  cout << "Convert time: " << average(convert_time) << " sec. "
       << average(convert_time) / average(frame_time) * 100 << "%" << endl;
  cout << "Show time: " << average(show_time) << " sec. "
       << average(show_time) / average(frame_time) * 100 << "%" << endl;
  cout << "Frame time: " << average(frame_time) << " sec. " << endl;
  cout << "Time taken: " << common_sec << " sec. " << endl;
  return 0;
}
