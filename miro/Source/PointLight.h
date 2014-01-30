#ifndef CSE168_POINTLIGHT_H_INCLUDED
#define CSE168_POINTLIGHT_H_INCLUDED

#include <vector>
#include "Vector3.h"
#include "Ray.h"
#include "Utils.h"

class PointLight
{
public:
    void setPosition(const Vector3& v)  {m_position = v;}
    void setColor(const Vector3& v)     {m_color = v;}
    void setWattage(float f)            {m_wattage = f;}
    void setRadius(float f)             {m_radius = f;}

    inline bool
    intersect(HitInfo& result, const Ray& ray,
                      float tMin = 0.0f, float tMax = MIRO_TMAX)
    {
        const Vector3 toO = ray.o - m_position; 

        const float a = ray.d.length2(); 
        const float b = dot(2*ray.d, toO);
        const float c = toO.length2() - m_radius*m_radius;

        const float discrim = b*b-4.0f*a*c; 

        if (discrim < 0) 
            return false;   // quadratic equation would yield imaginary numbers

        const float sqrt_discrim = sqrt(discrim); 

        // solve the quadratic equation
        const float t[2] = {(-b-sqrt_discrim)/(2.0f*a), (-b+sqrt_discrim)/(2.0f*a)}; 

        // since we know that discrim >= 0, t[0] < t{1]
        // return the t closest to us that is within range
        if ((t[0] > tMin) && (t[0] < tMax))
        {
            result.t = t[0];
        }
        else if((t[1] > tMin) && (t[1] < tMax))
        {
            result.t = t[1];
        }
        else
        {
            // neither of the solutions are in the required range
            return false; 
        }

        result.P = ray.o + result.t*ray.d; 
        result.N = (result.P-m_position); 
        result.N.normalize(); 

        return true;
    }


	inline Vector3 randomPointonLight(const Vector3& point) {
		Vector3 normal = point - m_position;
        normal.normalize();
		Vector3 random = generateRandomRayDirection(normal) * m_radius;
        return random;
	}

    inline Ray RayFromPointToLight(const Vector3& point) {
        Ray ray(point, (randomPointonLight(point) - point).normalize());
        return ray;
    }
        


    float wattage() const               {return m_wattage;}
    float radius() const                {return m_radius;}
    const Vector3 & color() const       {return m_color;}
    const Vector3& position() const     {return m_position;}

    void preCalc() {} // use this if you need to

protected:
    Vector3 m_position;
    Vector3 m_color;
    float m_wattage;
    float m_radius;
};

typedef std::vector<PointLight*> Lights;

#endif // CSE168_POINTLIGHT_H_INCLUDED
