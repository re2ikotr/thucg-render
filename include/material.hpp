#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.h"
#include <iostream>

enum material_type {DIFF, SPEC, REFR, LIGHT};

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0, const Vector3f &_attenuation=Vector3f::ZERO, float ref = 0, material_type _type=DIFF, const char* texture_name = "", bool hastex = false) :
            diffuseColor(d_color), specularColor(s_color), shininess(s), attenuation(_attenuation), refractive(ref), type(_type), hasTexture(hastex)
    {
        texture = Texture(texture_name);
    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }
    virtual Vector3f getSpecularColor() const {
        return specularColor;
    }
    virtual float getShininess() const {
        return shininess;
    }
    virtual Vector3f getAttenuation(float u = 0.0, float v = 0.0) {
        if(hasTexture)
        {
            return texture.ColorAt(u, v);
        }
        return attenuation;
    }
    virtual float getRefractive() const {
        return refractive;
    }
    virtual material_type getType() const {
        return type;
    }
    virtual Texture getTexture() const {
        return texture;
    }

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        // 
        Vector3f V = -ray.getDirection();
        Vector3f R = 2 * Vector3f::dot(hit.getNormal(), dirToLight) * hit.getNormal() - dirToLight;
        if(Vector3f::dot(dirToLight, hit.getNormal()) > 0)
            shaded += lightColor * diffuseColor * (Vector3f::dot(dirToLight, hit.getNormal()));
        if(Vector3f::dot(V, R) > 0)
            shaded += lightColor * specularColor * pow(Vector3f::dot(V, R), shininess);
        Vector3f shadingpoint = ray.getOrigin() + hit.getT() * ray.getDirection();
        return shaded;
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
    Vector3f attenuation;
    float refractive;
    material_type type;
    Texture texture;
    bool hasTexture;
};


#endif // MATERIAL_H
