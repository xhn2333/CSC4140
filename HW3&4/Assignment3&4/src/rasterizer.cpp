#include "rasterizer.h"
#include <queue>
#include <set>
using namespace std;

namespace CGL
{
    int numTriangle = 0;
    int numLine = 0;
    RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
                                 size_t width, size_t height,
                                 unsigned int sample_rate)
    {
        this->psm = psm;
        this->lsm = lsm;
        this->width = width;
        this->height = height;
        this->sample_rate = sample_rate;

        sample_buffer.resize(width * height * sample_rate, Color::White);
    }

    // Used by rasterize_point and rasterize_line
    void RasterizerImp::fill_pixel(float x, float y, Color c) // size_t
    {
        // TODO: Task 2: You might need to this function to fix points and lines (such as the black rectangle border in test4.svg)
        // NOTE: You are not required to implement proper supersampling for points and lines
        // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)
        int sqrt_sample_rate = int(sqrt(sample_rate));
        int sx = (int)floor(1.0 * x * sqrt_sample_rate);
        int sy = (int)floor(1.0 * y * sqrt_sample_rate);
        sample_buffer[(sy * width * sqrt_sample_rate + sx)] = c;
    }

    // Rasterize a point: simple example to help you start familiarizing
    // yourself with the starter code.
    //
    void RasterizerImp::rasterize_point(float x, float y, Color color)
    {
        // fill in the nearest pixel
        float threshold_sample_rate = 1.0 / int(sqrt(sample_rate));
        // check bounds
        for (float sx = x; sx < x + 1; sx += threshold_sample_rate)
        {
            for (float sy = y; sy < y + 1; sy += threshold_sample_rate)
            {
                if (sx < 0 || sx >= width)
                    continue;
                if (sy < 0 || sy >= height)
                    continue;
                fill_pixel(sx, sy, color);
            }
        }
        return;
    }

    // Rasterize a line.
    void RasterizerImp::rasterize_line(float x0, float y0,
                                       float x1, float y1,
                                       Color color)
    {
        if (x0 > x1)
        {
            swap(x0, x1);
            swap(y0, y1);
        }
        float threshold_sample_rate = 1.0 / int(sqrt(sample_rate));
        float pt[] = {x0, y0};
        float m = (y1 - y0) / (x1 - x0);
        float dpt[] = {1, m};
        int steep = abs(m) > 1;
        if (steep)
        {
            dpt[0] = x1 == x0 ? 0 : 1.0 / abs(m);
            dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
        }

        while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0))
        {
            rasterize_point(pt[0], pt[1], color);
            pt[0] += dpt[0] * threshold_sample_rate;
            pt[1] += dpt[1] * threshold_sample_rate;
        }

        // numLine ++;
        // cout << "-------------line" << numLine << endl;
    }

    // Rasterize a triangle.

    bool RasterizerImp::inside(float Ax, float Ay,
                               float Bx, float By,
                               float Cx, float Cy,
                               float Px, float Py)
    {

        float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
        float cCROSSap, bCROSScp, aCROSSbp;

        ax = Cx - Bx;
        ay = Cy - By;
        bx = Ax - Cx;
        by = Ay - Cy;
        cx = Bx - Ax;
        cy = By - Ay;
        apx = Px - Ax;
        apy = Py - Ay;
        bpx = Px - Bx;
        bpy = Py - By;
        cpx = Px - Cx;
        cpy = Py - Cy;

        aCROSSbp = ax * bpy - ay * bpx;
        cCROSSap = cx * apy - cy * apx;
        bCROSScp = bx * cpy - by * cpx;

        return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f)) || ((aCROSSbp <= 0.0f) && (bCROSScp <= 0.0f) && (cCROSSap <= 0.0f));
    };

    void RasterizerImp::rasterize_triangle(float x0, float y0,
                                           float x1, float y1,
                                           float x2, float y2,
                                           Color color)
    {
        // numTriangle++;
        // TODO: Task 1: Implement basic triangle rasterization here, no supersampling

        int sqrt_sample_rate = int(sqrt(sample_rate));
        float threshold_sample_rate = 1.0 / sqrt_sample_rate;

        RasterizerPoint midPoint;
        midPoint.x = float(int((x0 + x1 + x2) / 3)) + 0.5 * threshold_sample_rate;
        midPoint.y = float(int((y0 + y1 + y2) / 3)) + 0.5 * threshold_sample_rate;

        queue<RasterizerPoint> q;
        set<pair<int, int>> hashMap;
        float dir[8][2] = {{0.0f, 1.0}, {0.0f, -1.0}, {1.0, 0.0f}, {-1.0, 0.0f}, {1.0, 1.0}, {1.0, -1.0}, {-1.0, 1.0}, {-1.0, -1.0}};

        q.push(midPoint);
        while (!q.empty())
        {
            RasterizerPoint head = q.front();
            q.pop();
            int sx = (int)floor(float(head.x * sqrt_sample_rate) + 0.5);
            int sy = (int)floor(float(head.y * sqrt_sample_rate) + 0.5);
            if (sx < 0 || sx >= width * sqrt_sample_rate)
                continue;
            if (sy < 0 || sy >= height * sqrt_sample_rate)
                continue;
            if (hashMap.find(make_pair(sx, sy)) == hashMap.end())
            {

                if (inside(x0, y0, x1, y1, x2, y2, head.x, head.y))
                {
                    // sample_buffer[sy * width * sqrt_sample_rate + sx] = color;
                    fill_pixel(head.x, head.y, color);
                    hashMap.insert(make_pair(sx, sy));
                    for (int i = 0; i < 8; ++i)
                    {
                        RasterizerPoint tail;
                        tail.x = head.x + dir[i][0] * threshold_sample_rate;
                        tail.y = head.y + dir[i][1] * threshold_sample_rate;
                        q.push(tail);
                    }
                }
            }
            rasterize_line(x0, y0, x1, y1, color);
            rasterize_line(x1, y1, x2, y2, color);
            rasterize_line(x2, y2, x0, y0, color);
        }
        /*
        if (!hashMap.size())
        {
            cout << "--------------------" << numTriangle << endl;
            cout << "A: " << x0 << " " << y0 << endl;
            cout << "B: " << x1 << " " << y1 << endl;
            cout << "C: " << x2 << " " << y2 << endl;
            cout << "MidPoint: " << midPoint.x << " " << midPoint.y << endl;
            for (int i = 0; i < 4; ++i)
            {
                RasterizerPoint tail;
                tail.x = midPoint.x + dir[i][0];
                tail.y = midPoint.y + dir[i][1];
                cout << "tail:" << tail.x << " " << tail.y << " "
                     << (hashMap.find(make_pair(int(tail.x), int(tail.y))) == hashMap.end()) << " "
                     << inside(x0, y0, x1, y1, x2, y2, tail.x, tail.y)
                     << endl;
            }
        }
        */

        // TODO: Task 2: Update to implement super-sampled rasterization
    }

    void RasterizerImp::rasterize_interpolated_color_line(float x0, float y0, Color c0,
                                                          float x1, float y1, Color c1)
    {
        if (x0 > x1)
        {
            swap(x0, x1);
            swap(y0, y1);
            swap(c0, c1);
        }
        float threshold_sample_rate = 1.0 / int(sqrt(sample_rate));
        float pt[] = {x0, y0};
        float m = (y1 - y0) / (x1 - x0);
        float dpt[] = {1, m};
        int steep = abs(m) > 1;
        if (steep)
        {
            dpt[0] = x1 == x0 ? 0 : 1.0 / abs(m);
            dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
        }

        while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0))
        {
            float Alpha = ((x0 != x1) ? (x1 - pt[0]) / (x1 - x0) : (y1 - pt[1]) / (y1 - y0));
            Color color = Alpha * c0 + (1 - Alpha) * c1;
            rasterize_point(pt[0], pt[1], color);
            pt[0] += dpt[0] * threshold_sample_rate;
            pt[1] += dpt[1] * threshold_sample_rate;
        }

        // numLine ++;
        // cout << "-------------line" << numLine << endl;
    }

    void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
                                                              float x1, float y1, Color c1,
                                                              float x2, float y2, Color c2)
    {
        // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
        float Alpha, Beta, Gamma;

        int sqrt_sample_rate = int(sqrt(sample_rate));
        float threshold_sample_rate = 1.0 / sqrt_sample_rate;

        RasterizerPoint midPoint;
        midPoint.x = float(int((x0 + x1 + x2) / 3)) + 0.5 * threshold_sample_rate;
        midPoint.y = float(int((y0 + y1 + y2) / 3)) + 0.5 * threshold_sample_rate;

        queue<RasterizerPoint> q;
        set<pair<int, int>> hashMap;
        float dir[8][2] = {{0.0f, 1.0}, {0.0f, -1.0}, {1.0, 0.0f}, {-1.0, 0.0f}, {1.0, 1.0}, {1.0, -1.0}, {-1.0, 1.0}, {-1.0, -1.0}};

        q.push(midPoint);
        while (!q.empty())
        {
            RasterizerPoint head = q.front();
            q.pop();
            int sx = (int)floor(float(head.x * sqrt_sample_rate));
            int sy = (int)floor(float(head.y * sqrt_sample_rate));
            if (sx < 0 || sx >= width * sqrt_sample_rate)
                continue;
            if (sy < 0 || sy >= height * sqrt_sample_rate)
                continue;
            if (hashMap.find(make_pair(sx, sy)) == hashMap.end())
            {

                if (inside(x0, y0, x1, y1, x2, y2, head.x, head.y))
                {
                    Alpha = (-(head.x - x1) * (y2 - y1) + (head.y - y1) * (x2 - x1)) / (-(x0 - x1) * (y2 - y1) + (y0 - y1) * (x2 - x1));
                    Beta = (-(head.x - x2) * (y0 - y2) + (head.y - y2) * (x0 - x2)) / (-(x1 - x2) * (y0 - y2) + (y1 - y2) * (x0 - x2));
                    Gamma = 1 - Alpha - Beta;
                    Color color = Alpha * c0 + Beta * c1 + Gamma * c2;
                    // sample_buffer[sy * width * sqrt_sample_rate + sx] = color;
                    fill_pixel(head.x, head.y, color);
                    hashMap.insert(make_pair(sx, sy));
                    for (int i = 0; i < 8; ++i)
                    {
                        RasterizerPoint tail;
                        tail.x = head.x + dir[i][0] * threshold_sample_rate;
                        tail.y = head.y + dir[i][1] * threshold_sample_rate;
                        q.push(tail);
                    }
                }
            }
        }

        rasterize_interpolated_color_line(x0, y0, c0, x1, y1, c1);
        rasterize_interpolated_color_line(x1, y1, c1, x2, y2, c2);
        rasterize_interpolated_color_line(x2, y2, c2, x0, y0, c0);
        // Hint: You can reuse code from rasterize_triangle
    }

    void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
                                                    float x1, float y1, float u1, float v1,
                                                    float x2, float y2, float u2, float v2,
                                                    Texture &tex)
    {
        // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
        float Alpha, Beta, Gamma;
        float Alpha1, Beta1, Gamma1;
        float Alpha2, Beta2, Gamma2;

        SampleParams params;
        Vector2D U1(x1 - x0, y1 - y0), U2(x2 - x0, y2 - y0);
        Vector2D V1(u1 - u0, v1 - v0), V2(u2 - u0, v2 - v0);
        params.p_dx_uv = Vector2D((U1.y * (-V2.x) - U2.y * (-V1.x)) / (U1.x * U2.y - U1.y * U2.x),
                                  (U1.y * (-V2.y) - U2.y * (-V1.y)) / (U1.x * U2.y - U1.y * U2.x));
        params.p_dy_uv = Vector2D((U1.x * (-V2.x) - U2.x * (-V1.x)) / (U1.y * U2.x - U1.x * U2.y),
                                  (U1.y * (-V2.y) - U2.y * (-V1.y)) / (U1.x * U2.y - U1.y * U2.x));

        int sqrt_sample_rate = int(sqrt(sample_rate));
        float threshold_sample_rate = 1.0 / sqrt_sample_rate;

        RasterizerPoint midPoint;
        midPoint.x = float(int((x0 + x1 + x2) / 3)) + 0.5 * threshold_sample_rate;
        midPoint.y = float(int((y0 + y1 + y2) / 3)) + 0.5 * threshold_sample_rate;

        queue<RasterizerPoint> q;
        set<pair<int, int>> hashMap;
        float dir[8][2] = {{0.0f, 1.0}, {0.0f, -1.0}, {1.0, 0.0f}, {-1.0, 0.0f}, {1.0, 1.0}, {1.0, -1.0}, {-1.0, 1.0}, {-1.0, -1.0}};

        q.push(midPoint);
        while (!q.empty())
        {
            RasterizerPoint head = q.front();
            q.pop();
            int sx = (int)floor(float(head.x * sqrt_sample_rate));
            int sy = (int)floor(float(head.y * sqrt_sample_rate));
            if (sx < 0 || sx >= width * sqrt_sample_rate)
                continue;
            if (sy < 0 || sy >= height * sqrt_sample_rate)
                continue;
            if (hashMap.find(make_pair(sx, sy)) == hashMap.end())
            {

                if (inside(x0, y0, x1, y1, x2, y2, head.x, head.y))
                {
                    Alpha = (-(head.x - x1) * (y2 - y1) + (head.y - y1) * (x2 - x1)) / (-(x0 - x1) * (y2 - y1) + (y0 - y1) * (x2 - x1));
                    Beta = (-(head.x - x2) * (y0 - y2) + (head.y - y2) * (x0 - x2)) / (-(x1 - x2) * (y0 - y2) + (y1 - y2) * (x0 - x2));
                    Gamma = 1 - Alpha - Beta;

                    Alpha1 = (-(head.x + threshold_sample_rate - x1) * (y2 - y1) + (head.y - y1) * (x2 - x1)) / (-(x0 - x1) * (y2 - y1) + (y0 - y1) * (x2 - x1));
                    Beta1 = (-(head.x + threshold_sample_rate - x2) * (y0 - y2) + (head.y - y2) * (x0 - x2)) / (-(x1 - x2) * (y0 - y2) + (y1 - y2) * (x0 - x2));
                    Gamma1 = 1 - Alpha - Beta;

                    Alpha2 = (-(head.x - x1) * (y2 - y1) + (head.y + threshold_sample_rate - y1) * (x2 - x1)) / (-(x0 - x1) * (y2 - y1) + (y0 - y1) * (x2 - x1));
                    Beta2 = (-(head.x - x2) * (y0 - y2) + (head.y + threshold_sample_rate - y2) * (x0 - x2)) / (-(x1 - x2) * (y0 - y2) + (y1 - y2) * (x0 - x2));
                    Gamma2 = 1 - Alpha - Beta;
                    
                    params.p_uv = Alpha * Vector2D(u0, v0) + Beta * Vector2D(u1, v1) + Gamma * Vector2D(u2, v2);
                    params.p_dx_uv = (Alpha1 * Vector2D(u0, v0) + Beta1 * Vector2D(u1, v1) + Gamma1 * Vector2D(u2, v2) - params.p_uv) / threshold_sample_rate;
                    params.p_dy_uv = (Alpha2 * Vector2D(u0, v0) + Beta2 * Vector2D(u1, v1) + Gamma2 * Vector2D(u2, v2) - params.p_uv) / threshold_sample_rate;
                    // std::cout << params.p_uv.x << " " << params.p_uv.y << endl;

                    fill_pixel(head.x, head.y, tex.sample(params));
                    hashMap.insert(make_pair(sx, sy));
                    for (int i = 0; i < 8; ++i)
                    {
                        RasterizerPoint tail;
                        tail.x = head.x + dir[i][0] * threshold_sample_rate;
                        tail.y = head.y + dir[i][1] * threshold_sample_rate;
                        q.push(tail);
                    }
                }
            }
        }
        // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
        // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle
    }

    void RasterizerImp::set_sample_rate(unsigned int rate)
    {
        // TODO: Task 2: You may want to update this function for supersampling support

        this->sample_rate = rate;

        this->sample_buffer.resize(width * height * sample_rate, Color::White);
    }

    void RasterizerImp::set_framebuffer_target(unsigned char *rgb_framebuffer,
                                               size_t width, size_t height)
    {

        // TODO: Task 2: You may want to update this function for supersampling support

        this->width = width;
        this->height = height;
        this->rgb_framebuffer_target = rgb_framebuffer;

        this->sample_buffer.resize(width * height, Color::White);
    }

    void RasterizerImp::clear_buffers()
    {
        std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
        std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
    }

    // This function is called at the end of rasterizing all elements of the
    // SVG file.  If you use a supersample buffer to rasterize SVG elements
    // for antialising, you could use this call to fill the target framebuffer
    // pixels from the supersample buffer data.
    //
    void RasterizerImp::resolve_to_framebuffer()
    {
        // TODO: Task 2: You will likely want to update this function for supersampling support
        int sqrt_sample_rate = int(sqrt(sample_rate));
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                Color col;
                for (int sx = x * sqrt_sample_rate; sx < (x + 1) * sqrt_sample_rate; sx++)
                    for (int sy = y * sqrt_sample_rate; sy < (y + 1) * sqrt_sample_rate; sy++)
                        col += sample_buffer[sy * width * sqrt_sample_rate + sx];
                col *= 1.0 / sample_rate;
                for (int k = 0; k < 3; ++k)
                {
                    this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&col.r)[k] * 255;
                }
            }
        }
    }

    Rasterizer::~Rasterizer() {}

} // CGL
