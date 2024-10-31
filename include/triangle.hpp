#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle : public Object3D
{

public:
	Triangle() = delete;

	// a b c are three vertex positions of the triangle
	Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m) : Object3D(m), a(a), b(b), c(c)
	{
		Vector3f min_point(fmin(a.x(), fmin(b.x(), c.x())), fmin(a.y(), fmin(b.y(), c.y())), fmin(a.z(), fmin(b.z(), c.z())));
		Vector3f max_point(fmax(a.x(), fmax(b.x(), c.x())), fmax(a.y(), fmax(b.y(), c.y())), fmax(a.z(), fmax(b.z(), c.z())));
		bound = Bound(min_point - 0.0001f, max_point +0.0001f);
		normal = Vector3f::cross(b - a, c - a).Vector3f::normalized();
		normal_set = false;
	}

	bool intersect(const Ray &ray, Hit &hit, float tmin) override
	{
		normal = Vector3f::cross(b - a, c - a).Vector3f::normalized();
		float d = Vector3f::dot(normal, a);
		Vector3f origin = ray.getOrigin();
		Vector3f direction = ray.getDirection();
		float k = Vector3f::dot(direction, normal);
		float RHS = d - Vector3f::dot(origin, normal);
		if (k > 1e-9 || k < -1e-9)
		{
			float t = RHS / k;
			if (t > tmin && t < hit.getT())
			{
				Vector3f hit_point = origin + t * direction; // judge if point is in triangle
				Vector3f judge1 = Vector3f::cross(b - a, hit_point - a);
				Vector3f judge2 = Vector3f::cross(c - b, hit_point - b);
				Vector3f judge3 = Vector3f::cross(a - c, hit_point - c);
				if (Vector3f::dot(judge1, judge2) > 0 && Vector3f::dot(judge1, judge3) > 0)
				{
					
					Vector3f normal_result = getNormal(hit_point);
					if (Vector3f::dot(normal_result, direction) > 0)
						normal_result = -normal;
					if (t > tmin && t < hit.getT())
					{
						hit.set(t, material, normal_result.normalized());
						hit.position = hit_point;
						return true;
					}
				}
			}
		}
		return false;
	}
	bool getBound(Bound &bound)
	{
		bound = this->bound;
		return true;
	}
	void setNormal(const Vector3f& anorm, const Vector3f& bnorm,const Vector3f& cnorm)
	{
		an = anorm;
		bn = bnorm;
		cn=cnorm;
		normal_set = true;
	}
	Vector3f getNormal(const Vector3f p)
	{
		if(!normal_set)
			return normal;
		Vector3f va = (a - p), vb = (b - p), vc = (c - p);
        float ra = Vector3f::cross(vb, vc).length(),
              rb = Vector3f::cross(vc, va).length(),
              rc = Vector3f::cross(va, vb).length();
        return (ra * an + rb * bn + rc * cn).normalized();
	}
	Vector3f normal;
	Vector3f vertices[3];
	Bound bound;
	Vector3f an, bn, cn;
	bool normal_set;
protected:
	Vector3f a;
	Vector3f b;
	Vector3f c;
};

#endif // TRIANGLE_H