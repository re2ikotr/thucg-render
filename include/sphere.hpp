#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = Vector3f(0.0, 0.0, 0.0);
        radius = 1.0;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material), center(center), radius(radius){
        // 
        can_be_bounded = true;
       
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        //
        Vector3f origin = r.getOrigin();
        Vector3f direction = r.getDirection();
        
        float a = direction.squaredLength();
        float b = 2 * (direction.x() * (origin.x() - center.x()) + direction.y() * (origin.y() - center.y()) + direction.z() * (origin.z() - center.z()));
        float c = (origin - center).squaredLength() - radius * radius;

        float delta = b * b - 4 * a * c;
        if(delta >= 0)
        {
            float t = (-b - sqrt(delta)) / (2 * a);
            if(t > tmin && t < h.getT())
            {
                //the direction of normal vector, positve or negtive?
                Vector3f hit_point = origin + t * direction;
                Vector3f normal = hit_point - center;
                float u = atan2(-normal.z(), normal.x()) / (2 * M_PI) + 0.5f, v = acos(-normal.y()) / M_PI;
                h.set(t, material, normal.normalized(), u, v);
                h.position = r.pointAtParameter(t);
                if(Vector3f::dot(normal, r.getDirection()) <= 0)
                    h.is_front_face = true;
                else
                    h.is_front_face = false;
                return true;
            }
        }
        return false;
    }
    Ray generateRay() override {
        Vector3f ret;
        do {
            ret = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
        }   while(ret.squaredLength() >= 1.0);
        ret.normalize();
        Vector3f ret2;
        do {
            ret2 = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
        }   while(ret2.squaredLength() >= 1.0);

        return Ray(center+radius*ret, (ret2 + ret).normalized());
    }

    bool getBound(Bound& bound) override 
    {
        bound = Bound(center - Vector3f(radius, radius, radius), center + Vector3f(radius, radius, radius));
        return true;
    }

protected:
    Vector3f center;
    float radius;
};


#endif