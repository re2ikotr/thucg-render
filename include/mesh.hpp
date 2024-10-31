#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "bound.hpp"
#include "triangle.hpp"
#include "objectkdtree.hpp"
#include "Vector2f.h"
#include "Vector3f.h"


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
        bool valid() { return x[0] != -1 && x[1] != -1 && x[2] != -1; }
    };

    Bound bound;
    void setTree();
    ObjectKDTreeNode* kdtreeRoot;
    std::vector<Object3D*> triangles;
    bool getBound(Bound& bound) override;
    std::vector<std::string> split(std::string str, std::string pattern);

    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    std::vector<TriangleIndex> nIdx;
    std::vector<Vector3f> n;
    std::vector<Vector3f> vn;
    bool intersect(const Ray &r, Hit &h, float tmin) override;

private:

    // Normal can be used for light estimation
    void computeNormal();
};

#endif
