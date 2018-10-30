#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

#include "Bird.h"

#define URbegY (height / 3 * 2)
#define URendY (height)
#define URbegX (0)
#define URendX (width / 6 * 2)

#define ULbegY (height / 3 * 2)
#define ULendY (height)
#define ULbegX (width / 6 * 4)
#define ULendX (width)


using namespace std;
using namespace cv;

int width = 0;
int height = 0;

Mat frame;
Mat fgMaskMog;
Mat fgMaskMogPre;
cv::Ptr<cv::BackgroundSubtractorMOG2> pMOG2;

bool compareArea(Mat fa, Mat fb, int ya, int yb, int xa, int xb){

    if(fa.empty() || fb.empty())
        return false;
    int acc = 0;
    for(int y = ya; y < yb; y++)
        for(int x = xa; x < xb; x++)
            acc += fa.at<uchar>(y, x) != fb.at<uchar>(y, x);
    acc += 1;
    return acc > abs(yb - ya) * abs(xb - xa) / 10;
}

int main(){

    VideoCapture cap(0);
    pMOG2 = cv::createBackgroundSubtractorMOG2();

    if(!cap.isOpened())
        cout << "cam not opened" << endl;
    cvNamedWindow("Video");
    cvNamedWindow("Mask");

    width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    Bird birdie = Bird();

    while(char(waitKey(1)) != 'q' && cap.isOpened()){
        cap >> frame;
        if(frame.empty()){
            cout << "Video over" << endl;
            break;
        }

        fgMaskMog.copyTo(fgMaskMogPre);
        pMOG2->apply(frame, fgMaskMog);

        if(compareArea(fgMaskMog, fgMaskMogPre,
                    URbegY, URendY,
                    URbegX, URendX)){
            for(int y = URbegY; y < URendY; y++)
                for(int x = URbegX; x < URendX; x++)
                    frame.at<Vec3b>(y, x)[0] = 255;
            birdie.fly();
        }

        if(compareArea(fgMaskMog, fgMaskMogPre,
                    ULbegY, ULendY,
                    ULbegX, ULendX)){
            for(int y = ULbegY; y < ULendY; y++)
                for(int x = ULbegX; x < ULendX; x++)
                    frame.at<Vec3b>(y, x)[0] = 255;
            birdie.fly();
        }

        rectangle(
                frame,
                Point(URbegX, URbegY),
                Point(URendX, URendY),
                Scalar(255, 255, 255)
                );
        rectangle(
                frame,
                Point(ULbegX, ULbegY),
                Point(ULendX, ULendY),
                Scalar(255, 255, 255)
                );

        birdie.move(frame);
        flip(frame, frame, 1);
        imshow("Video", frame);
        //imshow("Mask",  fgMaskMog);
    }
    return 0;
}
