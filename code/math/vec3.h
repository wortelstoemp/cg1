#pragma once
#include <cmath>

template<typename T>
struct Vec3
{
    T x, y, z;   
    
    Vec3(T x = 0, T y = 0, T z = 0)
        : x(x), y(y), z(z)
    {}
    
    static T Dot(const Vec3& v1, const Vec3& v2)
    {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	static Vec3 Cross(const Vec3& v1, const Vec3& v2)
    {
        Vec3 r;
        r.x = (v1.y * v2.z) - (v1.z * v2.y);
        r.y = (v1.z * v2.x) - (v1.x * v2.z);
        r.z = (v1.x * v2.y) - (v1.y * v2.x);
        
        return r;
	}
    
    T Length() const
    {
		return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	T LengthSquared() const
    {
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}
    
    Vec3 Normalized() const
    {
		const T inverseLength = 1.0 / this->Length();
        
        Vec3 r;
        r.x = x * inverseLength;
        r.y = y * inverseLength;
        r.z = z * inverseLength;
        
        return r;
	}
    
    static T Distance(const Vec3& v1, const Vec3& v2)
    {
        return (v1 - v2).Length();
    }
    
    static T DistanceSquared(const Vec3& v1, const Vec3& v2)
    {
        return (v1 - v2).LengthSquared();
    }
    
    static Vec3 Lerp(const Vec3& v1, const Vec3& v2, T amount)
    {
        const T diff = 1 - amount;
        
        Vec3 r;
        r.x = diff * v1.x + amount * v2.x;
        r.y = diff * v1.y + amount * v2.y;
        r.z = diff * v1.z + amount * v2.z;
        
        return r;
    }
    
    Vec3 Rotated(const Vec3& axis, T angle)
    {
        // Rodrigues' Rotation Formula
        // v(rot) = v cos(t) + (axis X v) sin(t) + axis ( axis . v ) (1 - cos(t))
		// v(rot) = a + b + c
        
        const T t = angle * M_PI / 180;
		const T sint = sin(t);
		const T cost = cos(t);

		// a = v cos(t)
		const T ax = this->x * cost;
		const T ay = this->y * cost;
		const T az = this->z * cost;

		// b = (axis X v) sin(t)
		const T bx = ((axis.y * this->z) - (axis.z * this->y)) * sint;
		const T by = ((axis.z * this->x) - (axis.x * this->z)) * sint;
		const T bz = ((axis.x * this->y) - (axis.y * this->x)) * sint;
        
		// c = axis ( axis . v ) (1 - cos(t))
		const T scale = Dot(*this, axis) * (1 - cost);
		const T cx = axis.x * scale;
		const T cy = axis.y * scale;
		const T cz = axis.z * scale;
        
        // v(rot) = a + b + c
        Vec3 r;
        r.x = ax + bx + cx;
        r.y = ay + by + cy;
        r.z = az + bz + cz;
        
        return r;
    }
    
    
    Vec3& operator=(const Vec3& v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
		
		return *this;        
    }
    
    Vec3 operator-() const
    {
        Vec3 r;
        r.x = -this->x;
        r.y = -this->y;
        r.z = -this->z;   
        
        return r;             
    }
    
    Vec3 operator+(const Vec3& rhs) const
    {
        Vec3 r;
        r.x = this->x + rhs.x;
        r.y = this->y + rhs.y;
        r.z = this->z + rhs.z;
        
        return r;        
    }
    
    Vec3 operator-(const Vec3& rhs) const
    {
        Vec3 r;
        r.x = this->x - rhs.x;
        r.y = this->y - rhs.y;
        r.z = this->z - rhs.z;
        
        return r;        
    }
    
    void operator+=(const Vec3& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;        
    }
    
    Vec3& operator++()
    {
        ++this->x;
        ++this->y;
        ++this->z;
        
        return *this;  
    }
    
    Vec3 operator++(const int dummy)
    {
        Vec3 previous = *this;
        
        this->x++;
        this->y++;
        this->z++;
        
        return previous;
    }
    
	Vec3& operator--()
    {
        --this->x;
        --this->y;
        --this->z;
        
        return *this;  
    }
    
    Vec3 operator--(const int dummy)
    {
        Vec3 previous = *this;
        
        this->x--;
        this->y--;
        this->z--;
        
        return previous;
    }
     
    void operator-=(const Vec3& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;        
    }
    
    Vec3 operator*(const T rhs) const
    {
        Vec3 r;
        r.x = this->x * rhs;
        r.y = this->y * rhs;
        r.z = this->z * rhs;
        
        return r;       
    }
    
    friend Vec3 operator*(const T lhs, const Vec3& rhs)
    {
        Vec3 r;
        r.x = lhs * rhs.x;
        r.y = lhs * rhs.y;
        r.z = lhs * rhs.z;
        
        return r;  
    }
    
    void operator*=(const T rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;        
    }
    
    Vec3 operator/(const T rhs) const
    {
        Vec3 r;
        r.x = this->x / rhs;
        r.y = this->y / rhs;
        r.z = this->z / rhs;
        
        return r;       
    }
    
    friend Vec3 operator/(const T lhs, const Vec3& rhs)
    {
        Vec3 r;
        r.x = lhs / rhs.x;
        r.y = lhs / rhs.y;
        r.z = lhs / rhs.z;
        
        return r;  
    }
    
    
    void operator/=(const T rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;        
    }
    
    bool operator==(const Vec3& rhs) const
    {
        return this->x == rhs.x &&
               this->y == rhs.y &&
               this->z == rhs.z;
    }
    
    bool operator!=(const Vec3& rhs) const
    {
        return this->x != rhs.x ||
               this->y != rhs.y ||
               this->z != rhs.z;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v)
    {
        os << '[' << v.x
           << ", " << v.y
           << ", " << v.z
           << ']';
           
       return os;
    }
};