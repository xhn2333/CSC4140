#include "rasterizer.h"
#include <queue>
using namespace std;

namespace CGL
{
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
    void RasterizerImp::fill_pixel(size_t x, size_t y, Color c)
    {
        // TODO: Task 2: You might need to this function to fix points and lines (such as the black rectangle border in test4.svg)
        // NOTE: You are not required to implement proper supersampling for points and lines
        // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)

        sample_buffer[y * width + x] = c;
    }

    // Rasterize a point: simple example to help you start familiarizing
    // yourself with the starter code.
    //
    void RasterizerImp::rasterize_point(float x, float y, Color color)
    {
        // fill in the nearest pixel
        int sx = (int)floor(x);
        int sy = (int)floor(y);

        // check bounds
        if (sx < 0 || sx >= width)
            return;
        if (sy < 0 || sy >= height)
            return;

        fill_pixel(sx, sy, color);
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
            pt[0] += dpt[0];
            pt[1] += dpt[1];
        }
    }

    // Rasterize a triangle.

    bool RasterizerImp::point_in_triangle(float x0, float y0,
                                          float x1, float y1,
                                          float x2, float y2,
                                          RasterizerPoint p)
    {
        if (locationEdge(x0, y0, x1, y1, p) >= 0 && locationEdge(x1, y1, x2, y2, p) >= 0 && locationEdge(x2, y2, x0, y0, p) >= 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // locationEdge
    // >0   inside the edge
    // ==0  on the edge
    // <0   outside the edge
    float RasterizerImp::locationEdge(float x0, float y0,
                                      float x1, float y1,
                                      RasterizerPoint p)
    {
        float dx = x1 - x0;
        float dy = y1 - y0;
        float l = -(p.x - x0) * dy + (p.y - y0) * dx;
        return l;
    }

    void RasterizerImp::rasterize_triangle(float x0, float y0,
                                           float x1, float y1,
                                           float x2, float y2,
                                           Color color)
    {
        // TODO: Task 1: Implement basic triangle rasterization here, no supersampling

        RasterizerPoint midPoint;
        midPoint.x = int((x0 + x1 + x2) / 3) + 0.5, midPoint.y = int((y0 + y1 + y2) / 3) + 0.5;
        queue<RasterizerPoint> q;
        // map<pair<int, int>, Color> hashMap;
        bool hashMap[width][height]={0};
        vector<vector<float>> dir = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

        q.push(midPoint);
        for (int i = 0; i < 4; ++i)
        {
            RasterizerPoint tail;
            tail.x = midPoint.x + dir[i][0];
            tail.y = midPoint.y + dir[i][1];
            q.push(tail);
        }
        while (!q.empty())
        {
            RasterizerPoint head = q.front();
            q.pop();
            if (!hashMap[(int(head.x)%width+width)%width][(int(head.y)%height+height)%height])
            {
                if (point_in_triangle(x0, y0, x1, y1, x2, y2, head))
                {
                    rasterize_point(head.x - 0.5, head.y - 0.5, color);
                    hashMap[(int(head.x)%width+width)%width][(int(head.y)%height+height)%height] = true;
                    /*
                    hashMap.insert(
                        make_pair(
                            make_pair(int(floor(head.x)), int(floor(head.y))),
                            color));
                    */
                    for (int i = 0; i < 4; ++i)
                    {
                        RasterizerPoint tail;
                        tail.x = head.x + dir[i][0];
                        tail.y = head.y + dir[i][1];
                        q.push(tail);
                    }
                }
            }
        }
        // TODO: Task 2: Update to implement super-sampled rasterization
    }

    void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
                                                              float x1, float y1, Color c1,
                                                              float x2, float y2, Color c2)
    {
        // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
        // Hint: You can reuse code from rasterize_triangle
    }

    void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
                                                    float x1, float y1, float u1, float v1,
                                                    float x2, float y2, float u2, float v2,
                                                    Texture &tex)
    {
        // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
        // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
        // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle
    }

    void RasterizerImp::set_sample_rate(unsigned int rate)
    {
        // TODO: Task 2: You may want to update this function for supersampling support

        this->sample_rate = rate;

        this->sample_buffer.resize(width * height, Color::White);
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

        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                Color col = sample_buffer[y * width + x];

                for (int k = 0; k < 3; ++k)
                {
                    this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&col.r)[k] * 255;
                }
            }
        }
    }

    Rasterizer::~Rasterizer() {}

} // CGL
