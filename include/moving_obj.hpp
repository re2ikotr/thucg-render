#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include <vecmath.h>
#include <cmath>
#include "object3d.hpp"
#include "bound.hpp"

class MovingObj : public Object3D {
public:
    MovingObj() {}
    MovingObj(
        const Vector3f &cen0, const Vector3f &cen1, float t0, float t1, float r, Material* m
    ): Object3D(m), center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r)
    {};

    bool intersect(const Ray& r, Hit& h, float tmin) override {
        
        Vector3f dim = r.getOrigin() - center(r.time_count);
        float dsquare = r.getDirection().squaredLength();
        float half_b = Vector3f::dot(dim, r.getDirection());
        float c = dim.squaredLength() - radius*radius;

        float discriminant = half_b*half_b - dsquare*c;
        if (discriminant < 0) 
            return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / dsquare;
        if (root < tmin ) {
            root = (-half_b + sqrtd) / dsquare;
            if (root < tmin)
                return false;
        }
        Vector3f hit_point=r.pointAtParameter(root);
        Vector3f norm=(hit_point-center(r.time_count))/radius;
        h.set(root, material, norm);
        return true;
    }

    Vector3f center(float time) const {
        return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
    }
    bool getBound(Bound& bound)
    {
        return false; //not apply bound to moving object
    }
    

protected:
    Vector3f center0, center1; //center is center0 at time0; center is center1 at time1
    float time0, time1;
    float radius;
    
};

#endif // MOVING_SPHERE_H