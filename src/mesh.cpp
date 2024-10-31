#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

bool Mesh::intersect(const Ray &r, Hit &h, float tmin) {

    // Optional: Change this brute force method into a faster one.
    bool result = false;
    // for (int triId = 0; triId < (int) t.size(); ++triId) {
    //     TriangleIndex& triIndex = t[triId];
    //     Triangle triangle(v[triIndex[0]],
    //                       v[triIndex[1]], v[triIndex[2]], material);
    //     triangle.normal = n[triId];
    //     result |= triangle.intersect(r, h, tmin);
    // }
    result = kdtreeRoot->intersect(r, h, tmin);

    return result;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string vnTok("vn");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        } else if (tok == fTok) {
            // if (line.find(bslash) != std::string::npos) {
            //     std::replace(line.begin(), line.end(), bslash, space);
            //     std::stringstream facess(line);
            //     TriangleIndex trig;
            //     facess >> tok;
            //     for (int ii = 0; ii < 3; ii++) {
            //         facess >> trig[ii] >> texID;
            //         trig[ii]--;
            //     }
            //     t.push_back(trig);
            // } else {
            //     TriangleIndex trig;
            //     for (int ii = 0; ii < 3; ii++) {
            //         ss >> trig[ii];
            //         trig[ii]--;
            //     }
            //     t.push_back(trig);
            // }
            bool tFlag = 1, nFlag = 1;
            TriangleIndex vId, tId, nId;
            for (int i = 0; i < 3; ++i) {
                std::string str;
                ss >> str;
                std::string bslash("/");
                std::vector<std::string> id = split(str, bslash);
                vId[i] = atoi(id[0].c_str()) - 1;
                if (id.size() > 1) {
                    tId[i] = atoi(id[1].c_str()) - 1;
                }
                if (id.size() > 2) {
                    nId[i] = atoi(id[2].c_str()) - 1;
                }
            }
            t.push_back(vId);
            nIdx.push_back(nId);
        } else if (tok == texTok) {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        } else if(tok == vnTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            vn.push_back(vec);
        }
    }
    computeNormal();

    f.close();
}
std::vector<std::string> Mesh::split(std::string str, std::string pattern) {
    std::string::size_type pos;
    std::vector<std::string> result;
    str += pattern;
    int size = str.size();
    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

void Mesh::computeNormal() {
    Bound temp;
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        triangles.push_back((Object3D*)new Triangle(v[triIndex[0]], v[triIndex[1]], v[triIndex[2]], material));
        ((Triangle *)triangles.back())->getBound(temp);
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
        // TriangleIndex &nIndex = nIdx[triId];
        // if (nIndex.valid())
        //     ((Triangle *)triangles.back())
        //         ->setNormal(vn[nIndex[0]], vn[nIndex[1]], vn[nIndex[2]]);
    }
    setTree();
}
bool Mesh::getBound(Bound &bound) 
{
    bound=this->bound;
    return true;
}

void Mesh::setTree()
{
    kdtreeRoot = new ObjectKDTreeNode(triangles, 0, triangles.size() -1,0);
}
