#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "objectkdtree.hpp"
#include "bound.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:
    std::vector<Object3D*> obj_boundeds;
    std::vector<Object3D*> obj_not_boundeds;
    ObjectKDTreeNode* kdtreeRoot;
    Group() {

    }

    explicit Group (int num_objects) {

    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool result = false;
        // for(auto object = objects.begin(); object < objects.end(); object++)
        // {
        //     if((*object)->intersect(r, h, tmin) == true)
        //         result = true;
        // }
        for(auto obj: obj_not_boundeds)
        {
            if(obj->intersect(r, h, tmin))
                result = true;
        }
        result |= kdtreeRoot->intersect(r, h, tmin);
        return result;
    }

    void addObject(int index, Object3D *obj) {
        objects.push_back(obj);
    }

    int getGroupSize() {
        return objects.size();
    }
    std::vector<Object3D*> getLights() {
        std::vector<Object3D*> lights;
        for(int i = 0 ;i<getGroupSize();i++)
        {
            if(objects[i]->material->getSpecularColor() != Vector3f::ZERO)
            {
                lights.push_back(objects[i]);
            }
        }
        return lights;
    }
    bool getBound(Bound& bound)
    {
        bool tmp = true;
        Bound ret;
        for(auto obj: objects)
        {
            if(obj->getBound(ret) == false)
                return false;
            if(tmp)
            {
                bound = ret;
            }
            else
            {
                tmp = false;
                bound = merge(bound, ret);
            }
        }
        return true;
    }
    void setKDtree()
    {
        for(auto obj: objects)
        {
            if(obj->can_be_bounded)
                obj_boundeds.push_back(obj);
            else 
                obj_not_boundeds.push_back(obj);
        }
        kdtreeRoot = new ObjectKDTreeNode(obj_boundeds,0, obj_boundeds.size()-1,0);
    }

private:
    std::vector<Object3D*> objects;
};

#endif
	
