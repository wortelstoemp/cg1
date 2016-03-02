#pragma once
#include <cmath>
#include "vec3.h"
#include "vec4.h"

template<typename T>
struct Mat4x4
{
    T m[16];
    
    Mat4x4(T d = 1)
    {
        this->m = 
        {
            d, 0, 0, 0,
            0, d, 0, 0,
            0, 0, d, 0,
            0, 0, 0, d,                        
        };
    }
    
    void LoadIdentity()
    {
        this->m = 
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,                        
        }; 
    }
    
    void Translate(T tx, T ty, T tz)
    {
        Mat4x4 translation;
        translation.m = 
        {
            1, 0, 0, tx,
            0, 1, 0, ty,
            0, 0, 1, tz,
            0, 0, 0, 1,
        };
        
        *this = translation * (*this);
    }
    
    void Scale(T sx, T sy, T sz)
    {
        Mat4x4 scale;
        scale.m = 
        {
            sx, 0, 0, 0,
            0, sx, 0, 0,
            0, 0, sx, 0,
            0, 0, 0, 1,
        };
        
        *this = scale * (*this);
    }
    
    void RotateX(T angle)
    {
        Mat4x4 rotation;
        const T cost = (T) cos(angle);
        const T sint = (T) sin(angle);
        
        rotation.m = 
        {
            1,  0,      0,      0,
            0,  cost,   -sint,  0,
            0,  sint,   cost,   0,
            0,  0,      0,      1,
        };
        
        *this = rotation * (*this);
    }
    
    void RotateY(T angle)
    {
        Mat4x4 rotation;
        const T cost = (T) cos(angle);
        const T sint = (T) sin(angle);
        
        rotation.m = 
        {
            cost,   0,      sint,   0,
            0,      1,      0,      0,
            -sint,  0,      cost,   0,
            0,      0,      0,      1,
        };
        
        *this = rotation * (*this);
    }
    
    void RotateZ(T angle)
    {
        Mat4x4 rotation;
        const T cost = (T) cos(angle);
        const T sint = (T) sin(angle);
        
        rotation.m = 
        {
            cost,   -sint,  0,  0,
            sint,   cost,   0,  0,
            0,      0,      1,  0,
            0,      0,      0,  1,
        };
        
        *this = rotation * (*this);
    }
    
    Mat4x4 operator*(const Mat4x4& rhs) const
    {
        Mat4x4 r;
        r.m = 
        {
            // Row 0
			this->m[0]*rhs.m[0] + this->m[3]*rhs.m[12] + this->m[1]*rhs.m[4] + this->m[2]*rhs.m[8],
			this->m[0]*rhs.m[1] + this->m[3]*rhs.m[13] + this->m[1]*rhs.m[5] + this->m[2]*rhs.m[9],
			this->m[2]*rhs.m[10] + this->m[3]*rhs.m[14] + this->m[0]*rhs.m[2] + this->m[1]*rhs.m[6],
			this->m[2]*rhs.m[11] + this->m[3]*rhs.m[15] + this->m[0]*rhs.m[3] + this->m[1]*rhs.m[7],

			// Row 1
			this->m[4]*rhs.m[0] + this->m[7]*rhs.m[12] + this->m[5]*rhs.m[4] + this->m[6]*rhs.m[8],
			this->m[4]*rhs.m[1] + this->m[7]*rhs.m[13] + this->m[5]*rhs.m[5] + this->m[6]*rhs.m[9],
			this->m[6]*rhs.m[10] + this->m[7]*rhs.m[14] + this->m[4]*rhs.m[2] + this->m[5]*rhs.m[6],
			this->m[6]*rhs.m[11] + this->m[7]*rhs.m[15] + this->m[4]*rhs.m[3] + this->m[5]*rhs.m[7],

			// Row 2
			this->m[8]*rhs.m[0] + this->m[11]*rhs.m[12] + this->m[9]*rhs.m[4] + this->m[10]*rhs.m[8],
			this->m[8]*rhs.m[1] + this->m[11]*rhs.m[13] + this->m[9]*rhs.m[5] + this->m[10]*rhs.m[9],
			this->m[10]*rhs.m[10] + this->m[11]*rhs.m[14] + this->m[8]*rhs.m[2] + this->m[9]*rhs.m[6],
			this->m[10]*rhs.m[11] + this->m[11]*rhs.m[15] + this->m[8]*rhs.m[3] + this->m[9]*rhs.m[7],
			
			// Row 3
			this->m[12]*rhs.m[0] + this->m[15]*rhs.m[12] + this->m[13]*rhs.m[4] + this->m[14]*rhs.m[8],
			this->m[12]*rhs.m[1] + this->m[15]*rhs.m[13] + this->m[13]*rhs.m[5] + this->m[14]*rhs.m[9],
			this->m[14]*rhs.m[10] + this->m[15]*rhs.m[14] + this->m[12]*rhs.m[2] + this->m[13]*rhs.m[6],
			this->m[14]*rhs.m[11] + this->m[15]*rhs.m[15] + this->m[12]*rhs.m[3] + this->m[13]*rhs.m[7],
        };
        
        return r;
    }
	
	Vec4 operator*(const Vec4& rhs) const
	{
		Vec4 r;
		r.x = this->m[0]*rhs.x + this->m[1]*rhs.y + this->m[2]*rhs.z + this->m[3]*rhs.w;
		r.y = this->m[4]*rhs.x + this->m[5]*rhs.y + this->m[6]*rhs.z + this->m[7]*rhs.w;
		r.z = this->m[8]*rhs.x + this->m[9]*rhs.y + this->m[10]*rhs.z + this->m[11]*rhs.w;
		r.w = this->m[12]*rhs.x + this->m[13]*rhs.y + this->m[14]*rhs.z + this->m[15]*rhs.w;
		
		return r;
	}
	
	Vec4 operator*(const Vec3& rhs) const
	{
		Vec4 r;
		r.x = this->m[0]*rhs.x + this->m[1]*rhs.y + this->m[2]*rhs.z + this->m[3];
		r.y = this->m[4]*rhs.x + this->m[5]*rhs.y + this->m[6]*rhs.z + this->m[7];
		r.z = this->m[8]*rhs.x + this->m[9]*rhs.y + this->m[10]*rhs.z + this->m[11];
		r.w = this->m[12]*rhs.x + this->m[13]*rhs.y + this->m[14]*rhs.z + this->m[15];				
		
		return r;
	}
    
    Mat4x4& operator=(const Mat4x4& m)
    {
        this->m = m.m;
		
		return *this;
    }
};