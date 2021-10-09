/********************************************************************************
* OpenGL-Framework                                                              *
* Copyright (c) 2013 Daniel Chappuis                                            *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/
#ifndef VECTOR3_H
#define VECTOR3_H

// Needed for sqrt
#include <cmath>


// An easy 3d vector class to make life easier
// Slightly modified from
// https://github.com/DanielChappuis/opengl-framework/blob/master/src/maths/Vector3.h
// Equality operators now check pointers, and an inline addition operator has been
// removed because it gave me compiler errors.
// Namespace has also been removed.
class Vector3 {
    public:
        float x;
        float y;
        float z;
        // Constructor
        Vector3(float px=0.0, float py=0.0, float pz=0.0){
            x = px;
            y = py;
            z = pz; }
        // Deconstructor
        ~Vector3() {}
        // Addition operator
        Vector3 operator+(const Vector3 &other)
        {   return Vector3(x+other.x, y+other.y, z+other.z);    }
        // Subtraction operator
        Vector3 operator-(const Vector3 &other)
        {   return Vector3(x-other.x, y-other.y, z-other.z);    }
        // += operator
        Vector3& operator+=(const Vector3 &other) 
        {   x += other.x; y += other.y; z += other.z;
            return *this;   }
        // -= operator
        Vector3 operator-=(const Vector3 &other)
        {   x -= other.x; y -= other.y; z -= other.z;
            return *this;   }
        // Multiplication operators
        Vector3 operator*(const float scalar)
        {   return Vector3(scalar*x, scalar*y, scalar*z);   }
        // Division operator
        Vector3 operator/(const float scalar)
        {   return Vector3(x/scalar, y/scalar, z/scalar);   }
        // *= operator
        Vector3 &operator*=(const float scalar)
        {   x*=scalar; y*=scalar; z*=scalar;
            return *this;   }
        // /= operator
        // The original class uses C asserts and limits to make sure that this won't
        // divide by 0 or overflow. I'll live without that
        Vector3 &operator/=(const float scalar)
        {   x/=scalar; y/=scalar; z/=scalar;
            return *this;   }
        // Negation operator
        Vector3 operator-()
        {   return Vector3(-x, -y, -z); }
        // Cross product
        Vector3 cross(const Vector3 &other)
        {   return Vector3(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x);    }
        // Dot product
        Vector3 dot(const Vector3 &other)
        {   return x*other.x + y*other.y + z*other.z;   }
        // Equality operator
        // Checks by reference, because checking floats by value is pointless
        bool operator==(const Vector3 &other)
        {   return *this==other;  }
        // Inequality operator
        // Checks by reference, because checking floats by value is pointless
        bool operator!=(const Vector3 &other)
        {   return !(*this==other);  }
        float length()
        {   return std::sqrt(x*x + y*y + z*z);  }
        Vector3 normalize()
        {   float l = length();
            x/=l;
            y/=l;
            z/=l;
            return *this;   }
        Vector3 normalized()
        {   float l = length();
            return Vector3(x/l, y/l, z/l);  }
};

//I don't know why this isn't working to be honest
//But it's just for statements like 2*vector, as opposed to vector*2
//So I'll live without for now
/*
inline Vector3 operator*(float f, const Vector3& o) {
  return o*f;
}
*/

#endif