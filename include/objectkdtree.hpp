#ifndef OBJECTKDTREE_H
#define OBJECTKDTREE_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "bound.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>

//refer tohttps://github.com/yaoxingcheng/ComputerGraphics-THU-2020, but write it myself

class ObjectKDTreeNode: public Object3D {
public:
    Object3D* lc;
    Object3D* rc;
    Object3D* object;
    Bound bound;
    int axis;
    int axis_bound;
    ObjectKDTreeNode()
    {
        lc = rc = nullptr;
    }
    ObjectKDTreeNode(std::vector<Object3D*> &objs, int left, int right, int depth)
    {
        int axis = depth%3;
        lc= rc = nullptr;
        if(left==right)
        {
            objs[left]->getBound(bound);
            object = objs[left];
            return;
        }
        if(right == left+1)
        {
            objs[left]->getBound(bound);
            object = objs[left];
            lc = objs[right];
        }
        else if(right == left +2)
        {
            objs[left]->getBound(bound);
            object = objs[left];
            lc = objs[left+1];
            rc = objs[right];
        } else
        {
            auto tmp = objs;
            int mid = (left+ right)/2;
            if(axis == 0)
            {
                std::sort(tmp.begin()+left, tmp.begin()+right+1,CompareX);
            }
            else if(axis == 1)
            {
                std::sort(tmp.begin()+left, tmp.begin()+right+1, CompareY);
            }
            else 
            {
                std::sort(tmp.begin()+left, tmp.begin()+right+1, CompareZ);
            }
            object = tmp[mid];
            object->getBound(bound);
            if(left<mid)
            {
                lc = new ObjectKDTreeNode(tmp, left, mid-1, depth+1);
            }
            if(right>mid)
            {
                rc = new ObjectKDTreeNode(tmp, mid+1, right, depth+1);
            }
        }
        Bound bound_l, bound_r;
        if(lc)
        {
            lc->getBound(bound_l);
            bound = merge(bound, bound_l);
        }
        if(rc)
        {
            rc->getBound(bound_r);
            bound = merge(bound, bound_r);
        }
    }
    bool intersect(const Ray& r, Hit &h, float tmin) override
    {
        // if(bound.intersect(r, -0x3f3f3f3f, 0x3f3f3f3f) == false) //todo:debug
        //     return false;
        bool hit_l = false, hit_r = false;
        if(lc)
        {
            hit_l = lc->intersect(r, h, tmin);
        }
        if(rc)
        {
            hit_r = rc->intersect(r, h, tmin);
        }
        bool tmp = object->intersect(r, h, tmin);
        return hit_l || hit_r || tmp;
    }
    bool getBound(Bound& bound) override
    {
        bound = this->bound;
        return true;
    }
    static bool CompareX(Object3D* a, Object3D* b) {
        Bound _a, _b;
        if (!a->getBound(_a) || !b->getBound(_b))
            return false;
        return _a.bounds[0].x() < _b.bounds[0].x();
    }
    static bool CompareY(Object3D* a, Object3D* b) {
        Bound _a, _b;
        if (!a->getBound(_a) || !b->getBound(_b))
            return false;
        return _a.bounds[0].y() < _b.bounds[0].y();
    }
    static bool CompareZ(Object3D* a, Object3D* b) {
        Bound _a, _b;
        if (!a->getBound(_a) || !b->getBound(_b))
            return false;
        return _a.bounds[0].z() < _b.bounds[0].z();
    }
};

#endif