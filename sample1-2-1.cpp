#include <opencv2/opencv.hpp>
#include <stdio.h>

cv::Mat inpaint_mask;
cv::Mat original_image, whiteLined_image, inpainted;

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
        cv::rectangle(inpaint_mask, prevPt, pt, cv::Scalar(255), -1, 8, 0);
        cv::rectangle(whiteLined_image, prevPt, pt, cv::Scalar::all(255), -1, 8, 0);
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
    inpainted = original_image.clone();
    inpaint_mask.create(original_image.size(), CV_8UC1);

    inpaint_mask = cv::Scalar(0);
    inpainted = cv::Scalar(0);

    // 4. show image to window for generating mask
    cv::imshow("image", whiteLined_image);

    // 5. set callback function for mouse operations
    cv::setMouseCallback("image", myMouseEventHandler, 0);


    bool loop_flag = true;
    while(loop_flag){

        // 6. wait for key input
        int c = cv::waitKey(0);

        // 7. process according to input
        switch(c){
        case 27://ESC
        case 'q':
            loop_flag = false;
            break;
        case 'r':
            inpaint_mask = cv::Scalar(0);
            original_image.copyTo(whiteLined_image);
            cv::imshow("image", whiteLined_image);
            break;
        case 'i':
        case 10://ENTER(LF)
        case 13://ENTER(CR)
            cv::namedWindow("inpainted image", 1);
            cv::inpaint(whiteLined_image, inpaint_mask, inpainted, 3.0, cv::INPAINT_TELEA);
            cv::imshow("inpainted image", inpainted);
            break;
        }
    }
    
    return 0;
}