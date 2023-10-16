#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
using namespace std;

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FRAME_RATE 15

int main(int argc, char *argv[])
{
  int INIT_TIME = 50;
  double B_PARAM = 1.0 / 50.0;
  double T_PARAM = 1.0 / 200.0;
  double Zeta = 10.0;

  cv::VideoCapture cap;
  cv::Mat frame;
  cv::Mat pre_img, tmp_img;
  cv::Mat dst_img, msk_img, diff_img;

  // 1. initialize VideoCapture
  if(argc > 1){
    char *in_name = argv[1];
    if('0' <= in_name[0] && in_name[0] <= '9' && in_name[1] == '\0'){
      cap.open(in_name[0] - '0', cv::CAP_V4L2);  // open with device id
    }else{
      cap.open(in_name);  // open with filename
    }
  }else{
    cap.open(0, cv::CAP_V4L2);  // changed
  }

  cap.set(cv::CAP_PROP_FPS, FRAME_RATE);
  cap.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
  
  if(!cap.isOpened()){
    printf("Cannot open the video.\n");
    exit(0);
  }

  // 2. prepare window for showing images
  cv::namedWindow("Input", 1);
  cv::namedWindow("FG", 1);
  cv::namedWindow("mask", 1);

  // 3. calculate initial value of background
  cap >> frame;

  cv::Size s = frame.size();

  tmp_img.create(s, CV_32FC3);
  pre_img.create(s, CV_32FC3);
  diff_img.create(s, CV_32FC3);

  dst_img.create(s, CV_8UC3);
  msk_img.create(s, CV_8U);

  cv::Mat back_image = cv::imread("fruits.jpg");
  if(back_image.empty()){
      printf("ERROR: image not found!\n");
      return 0;
  }
  cv::resize(back_image, back_image, dst_img.size());

  bool loop_flag = true;
  while(loop_flag){
    cap >> frame;
    if (frame.empty()) {
        break;   // changed
    }
    pre_img = tmp_img.clone();
    frame.convertTo(tmp_img, tmp_img.type());

    cv::absdiff(tmp_img, pre_img, diff_img);
    cv::threshold(diff_img, msk_img, 20, 255, cv::THRESH_BINARY);

    cv::cvtColor(msk_img, msk_img, cv::COLOR_BGR2GRAY);

    dst_img = cv::Scalar(0);
    msk_img.convertTo(msk_img, CV_8UC1);
    frame.copyTo(dst_img, msk_img);

    cv::Mat back;
    cv::bitwise_not(msk_img, msk_img);
    back_image.copyTo(back, msk_img);

    cv::add(dst_img, back, dst_img);
    

    cv::imshow("Input", frame);
    cv::imshow("FG", dst_img);
    cv::imshow("mask", msk_img);

    char key =cv::waitKey(10);
    if(key == 27){
      loop_flag = false;
    }
  }
  return 0;
}