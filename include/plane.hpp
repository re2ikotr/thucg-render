#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Vector3f udim, vdim;
    Plane() {

    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m), normal(normal), d(d) {
        can_be_bounded = false;
        if (Vector3f::cross(this->normal.normalized(), Vector3f::UP).length() < 1e-6)
            this->vdim = Vector3f::FORWARD;
        else this->vdim = Vector3f::UP;
        this->udim = Vector3f::cross(this->normal.normalized(), this->vdim).normalized();
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f origin = r.getOrigin();
        Vector3f direction = r.getDirection();
        float k = Vector3f::dot(direction, normal);
        float RHS = d - Vector3f::dot(origin, normal);
        if(k > 1e-9 || k <-1e-9)
        {
            float t = RHS / k;
            Vector3f normal_result = normal;
            if(Vector3f::dot(normal_result, direction) > 0)
                normal_result = -normal;
            if(t > tmin && t < h.getT())
            {
                Vector3f point = r.pointAtParameter(t);
		        float v = Vector3f::dot(point, vdim);
                float u = Vector3f::dot(point, udim);
                h.set(t, material, normal_result.normalized(), u, v);
                h.position = r.pointAtParameter(t);
                return true;
            }
        }
        return false;
    }
    bool getBound(Bound& bound) override 
    {
        Vector3f norm = normal.normalized();
        Vector3f max_point = float(10000) * Vector3f(norm.x() < 1 - 1e-6, norm.y() < 1 - 1e-6, norm.z() < 1 - 1e-6) + d * norm;
        Vector3f min_point =  -float(10000) * Vector3f(norm.x() < 1 - 1e-6, norm.y() < 1 - 1e-6, norm.z() < 1 - 1e-6) + d * norm;
        bound = Bound(min_point, max_point);
        return true;
    }


protected:
    Vector3f normal;
    float d;
};

#endif //PLANE_H
		

