#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// TODO (PA2): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};


//refer to https://github.com/yaoxingcheng/ComputerGraphics-THU-2020


class Curve {
protected:
    std::vector<Vector3f> controls;
public:
    float ymin, ymax, xzmax, mu_min, mu_max;
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {
        ymin = 0x3f3f3f3f;
        ymax = -0x3f3f3f3f;
        xzmax = 0;
        for (auto pt : controls) {
            ymin = min(pt.y(), ymin);
            ymax = max(pt.y(), ymax);
            xzmax = max(xzmax, fabs(pt.x()));
            xzmax = max(xzmax, fabs(pt.z()));
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin){
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;
    virtual CurvePoint getPoint(float mu) = 0;
    
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        mu_min = 0;
        mu_max = 1;
    }

    CurvePoint getPoint(float mu)
    {
        int k = controls.size() - 1;
        std::vector<float> knots(controls.size());
        for (int i = 0; i < controls.size(); i ++)
            knots.push_back(1); 
        std::vector<float> upknots = knots;
        float last = knots[knots.size() - 1];
        for (int i = 0; i < k; i ++)
            knots.push_back(last);
        //getSingle
        CurvePoint pt;
        int bpos = upper_bound(knots.begin(), knots.end(), mu) - knots.begin() - 1;
        //if (upknots[0] != mu) bpos = max(0l, lower_bound(upknots.begin(), upknots.end(), mu) - upknots.begin() - 1);
        std::vector<float> s(k + 2);
        std::vector<float> ds(k + 1, 1);
        s[k] = 1;
        for (int p = 1; p <= k; ++p) {
            for (int ii = k - p; ii < k + 1; ++ii) {
                int i = ii + bpos - k;
                float w1, dw1, w2, dw2;
                if (upknots[i + p] == upknots[i]) {
                    w1 = mu;
                    dw1 = 1;
                } else {
                    w1 = (mu - upknots[i]) / (upknots[i + p] - upknots[i]);
                    dw1 = 1.0 / (upknots[i + p] - upknots[i]);
                }
                if (upknots[i + p + 1] == upknots[i + 1]) {
                    w2 = 1 - mu;
                    dw2 = -1;
                } else {
                    w2 = (upknots[i + p + 1] - mu) /
                         (upknots[i + p + 1] - upknots[i + 1]);
                    dw2 = -1 / (upknots[i + p + 1] - upknots[i + 1]);
                }
                if (p == k) ds[ii] = (dw1 * s[ii] + dw2 * s[ii + 1]) * p;
                s[ii] = w1 * s[ii] + w2 * s[ii + 1];
            }
        }
        s.pop_back();
        int lsk = k - bpos, rsk = bpos + 1 - controls.size();
        if (lsk > 0) {
            for (int i = lsk; i < s.size(); ++i) {
                s[i - lsk] = s[i];
                ds[i - lsk] = ds[i];
            }
            s.resize(s.size() - lsk);
            ds.resize(ds.size() - lsk);
            lsk = 0;
        }
        if (rsk > 0) {
            if (rsk < s.size()) {
                s.resize(s.size() - rsk);
                ds.resize(ds.size() - rsk);
            } else {
                s.clear();
                ds.clear();
            }
        }
        for (int j = 0; j < s.size(); ++j) {
            pt.V += controls[-lsk + j] * s[j];
            pt.T += controls[-lsk + j] * ds[j];
        }
        return pt;
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        float delta = 1.0 / resolution;
        float t = 0;
        std::vector<Vector3f> controls = Curve::getControls();
        int n = controls.size() - 1;
        for(int i = 0; i <= resolution; i++)
        {
            float **B_dp = new float*[n+1];
            for(int j = 0; j < n + 1; j++)
            {
                B_dp[j] = new float[n+1];
            }
            B_dp[2][0] = (1 - t) * (1 - t);
            B_dp[2][1] = 2 * t * (1 - t);
            B_dp[2][2] = t * t;
            for(int k = 3; k <= n; k++)
            {
                for(int l = 0; l <= k; l++)
                {
                    if(l == 0)
                    {
                        B_dp[k][l] = pow(1 - t, k);
                    }
                    else if(l == k)
                    {
                        B_dp[k][l] = pow(t, k);
                    }
                    else
                    {
                        B_dp[k][l] = (1 - t) * B_dp[k-1][l] + t * B_dp[k-1][l-1];
                    }
                }
            }
            CurvePoint ret;
            ret.V = ret.T = Vector3f::ZERO;
            for(int j = 0; j <= n; j++)
            {
                ret.V += B_dp[n][j] * controls[j];
            }
            for(int j = 0; j < n; j++)
            {
                ret.T += n * B_dp[n-1][j] * (controls[j+1]-controls[j]);
            }
            ret.T.normalize();
            data.push_back(ret);
            t += delta;
        }
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }

    CurvePoint getPoint(float mu) {
        int k = 3;
        std::vector<float> knots;
        for (int i = 0; i <= controls.size() + k; i ++)
            knots.push_back(((float) i)/((float) controls.size() + k)); 
        std::vector<float> upknots = knots;
        float last = knots[knots.size() - 1];
        for (int i = 0; i < k; i ++)
            knots.push_back(last);
        //getSingle
        CurvePoint pt;
        int bpos = upper_bound(knots.begin(), knots.end(), mu) - knots.begin() - 1;
        //if (upknots[0] != mu) bpos = max(0l, lower_bound(upknots.begin(), upknots.end(), mu) - upknots.begin() - 1);
        std::vector<float> s(k + 2);
        std::vector<float> ds(k + 1, 1);
        s[k] = 1;
        for (int p = 1; p <= k; ++p) {
            for (int ii = k - p; ii < k + 1; ++ii) {
                int i = ii + bpos - k;
                float w1, dw1, w2, dw2;
                if (upknots[i + p] == upknots[i]) {
                    w1 = mu;
                    dw1 = 1;
                } else {
                    w1 = (mu - upknots[i]) / (upknots[i + p] - upknots[i]);
                    dw1 = 1.0 / (upknots[i + p] - upknots[i]);
                }
                if (upknots[i + p + 1] == upknots[i + 1]) {
                    w2 = 1 - mu;
                    dw2 = -1;
                } else {
                    w2 = (upknots[i + p + 1] - mu) /
                         (upknots[i + p + 1] - upknots[i + 1]);
                    dw2 = -1 / (upknots[i + p + 1] - upknots[i + 1]);
                }
                if (p == k) ds[ii] = (dw1 * s[ii] + dw2 * s[ii + 1]) * p;
                s[ii] = w1 * s[ii] + w2 * s[ii + 1];
            }
        }
        s.pop_back();
        int lsk = k - bpos, rsk = bpos + 1 - controls.size();
        if (lsk > 0) {
            for (int i = lsk; i < s.size(); ++i) {
                s[i - lsk] = s[i];
                ds[i - lsk] = ds[i];
            }
            s.resize(s.size() - lsk);
            ds.resize(ds.size() - lsk);
            lsk = 0;
        }
        if (rsk > 0) {
            if (rsk < s.size()) {
                s.resize(s.size() - rsk);
                ds.resize(ds.size() - rsk);
            } else {
                s.clear();
                ds.clear();
            }
        }
        for (int j = 0; j < s.size(); ++j) {
            pt.V += controls[-lsk + j] * s[j];
            pt.T += controls[-lsk + j] * ds[j];
        }
        return pt;
    } 

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        
        float *knots;
        std::vector<Vector3f> controls = Curve::getControls();
        int n = controls.size() - 1;
        knots = new float[n+3+2];
        float delta = 1.0 / (resolution * (n + 3 + 3));
        float t = 0;
        for(int i = 0; i <= n+3+1; i++)
        {
            knots[i] = (float)i / (float)(n + 3 + 1);
        }
        for(int i = 0; i <= resolution * (n + 3 + 3); i++)
        {
            if(t < knots[3] || t > knots[n+1])
            {
                t += delta;
                continue;
            }
            float **B_dp = new float*[4];
            for(int j = 0; j < 4; j++)
            {
                B_dp[j] = new float[n+4];
            }
            for(int j = 0; j < 4; j++)
            {
                for(int k = 0; k <= n+3-j; k++)
                {
                    if(j == 0)
                    {
                        B_dp[j][k] = (t >= knots[k] && t < knots[k+1]) ? 1 : 0;
                    }
                    else
                    {
                        B_dp[j][k] = (t - knots[k]) / (knots[k+j] - knots[k]) * B_dp[j-1][k]
                                        + (knots[k+j+1] - t) / (knots[k+j+1] - knots[k+1]) * B_dp[j-1][k+1];
                    }
                }
            }
            CurvePoint ret;
            ret.V = ret.T = Vector3f::ZERO;
            for(int j = 0; j <= n; j++)
            {
                ret.V += B_dp[3][j] * controls[j];
            }
            for(int j = 0; j <= n; j++)
            {
                ret.T += 3 * controls[j] * (B_dp[2][j] / (knots[j+3] - knots[j]) - B_dp[2][j+1] / (knots[j+4] - knots[j+1]));
            }
            ret.T.normalize();
            data.push_back(ret);
            t += delta;
        }    
    }

protected:

};

#endif // CURVE_HPP