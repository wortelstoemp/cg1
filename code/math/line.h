#pragma once

struct Line
{
	int x0;
	int y0;
	int x1;
    int y1;
       
    Line(int x0 = 0, int y0 = 0, int x1 = 0, int y1 = 0)
        : x0(x0), y0(y0), x1(x1), y1(y1)
    {}
};

struct ClippedLine
{
    Line line;
    bool accepted;
    
    ClippedLine(const Line& line, bool accepted = true)
        : line(line), accepted(accepted)
    {}
};

struct Line3D
{
	int x0;
	int y0;
	int z0;
	int x1;
    int y1;
	int z1;
       
    Line3D(int x0 = 0, int y0 = 0, int z0 = 0,
		   int x1 = 0, int y1 = 0, int z1 = 0)
        : x0(x0), y0(y0), z0(z0), x1(x1), y1(y1), z1(z1)
    {}
};

struct ClippedLine3D
{
    Line3D line;
    bool accepted;
    
    ClippedLine3D(const Line3D& line, bool accepted = true)
        : line(line), accepted(accepted)
    {}
};