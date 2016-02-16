#pragma once

struct Vec2
{
    float x;
    float y;
    
    Vec2() : x(0.0), y(0.0) {}
    Vec2(float x, float y): x(x), y(y) {}  
    virtual ~Vec2() {}
};