#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

//xyrectangle;transform to other rectangles

class rectangle : public Object3D {
public:
    rectangle() {
        
    }

    rectangle(float x0, float y0, float x1, float y1, float d, float n, Material* mat): Object3D(mat), x0(x0), y0(y0), x1(x1), y1(y1), d(d), n(n) {}
      

    ~rectangle() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        //final version edited
        auto t = (d-r.getOrigin().z()) / r.getDirection().z();

        if (t < tmin || t > h.getT())
            return false;
        auto x = r.getOrigin().x() + t*r.getDirection().x();
        auto y = r.getOrigin().y() + t*r.getDirection().y();
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return false;
        h.set(t, material, n*Vector3f(0,0,1),(x-x0)/(x1-x0), (y-y0)/(y1-y0));
        return true;
    }
    

    bool getBound(Bound& bound) override 
    {
        // bound = Bound(center - Vector3f(radius, radius, radius), center + Vector3f(radius, radius, radius));
        return false;
    }

protected:
    float x0, y0, x1, y1, d, n;
};


#endif