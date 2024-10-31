#ifndef TEXTURE_H
#define TEXTURE_H


#include <cstdio>
#include <cstring>
#include <iostream>
#include <vecmath.h>

//refer to https://github.com/yaoxingcheng/ComputerGraphics-THU-2020, write it myself

class Texture {
    public:
        Texture() {
            texture_image = nullptr;
            width = height = channel = 0;
        }

        Texture(const char* texture_file);

        Vector3f ColorAt(float u, float v) {

            if (texture_image == nullptr) return Vector3f::ZERO;

            u = getAbmin(u);
            v = 1.0 - getAbmin(v);

            int x = static_cast<int>(u * width + width) % width;
            int y = static_cast<int>(v * height + height) % height;

            auto target_pixel = texture_image + y * channel * width + x * channel;
            return Vector3f(target_pixel[0], target_pixel[1], target_pixel[2]) / 255.f;

        }

        bool hasTexture() {
            if (texture_image == nullptr) return false;
            return true;
        }


    protected:
        float getAbmin(float a) {
            a -= int(a);
            if (a < 0) a += 1;
            return a;
        }
        unsigned char* texture_image;
        int width, height;
        int channel; 
};
//bump, didn't finish yet

#endif