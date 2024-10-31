#ifndef RENDER_H
#define RENDER_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "scene_parser.hpp"
#include "image.hpp"
#include "utils.hpp"
#include "hitkdtree.hpp"
#include "photonkdtree.hpp"

#include <omp.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cstring>

using namespace std;
//todo: update contents in hit

class PathTracer
{
public:
    int spp; //samples per pixel
    int traceDepth = 50;
    SceneParser scene;
    string savePath;
    PathTracer(int _spp, const SceneParser& _scene, string outputFile, int depth = 50): scene(_scene), savePath(outputFile), traceDepth(depth)
    {
        spp=_spp;
    }
    Vector3f getRadiance(Ray ray, const SceneParser& scene, int depth, int seed = 0 ,Vector3f total_attenuation = Vector3f(1,1,1))
    {
        Group* group = scene.getGroup();
        Hit hit;
        Ray next(ray);
        bool isIntersect = false;
        if(group!=nullptr)
        {
            isIntersect = group->intersect(ray, hit, 0.001);
        }
        if(isIntersect)
        {
            Material* material = hit.getMaterial();
            Vector3f normal = hit.getNormal();
            
            Vector3f color = material->getSpecularColor();
            //for rendering dispersion
            // if(seed ==0)
            // {
            //     color = Vector3f(material->getSpecularColor().x(), 0, 0);
            // }
            // else if(seed ==1)
            // {
            //     color = Vector3f(0, material->getSpecularColor().y(), 0);
            // }
            // else
            //     color = Vector3f(0,0,material->getSpecularColor().z());

            Vector3f attenuation;
            if(material->getType() == DIFF)
            {
                Vector3f target = normal + generateRandomPoint();
                next = Ray(ray.pointAtParameter(hit.getT()), target);

                 
                attenuation = material->getAttenuation(hit.u,hit.v); //apply to sppm
                
            }
            else if(material->getType() == SPEC)
            {
                Vector3f tmp = ray.getDirection().normalized();
                Vector3f target = tmp - 2 * Vector3f::dot(tmp, normal) * normal;
                next = Ray(ray.pointAtParameter(hit.getT()), target);


                attenuation = material->getAttenuation();
            }
            else if(material->getType() == REFR)
            {
                attenuation = material->getAttenuation();
                float refractive = material->getRefractive();
                Vector3f tmp = ray.getDirection().normalized();
                Vector3f reflect = tmp - 2 * Vector3f::dot(tmp, normal) * normal;
                Vector3f refract;

                //for rendering dispersion
                // if(seed ==0)
                //     refractive = 2.405;
                // else if(seed ==1)
                //     refractive = 2.427;
                // else
                //     refractive = 2.449;

                float cos = -Vector3f::dot(tmp, normal);
                if(cos>0)
                {
                    refractive = 1/refractive;
                }
                bool canRefract = false;
                float disc = 1.0 - refractive*refractive*(1-cos*cos);
                if(disc>0)
                {
                    refract = refractive * (tmp + cos*normal) - normal* sqrt(disc);
                    canRefract = true;
                }

                float reflect_prob;
                if(canRefract)
                {
                    float r = ((1 - refractive) / (1 + refractive)) * ((1 - refractive) / (1 + refractive));
                    reflect_prob = r + (1 - r) * pow((1 - cos), 5);
                }
                else
                {
                    reflect_prob = 1.1;
                }
                if (drand48() < reflect_prob){
                    next = Ray(ray.pointAtParameter(hit.getT()), reflect);
                } else {
                    next = Ray(ray.pointAtParameter(hit.getT()), refract);
                }
            }
            if(depth<traceDepth)
            {
                Vector3f total = total_attenuation * attenuation;
                if(total.x() < 1e-3 && total.y() <1e-3 && total.z() <1e-3)
                {
                    return color;
                }
                return color+attenuation*getRadiance(next, scene, depth+1,seed, total);
            }
            return color;
        }
        else
        {
            return scene.getBackgroundColor();
        }
    }
    
    
    void render()
    {
        Camera* camera = scene.getCamera();
        int width = camera->getWidth();
        int height = camera->getHeight();
        Image output(width, height);
        omp_set_num_threads(25);
        #pragma omp parallel for schedule(dynamic, 1) 
        for(int y = 0; y <height;y++)
        {
            fprintf(stderr, "rendering row %d with thread %d\n", y, omp_get_thread_num());
            
            for(int x=0;x<width;x++)
            {
                // printf("\n\nx:%d y:%d\n", x, y);
                Vector3f color = Vector3f::ZERO;
                for(int sample_point = 0; sample_point<spp;sample_point++)
                {
                    //for rendering dispersion:
                    // for(int i=0;i<3;i++){
                    // Ray camRay = camera->generateRay(Vector2f(x+RND,y+RND));
                    // float t = RND2;
                    // int seed;
                    // if(3*t<1) 
                    //     seed = 0;
                    // else if(3*t<2)
                    //     seed=1;
                    // else 
                    //     seed=2;
                    // color+=getRadiance(camRay, scene, 0, seed);
                    // }
                    Ray camRay = camera->generateRay(Vector2f(x+RND,y+RND));
                    color+=getRadiance(camRay, scene, 0);
                }
                output.SetPixel(x, y , color/spp);
            }
        }
        output.SaveBMP(savePath.c_str());
    }
};

class SPPM {
public: 
    int num_round;
    int num_photon;
    int traceDepth = 50;
    SceneParser scene;
    string savePath;
    vector<Object3D*> lights;
    vector<Hit*> hits;
    HitKDTree* hitKDtree;
    Group* group;
    Camera* camera;
    PhotonKDTreeNode* photonKDtree;
    SPPM(const SceneParser& _scene, string outputFile, int nr, int np, int depth = 50): scene(_scene), savePath(outputFile), traceDepth(depth), num_round(nr), num_photon(np)
    {
        hitKDtree = nullptr;
        photonKDtree = nullptr;
        camera = _scene.getCamera();
        group = _scene.getGroup();
        lights = group->getLights();
        for(int i = 0; i<camera->getWidth();i++)
        {
            for(int j=0;j<camera->getHeight();j++)
            {
                hits.push_back(new Hit());
            }
        }
    }
    void render()
    {
        int width = camera->getWidth();
        int height = camera->getHeight();
        Image image(width, height);
        for(int round = 0 ;round<num_round;round++)
        {
            fprintf(stderr, "sppm round %d / %d \n", round, num_round);
#pragma omp parallel for schedule(dynamic, 1)
            for(int x=0;x<width;x++)
            {
                if(x%50==0)
                    fprintf(stderr, "row %d\n", x);
                for(int y=0;y<height;y++)
                {
                    Ray camRay = camera->generateRay(Vector2f(x+RND,y+RND));
                    hits[x*height+y]->setT(1e38);
                    launchPhotons(camRay, hits[x*height+y]);
                }
            }
            buildKDtree();
            int photon_per = num_photon / lights.size();
#pragma omp parallel for schedule(dynamic, 1)
            for(int i=0; i<photon_per; i++)
            {
                if (i % 100000 == 0)
                    //fprintf(stderr, "tracing photon %d with thread %d round %d\n", i, omp_get_thread_num(), round);
                for(int j = 0;j<lights.size();j++)
                {
                    // printf("1111\n");
                    Ray newRay = lights[j]->generateRay();
                    tracePhotons(newRay, lights[j]->material->getSpecularColor());
                }
            }
            if(round % 100 ==0)
            {
                fprintf(stderr, "saving\n");
                string filename = "output/test2/"+to_string(round)+".bmp";
                saveImage(num_round, num_photon, filename);
            }
        }
        saveImage(num_round, num_photon, savePath);
    }
    //part of this function refer to https://github.com/yaoxingcheng/ComputerGraphics-THU-2020
    void launchPhotons(Ray &ray, Hit*hit, int traceDepth=20)
    {
        if(group == nullptr)
            return;
        int depth = 0;
        Vector3f total_attenuation = Vector3f(1,1,1);
        while(depth<traceDepth)
        {
            depth++;
            hit->setT(1e38);
            if(group->intersect(ray, *hit, 0.00001) == false)
            {
                hit->fluxLight+=hit->attenWeight*scene.getBackgroundColor();
                return;
            }
            Ray next(ray);
            Vector3f attenuation;
            Material* material = hit->getMaterial();
            Vector3f normal = hit->getNormal();
            Vector3f color = material->getSpecularColor();
            if(material->getType() == DIFF || material->getType() == LIGHT)
            {
                float u = atan2(-normal.z(), normal.x()) / (2 * M_PI) + 0.5f, v = acos(-normal.y()) / M_PI;
                hit->attenWeight = total_attenuation*material->getAttenuation(u, v);
                hit->fluxLight+=total_attenuation*color; //todo???
                // total_attenuation.print();
                // color.print();
                // hit->fluxLight.print();
                // cout<<endl;
                return;
            }
            else if(material->getType() == SPEC)
            {
                Vector3f tmp = ray.getDirection().normalized();
                Vector3f target = tmp - 2 * Vector3f::dot(tmp, normal) * normal;
                next = Ray(ray.pointAtParameter(hit->getT()), target.normalized()); //todo:fuzz


                attenuation = material->getAttenuation();
            }
            else if(material->getType() == REFR)
            {
                attenuation = material->getAttenuation();
                float refractive = material->getRefractive();
                Vector3f tmp = ray.getDirection().normalized();
                Vector3f reflect = tmp - 2 * Vector3f::dot(tmp, normal) * normal;
                Vector3f refract;
                float cos = -Vector3f::dot(tmp, normal);
                if(cos>0)
                {
                    refractive = 1/refractive;
                }
                bool canRefract = false;
                float disc = 1.0 - refractive*refractive*(1-cos*cos);
                if(disc>0)
                {
                    refract = refractive * (tmp + cos*normal) - normal* sqrt(disc);
                    canRefract = true;
                }

                float reflect_prob;
                if(canRefract)
                {
                    float r = ((1 - refractive) / (1 + refractive)) * ((1 - refractive) / (1 + refractive));
                    reflect_prob = r + (1 - r) * pow((1 - cos), 5);
                }
                else
                {
                    reflect_prob = 1.1;
                }
                if (drand48() < reflect_prob){
                    next = Ray(ray.pointAtParameter(hit->getT()), reflect);
                } else {
                    next = Ray(ray.pointAtParameter(hit->getT()), refract);
                }
            }
            ray = next;
            total_attenuation = total_attenuation*attenuation;
        }
    }
    void tracePhotons(Ray& ray, const Vector3f& radiance, int traceDepth = 20)
    {
        if(group == nullptr)
            return;
        int depth = 0;
        Vector3f total_attenuation(radiance);
        total_attenuation=total_attenuation* Vector3f(255,255,255);
        while(depth<traceDepth && (total_attenuation.x()>=1e-3 || total_attenuation.y()>=1e-3 || total_attenuation.z()>=1e-3))
        {
            depth++;
            Hit hit;
            if(group->intersect(ray, hit, 0.0001) == false)
            {
                return;
            }
            ray.origin += ray.getDirection() * hit.getT();
            Ray next(ray);
            Vector3f attenuation;
            Material* material = hit.getMaterial();
            Vector3f normal = hit.getNormal();
            Vector3f color = material->getSpecularColor();
            
            if(material->getType() == DIFF || material->getType() == LIGHT)
            {
                //hitKDtree->update(hitKDtree->root, hit.position, total_attenuation, ray.getDirection());
                float u = atan2(-normal.z(), normal.x()) / (2 * M_PI) + 0.5f, v = acos(-normal.y()) / M_PI;
                photonKDtree->Update(hit.position, total_attenuation, hit.is_front_face);
                ray.direction = normal+generateRandomPoint();
                attenuation = material->getAttenuation(u, v);//todo
                
            }
            else if(material->getType() == SPEC)
            {
                Vector3f tmp = ray.getDirection().normalized();
                Vector3f target = tmp - 2 * Vector3f::dot(tmp, normal) * normal;

                ray.direction = (ray.direction-2*Vector3f::dot(ray.getDirection(), normal)).normalized();
                attenuation = material->getAttenuation();
            }
            else if(material->getType() == REFR)
            {
                attenuation = material->getAttenuation(); //todo
                float refractive = material->getRefractive();
                Vector3f tmp = ray.getDirection().normalized();
                Vector3f reflect = tmp - 2 * Vector3f::dot(tmp, normal) * normal;
                Vector3f refract;
                float cos = -Vector3f::dot(tmp, normal);
                if(cos>0)
                {
                    refractive = 1/refractive;
                }
                bool canRefract = false;
                float disc = 1.0 - refractive*refractive*(1-cos*cos);
                if(disc>0)
                {
                    refract = refractive * (tmp + cos*normal) - normal* sqrt(disc);
                    canRefract = true;
                }

                float reflect_prob;
                if(canRefract)
                {
                    float r = ((1 - refractive) / (1 + refractive)) * ((1 - refractive) / (1 + refractive));
                    reflect_prob = r + (1 - r) * pow((1 - cos), 5);
                }
                else
                {
                    reflect_prob = 1.1;
                }
                if (drand48() < reflect_prob){
                    ray.direction = (tmp - 2*Vector3f::dot(tmp, normal) * normal).normalized();
                } else {
                    ray.direction = (refractive * (tmp + cos*normal)-normal*sqrt(disc)).normalized();
                }
            }
            if(material->getType()!=LIGHT)
            {
                total_attenuation = total_attenuation*attenuation;
            }
            else
                return;
        }
    }
    void buildKDtree()
    {
        // if(hitKDtree) delete hitKDtree;
        // hitKDtree = new HitKDTree(hits);
        printf("building KDTree for SPPM \n");
        if (photonKDtree != nullptr)
            clearTree(photonKDtree);
        vector<Hit*> hit_list(hits);
        printf("total visible points %d \n", hit_list.size());
        photonKDtree = new PhotonKDTreeNode(hit_list, 0, hit_list.size() - 1, 0);
        printf("building finished \n");
    }
    void clearTree(PhotonKDTreeNode* node) {
        if (node != nullptr){
            if (node->left_node) clearTree(node->left_node);
            if (node->right_node) clearTree(node->right_node);
            delete node;
        }
    }
    void saveImage(int rounds, int photons, string filename) {
        int height = camera->getHeight(), width = camera->getWidth();
        Image image(width, height);
        for (int u = 0; u < width; ++u)
            for (int v = 0; v < height; ++v) {
                Hit* hit = hits[u * height + v];
                Vector3f tmp = hit->flux / (M_PI * hit->radius * num_photon * rounds) + hit->fluxLight / rounds;
                
                image.SetPixel(u, v, hit->flux / (M_PI * hit->radius * num_photon * rounds) + hit->fluxLight / rounds);
            }
        fprintf(stderr, "save image to %s \n", filename.c_str());
        image.SaveImage(filename.c_str());
    }
};

#endif
	
