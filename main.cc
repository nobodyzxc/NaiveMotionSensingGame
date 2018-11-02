#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>

#include <deque>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include <time.h>

#include "Ball.h"
#include "Bar.h"

#define COR 50

#define URbegY (height / 3 * 2 - COR)
#define URendY (height         - COR)
#define URbegX (0)
#define URendX (width  / 6 * 2)

#define ULbegY (height / 3 * 2 - COR)
#define ULendY (height         - COR)
#define ULbegX (width  / 6 * 4)
#define ULendX (width)

#define CbegY  (0)
#define CendY  (height / 6)
#define CbegX  (0)
#define CendX  (width  / 5)

#define AREA 4

using namespace cv;
using namespace std;

int state = 2;
int width = 0;
int height = 0;
int bar_count = 0;
int game_time = 0;
int barInterval = 100;
double threshold_low = 0.15;
double threshold_hei = 0.8;

Ball *ball;
deque<Bar*> barQueue;

Mat frame;
Mat fgMaskMog;
Mat fgMaskMogPre;
cv::Ptr<cv::BackgroundSubtractorMOG2> pMOG2;

double compareArea(Mat fa, Mat fb,
        int ya, int yb, int xa, int xb){

    if(fa.empty() || fb.empty())
        return false;
    int acc = 0;
    for(int y = ya; y < yb; y++)
        for(int x = xa; x < xb; x++)
            acc += fa.at<uchar>(y, x) != fb.at<uchar>(y, x);
    return double(acc) / double(abs(yb - ya) * abs(xb - xa));
}

void detect(){

#define RATE 150

    fgMaskMog.copyTo(fgMaskMogPre);
    pMOG2->apply(frame, fgMaskMog);

    double area = 0;
    static int acc = 0;

    if(state & 1){
        area = compareArea(
                fgMaskMog, fgMaskMogPre,
                URbegY, URendY,
                URbegX, URendX);
        if(area > threshold_low && area < threshold_hei){
            for(int y = URbegY; y < URendY; y++)
                for(int x = URbegX; x < URendX; x++)
                    frame.at<Vec3b>(y, x)[0] = 255;
            ball->move(frame, -int(RATE * area * area));
        }

        area = compareArea(
                fgMaskMog, fgMaskMogPre,
                ULbegY, ULendY,
                ULbegX, ULendX);
        if(area > threshold_low && area < threshold_hei){
            for(int y = ULbegY; y < ULendY; y++)
                for(int x = ULbegX; x < ULendX; x++)
                    frame.at<Vec3b>(y, x)[0] = 255;
            ball->move(frame, int(RATE * area * area));
        }
    }
    int speed[3] = {25, 10, 25};
    area = compareArea(
            fgMaskMog, fgMaskMogPre,
            CbegY, CendY,
            CbegX, CendX);
    acc = max(acc - 1, 0);
    if(area > threshold_low && area < threshold_hei){
        if(acc >= CendX){
            if(state == 2){
                game_time = 0;
                bar_count = 0;
                for(auto i = barQueue.begin();
                        i != barQueue.end(); i++)
                    delete (*i);
                barQueue.clear();
            }
            acc = 0, state = abs(state - 1);
        }
        else acc += CendX / speed[state];
    }
    for(int y = CbegY; y < CendY; y++)
        for(int x = CbegX;
                x < min(CendX, acc); x++)
            frame.at<Vec3b>(y, x)[0] = 255;
}

void drawUI(){

    if(state & 1){
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
    }
    if(state < 2){
        ball->update(frame);
    }
    rectangle(
            frame,
            Point(CbegX, CbegY),
            Point(CendX, CendY),
            Scalar(255, 255, 255)
            );

    string msg[3] = {
        string("cont"),
        string("stop"),
        string("start")
    };
    putText(frame, msg[state],
            Point(CendX / 5, CendY / 2),
            0, 1, Scalar(0,255,255), 3);
}

void gameOver(){
    state = 2;
    ball->set(width / 2, height / 5 * 4);
}

int main(){

    VideoCapture cap(0);
    pMOG2 = cv::createBackgroundSubtractorMOG2();

    if(!cap.isOpened())
        cout << "cam not opened" << endl;
    cvNamedWindow("Video");
    //cvNamedWindow("Mask");

    width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    ball = new Ball(width / 2, height / 5 * 4);
    srand(time(NULL));

    char key;
    while((key = char(waitKey(1))) != 'q' && cap.isOpened()){
        cap >> frame;
        if(frame.empty()){
            cout << "Video over" << endl;
            break;
        }

        flip(frame, frame, 1);
        detect();


        if(key == 'h')
            ball->move(frame, -10);
        else if(key == 'l')
            ball->move(frame, 10);
        else if(key == ' ' && state == 2){
            game_time = 0;
            bar_count = 0;
            for(auto i = barQueue.begin();
                    i != barQueue.end(); i++)
                delete (*i);
            barQueue.clear();
            state -= 1;
        }

        drawUI();

        if(state & 1){

            if(!barQueue.empty()){
                for(auto i = barQueue.begin();
                        i != barQueue.end(); i++){
                    (*i)->move();
                    if((*i)->detect(ball->getX(),
                                ball->getY(),
                                ball->getR()))
                        gameOver();
                }
                if(barQueue.front()->outOfBound(frame)){
                    delete barQueue.front();
                    barQueue.pop_front();
                    bar_count += 1;
                }
            }
            if(barQueue.empty() ||
                    barQueue.back()->y > height * 4 / 5 * max(0.3, 1 - double(bar_count) / 40)){
                Bar *bar = new Bar(frame, bar_count);
                barQueue.push_back(bar);
            }
            game_time += 1;
        }

        if(!barQueue.empty()){
            for(auto i = barQueue.begin();
                    i != barQueue.end(); i++){
                (*i)->display(frame);
            }
        }
        putText(frame, to_string(game_time / 20),
                Point(width / 2, height / 2),
                0, 1, Scalar(0,255,255), 3);

        imshow("Video", frame);

        //imshow("Mask",  fgMaskMog);
    }
    return 0;
}
