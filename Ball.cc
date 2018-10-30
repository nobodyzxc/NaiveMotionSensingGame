#include "Ball.h"
#include <cstdio>

Ball::Ball(int x, int y){
    loc.x = x, loc.y = y;
}

void Ball::set(int x, int y){
    loc.x = x, loc.y = y;
}

void Ball::move(Mat &map, int x){
    int nx = loc.x + x;
    if(nx > 0 && nx < map.cols)
        loc.x += x;
}

void Ball::update(Mat &map){
    circle(map, loc,
           RADIUS, Scalar(0, 0, 255), -1);
}
