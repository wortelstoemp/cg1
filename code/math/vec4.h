#pragma once
#include <cmath>
#include "vec3.h"

template<typename T>
struct Vec4
{
    T x, y, z, w;   
    
    Vec4(T x = 0, T y = 0, T z = 0, T w = 1)
        : x(x), y(y), z(z), w(w)
    {}
	
	Vec4(const Vec3& v)
        : x(v.x), y(v.y), z(v.z), w(1)
    {}
};