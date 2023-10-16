#include <opencv2/opencv.hpp>
#include <stdio.h>

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FRAME_RATE 15

int main(int argc, char *argv[])
{
    // 1. load classifier
    std::string cascadeName = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml"; //Haar-like
    cv::CascadeClassifier cascade;
    if(!cascade.load(cascadeName)){
        printf("ERROR: cascadeFile not found\n");
        return -1;
    }

    // 2. initialize VideoCapture
    cv::Mat frame;
    cv::VideoCapture cap;
    cap.open(0, cv::CAP_V4L2);

    cap.set(cv::CAP_PROP_FPS, FRAME_RATE);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

    cap >> frame;

    // 3. prepare window and trackbar
    cv::namedWindow("result", 1);

    double scale = 4.0;
    cv::Mat gray, smallImg(cv::saturate_cast<int>(frame.rows/scale),
                cv::saturate_cast<int>(frame.cols/scale), CV_8UC1);

    for(;;){

        // 4. capture frame
        cap >> frame;
        //convert to gray scale
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // 5. scale-down the image
        cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
        cv::equalizeHist(smallImg, smallImg);

        // 6. detect face using Haar-classifier
        std::vector<cv::Rect> faces;
        ///multi-scale face searching
        // image, size, scale, num, flag, smallest rect
        cascade.detectMultiScale(smallImg, faces,
                        1.1,
                        1,
                        cv::CASCADE_SCALE_IMAGE,
                        cv::Size(30, 30));
        // 7. mosaic(pixelate) face-region
        for(int i = 0; i < faces.size(); i++){
            cv::rectangle(frame,
                cv::Point(cv::saturate_cast<int>((faces[i].x)*scale),
                    cv::saturate_cast<int>((faces[i].y)*scale)),
                cv::Point(cv::saturate_cast<int>((faces[i].x + faces[i].width-1)*scale),
                    cv::saturate_cast<int>((faces[i].y + faces[i].height-1)*scale)),
                cv::Scalar(0, 200, 0), 3, 8, 0);
        }

        // 8. show mosaiced image to window
        cv::imshow("result", frame);

        int key = cv::waitKey(10);
        if(key == 'q' || key == 'Q')break;

    }
    return 0;
}