#include "Bar.h"
#include <cstdlib>

#define THICK 10

Bar::Bar(Mat &map, int _speed){
    y = 10;
    openw = 25 + rand() % 36;
    llong = rand() % (map.cols - 60);
    speed = _speed;
}

void Bar::move(){
    y += speed;
}

void Bar::display(Mat &map){
    rectangle(
            map,
            Point(0, y - THICK),
            Point(llong, y),
            Scalar(0, 255, 0),
            CV_FILLED
            );
    rectangle(
            map,
            Point(llong + openw, y - THICK),
            Point(map.cols, y),
            Scalar(0, 255, 0),
            CV_FILLED
            );

}

bool Bar::detect(int cx, int cy, int cr){
    if(cy - y > cr) return false;
    if(y - THICK > cy + cr) return false;
    return !(cx >= llong + cr && cx <= llong + openw - cr);
}

bool Bar::outOfBound(Mat &frame){
    return y > frame.cols - 10;
}
