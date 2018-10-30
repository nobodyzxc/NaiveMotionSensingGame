#include "Bird.h"
#include <cstdio>

Bird::Bird(){
    time = 0;
    speed = 0;
    height = 300;
}

void Bird::fly(){
    time = 4;
    speed = 5 * time;
}

void Bird::move(Mat &map){
    if(speed > 0){
        if(height - time > 0)
            time -= 0.2, height -= time, speed--;
        else
            speed = 0, time = 0;
    }
    else{
        if(height < map.rows){
            time += 0.2, height += time;
        }
        else{
            time = (int)(time / 2), speed = time * 5;
        }
    }
    circle(map,
           Point(map.cols / 2, height),
           10, Scalar(0, 0, 255), -1);
}
