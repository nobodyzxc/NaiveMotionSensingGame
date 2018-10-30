#ifndef _BIRD_H_
#define _BIRD_H_

#include <opencv2/opencv.hpp>

using namespace cv;

class Bird{
    public:
        Bird();
        void fly();
        void move(Mat &map);
    private:
        float time;
        float speed;
        float height;
};
#endif
