#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include "ctime"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include <random>
#include <cstdlib>
static std::mt19937 mersenneTwister1;
static std::uniform_real_distribution<float> uniform1;
#define RND3 (uniform1(mersenneTwister1))


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }
    inline double random_double() {
        // Returns a random real in [0,1).
        srand((unsigned int)time(0));
        return rand() / (RAND_MAX + 1.0);
    }

    inline double random_double(double min, double max) {
        // Returns a random real in [min,max).
        return min + (max-min)*RND3;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    float len_radius;
    Vector3f focus_plain;
    Vector3f plain_w;
    Vector3f plain_h;
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle, float len_r, float focus,float t1=0.0, float t2=0.0) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fx = fy = imgH / (2 * tan(angle / 2));
        time_start=t1;
        time_end=t2;
        float h = tan(angle/2.0);
        float w = h*((float)imgW/(float)imgH);
        len_radius = len_r;
        focus_plain = center - h*focus**this->up.normalized()-w*focus*this->horizontal.normalized()+focus*this->direction.normalized();
        plain_w = 2*w*focus*this->horizontal;
        plain_h = 2*h*focus*this->up;
    }

    Ray generateRay(const Vector2f &point) override {
        // 
        Vector3f random = len_radius * getRandom2();
        Vector3f offset = horizontal * random.x() + up * random.y();
        Vector3f dRc = Vector3f((point.x() - getWidth() / 2) / fx, (point.y() - getHeight() / 2) / fy, 1).normalized();
        Matrix3f R = Matrix3f(horizontal, up, direction, true);
        Ray result= Ray(center, R * dRc, random_double(time_start, time_end)); 
        if(len_radius>=99990)
        {
            result= Ray(center, R * dRc, random_double(time_start, time_end));
        }
        else
        {
            result = Ray(this->center + offset, this->focus_plain + float(point.x())/float(this->width) * this->plain_w + float(point.y())/float(this->height) * this->plain_h - this->center - offset);
        }
        result.time_count = random_double(time_start, time_end);
        return result;
    }
    Vector3f getRandom2()
    {
        Vector3f p;
        do {
            p = 2.0 * Vector3f(drand48(), drand48(), 0) - Vector3f(1,1,0);
        } while (Vector3f::dot(p, p) >= 1.0);
        return p;
    }

protected:
    float fx, fy;
    float time_start;
    float time_end;
};
#endif //CAMERA_H
