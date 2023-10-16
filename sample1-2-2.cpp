#include <opencv2/opencv.hpp>
#include <stdio.h>

cv::Mat original_image, whiteLined_image, mask;

void myMouseEventHandler(int event, int x, int y, int flags, void *){
    if(whiteLined_image.empty()) return;

    static cv::Point prevPt;
    cv::Point pt(x, y);
    cv::Mat temp_image;

    if(event == cv::EVENT_LBUTTONDOWN) prevPt = pt;
    if((bool)(flags & cv::EVENT_FLAG_LBUTTON))  // OpenCV 4.5.4 では動作しないことがあるので、その場合は適宜他のキーに変更してください
    {
        temp_image = whiteLined_image.clone();
        cv::rectangle(temp_image, prevPt, pt, cv::Scalar::all(255), -1, 8, 0);
        cv::imshow("image", temp_image);
    }
    if(event == cv::EVENT_LBUTTONUP) {
        cv::bitwise_not(whiteLined_image, whiteLined_image, mask);
        cv::rectangle(mask, prevPt, pt, cv::Scalar(255), -1, 8, 0);
        cv::bitwise_not(whiteLined_image, whiteLined_image, mask);
        cv::imshow("image", whiteLined_image);
    }
}

int main(int argc, char *argv[]){
    // 1. read image file
    char *filename = (argc >= 2) ? argv[1] : (char *)"fruits.jpg";
    original_image = cv::imread(filename);
    if(original_image.empty()){
        printf("ERROR: image not found!\n");
        return 0;
    }

    // 2. prepare window
    cv::namedWindow("image",1);

    // 3. prepare Mat objects for processing-mask and processed-image
    whiteLined_image = original_image.clone();

    mask = cv::Mat::zeros(whiteLined_image.size(), CV_8UC1);

    // 4. show image to window for generating mask
    cv::imshow("image", whiteLined_image);

    // 5. set callback function for mouse operations
    cv::setMouseCallback("image", myMouseEventHandler, 0);

    // 6. wait for key input
    int c = cv::waitKey(0);
    
    return 0;
}