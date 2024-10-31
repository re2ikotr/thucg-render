#ifndef BOUND_H
#define BOUND_H

#include <vecmath.h>
#include <vector>
#include "ray.hpp"

// class Bound {
// public:
//     Vector3f bounds[2];
//     Vector3f center;


//     Bound() {
//         bounds[0] = Vector3f(1e100);
//         bounds[1] = Vector3f(-1e100);
//         center = ((bounds[0] + bounds[1]) / 2);
//     }

//     Bound(const Vector3f &mn, const Vector3f &mx) {
//         bounds[0] = mn; // min
//         bounds[1] = mx; // max
//         center = ((bounds[0] + bounds[1]) / 2);
//     }

//     void set(const Vector3f &mn, const Vector3f &mx) {
//         bounds[0] = mn;
//         bounds[1] = mx;
//         center = ((bounds[0] + bounds[1]) / 2);
//     }

//     Vector3f getCenter() const {
//         return center;
//     }

//     void updateBound(const Vector3f &v) {
//         bounds[0] = std::min(bounds[0], v);
//         bounds[1] = std::max(bounds[1], v);
//         center = (bounds[0] + bounds[1]) / 2;
//     }

//     bool intersect(const Ray &r, float &t_min) {
//         Vector3f o(r.getOrigin()), invdir(1 / r.getDirection());
//         std::vector<int> sign = {invdir.x() < 0, invdir.y() < 0, invdir.z() < 0};
//         t_min = 1e100;
//         double tmin, tmax, tymin, tymax, tzmin, tzmax;
//         tmin = (bounds[sign[0]].x() - o.x()) * invdir.x();
//         tmax = (bounds[1 - sign[0]].x() - o.x()) * invdir.x();
//         tymin = (bounds[sign[1]].y() - o.y()) * invdir.y();
//         tymax = (bounds[1 - sign[1]].y() - o.y()) * invdir.y();
//         if ((tmin > tymax) || (tmax < tymin)) return false;
//         tmin = std::max(tymin, tmin);
//         tmax = std::min(tymax, tmax);
//         tzmin = (bounds[sign[2]].z() - o.z()) * invdir.z();
//         tzmax = (bounds[1 - sign[2]].z() - o.z()) * invdir.z();
//         if ((tmin > tzmax) || (tmax < tzmin)) return false;
//         tmin = std::max(tzmin, tmin);
//         tmax = std::min(tzmax, tmax);
//         t_min = tmin;
//         return true;
//     }
//     friend Bound merge(Bound a, Bound b)
//     {
//         Vector3f bounds[0](fmin(a.bounds[0].x(), b.bounds[0].x()), fmin(a.bounds[0].y(), b.bounds[0].y()), fmin(a.bounds[0].z(), b.bounds[0].z()));
//         Vector3f bounds[1](fmin(a.bounds[1].x(), b.bounds[1].x()), fmin(a.bounds[1].y(), b.bounds[1].y()), fmin(a.bounds[1].z(), b.bounds[1].z()));
//         return Bound(bounds[0], bounds[1]);
//     }
// };

class Bound {
public:
    Vector3f bounds[2];
    Vector3f center;


    Bound() {
        bounds[0] = Vector3f(1e100);
        bounds[1] = Vector3f(-1e100);
        center = ((bounds[0] + bounds[1]) / 2);
    }

    Bound(const Vector3f &mn, const Vector3f &mx) {
        bounds[0] = mn; // min
        bounds[1] = mx; // max
        center = ((bounds[0] + bounds[1]) / 2);
    }

    void set(const Vector3f &mn, const Vector3f &mx) {
        bounds[0] = mn;
        bounds[1] = mx;
        center = ((bounds[0] + bounds[1]) / 2);
    }

    Vector3f getCenter() const {
        return center;
    }

    void updateBound(const Vector3f &v) {
        bounds[0] = std::min(bounds[0], v);
        bounds[1] = std::max(bounds[1], v);
        center = (bounds[0] + bounds[1]) / 2;
    }

    bool intersect(const Ray &r, float t_min, float t_max = 0x3f3f3f3f) {
        float inv = 1.0f / r.getDirection().x();
            float eps = 1e-3;
            if (std::isinf(inv)) return true;
            float t0 = (bounds[0].x() - r.getOrigin().x()) * inv;
            float t1 = (bounds[1].x() - r.getOrigin().x()) * inv;
            if (inv < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max < t_min) 
                return false;

            inv = 1.0f / r.getDirection().y();
            if (std::isinf(inv)) return true;
            t0 = (bounds[0].y() - r.getOrigin().y()) * inv;
            t1 = (bounds[1].y() - r.getOrigin().y()) * inv;
            if (inv < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max < t_min) 
                return false;
            
            inv = 1.0f / r.getDirection().z();
            if (std::acoshf(inv)) return true;
            t0 = (bounds[0].z() - r.getOrigin().z()) * inv;
            t1 = (bounds[1].z() - r.getOrigin().z()) * inv;
            if (inv < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max < t_min) 
                return false;
            //if (t_min < tmin) return false;
            return true;
    }
    friend Bound merge(Bound a, Bound b)
    {
        Vector3f min__point(fmin(a.bounds[0].x(), b.bounds[0].x()), fmin(a.bounds[0].y(), b.bounds[0].y()), fmin(a.bounds[0].z(), b.bounds[0].z()));
        Vector3f max__point(fmin(a.bounds[1].x(), b.bounds[1].x()), fmin(a.bounds[1].y(), b.bounds[1].y()), fmin(a.bounds[1].z(), b.bounds[1].z()));
        return Bound(min__point, max__point);
    }
};

#endif //BOUND_H