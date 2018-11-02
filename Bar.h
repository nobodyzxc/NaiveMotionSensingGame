#ifndef _BAR_H_
#define _BAR_H_

#include <opencv2/opencv.hpp>

using namespace cv;

class Bar{
    public:
        Bar(Mat &map, int _speed);
        void move();
        void display(Mat &map);
        bool detect(int x, int y, int r);
        bool outOfBound(Mat &frame);
        int speed;
        int y;
    private:
        int openw;
        int llong;
};
#endif
