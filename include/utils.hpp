#ifndef UTILS_H
#define UTILS_H
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <vecmath.h>
#include <iostream>

//part of functions refer to Guangxuan Xiao

static std::mt19937 mersenneTwister;
static std::uniform_real_distribution<float> uniform;
#define RND (2.0 * uniform(mersenneTwister) - 1.0)
#define RND2 (uniform(mersenneTwister))
#define INF 99999999
Vector3f generateRandomPoint()
{
    Vector3f ret;
    do
    {
        ret = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1, 1, 1);
    } while (ret.squaredLength() >= 1.0);
    return ret;
}
int getRandomInt()
{
    std::default_random_engine e;
    std::uniform_int_distribution<int> u(0,999999); // 左闭右闭区间
    e.seed(time(0));
    return u(e);
}


#endif // UTILS_H
