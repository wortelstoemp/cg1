#pragma once

struct Line
{
    int x0;
    int y0;
    int x1;
    int y1;
       
    Line() : x0(0), y0(0), x1(0), y1(0) {}
    Line(int x0, int y0, int x1, int y1) : x0(x0), y0(y0), x1(x1), y1(y1) {}
    virtual ~Line() {} 
};