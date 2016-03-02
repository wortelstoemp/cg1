#pragma once

template<typename T>
struct Vec2
{
    T x, y;
    
    Vec2(T x = 0, T y = 0)
		: x(x), y(y)
	{}
};