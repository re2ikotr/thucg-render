#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        t = 1e38;
        flux = fluxLight = Vector3f::ZERO;
        attenWeight = Vector3f(1.0);
        radius = 0.0005;
        num_photon = 0;
    }

    Hit(float _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
        flux = fluxLight = Vector3f::ZERO;
        attenWeight = Vector3f(1.0);
        radius = 0.0005;
        num_photon = 0;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
    }

    // destructor
    ~Hit() = default;

    float getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    void set(float _t, Material *m, const Vector3f &n, float _u =0.0, float _v=0.0, bool face=true) {
        t = _t;
        material = m;
        normal = n;
        is_front_face = face;
        u=_u;
        v=_v;
    }
    void setT(float _t)
    {
        t=_t;
    }

    float t;
    Material *material;
    Vector3f normal;
    Vector3f flux, fluxLight, attenWeight;
    Vector3f position;
    float radius;
    int num_photon;
    bool is_front_face;
    float u,v;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
