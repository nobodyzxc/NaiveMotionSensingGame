#ifndef _BIRD_H_
#define _BIRD_H_

#include <opencv2/opencv.hpp>
#define RADIUS 10

using namespace cv;

class Ball{
    public:
        Ball(int x, int y);
        void set(int x, int y);
        void move(Mat &map, int x);
        void update(Mat &map);
        int getY(){ return loc.y; }
        int getX(){ return loc.x; }
        int getR(){ return RADIUS; }
    private:
        Point loc;
};
#endif
