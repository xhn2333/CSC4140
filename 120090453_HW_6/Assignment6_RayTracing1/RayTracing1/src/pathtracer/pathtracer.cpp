#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"

using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
    gridSampler = new UniformGridSampler2D();
    hemisphereSampler = new UniformHemisphereSampler3D();

    tm_gamma = 2.2f;
    tm_level = 1.0f;
    tm_key = 0.18;
    tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
    delete gridSampler;
    delete hemisphereSampler;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
    sampleBuffer.resize(width, height);
    sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
    bvh = NULL;
    scene = NULL;
    camera = NULL;
    sampleBuffer.clear();
    sampleCountBuffer.clear();
    sampleBuffer.resize(0, 0);
    sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
    sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Vector3D PathTracer::estimate_direct_lighting_hemisphere(
    const Ray &r, const Intersection &isect) {
    // Estimate the lighting from this intersection coming directly from a
    // light. For this function, sample uniformly in a hemisphere.

    // Note: When comparing Cornel Box (CBxxx.dae) results to importance
    // sampling, you may find the "glow" around the light source is gone. This
    // is totally fine: the area lights in importance sampling has
    // directionality, however in hemisphere sampling we don't model this
    // behaviour.

    // make a coordinate system for a hit point
    // with N aligned with the Z direction.
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    // w_out points towards the source of the ray (e.g.,
    // toward the camera if this is a primary ray)
    const Vector3D hit_p = r.o + r.d * isect.t;
    const Vector3D w_out = w2o * (-r.d);

    // This is the same number of total samples as
    // estimate_direct_lighting_importance (outside of delta lights). We keep
    // the same number of samples for clarity of comparison.
    int num_samples = scene->lights.size() * ns_area_light;
    Vector3D L_out(0, 0, 0);

    // TODO (Part 3): Write your sampling loop here
    // TODO BEFORE YOU BEGIN
    // UPDATE `est_radiance_global_illumination` to return direct lighting
    // instead of normal shading
    Vector3D w_in;
    double w_cos_theta;
    for (int i = 0; i < num_samples; i++) {
        w_in = hemisphereSampler->get_sample();
        w_cos_theta = cos_theta(w_in);

        Vector3D d_next = o2w * w_in;
        Vector3D o_next = hit_p;
        Ray r_next = Ray(o_next, d_next);
        r_next.min_t = EPS_F;

        Intersection i_next;

        if (bvh->intersect(r_next, &i_next)) {
            Vector3D f = isect.bsdf->f(w_out, w_in);
            Vector3D L = i_next.bsdf->get_emission();
            L_out += L * f * w_cos_theta;
        }
    }
    L_out /= num_samples;
    return L_out;
}

Vector3D PathTracer::estimate_direct_lighting_importance(
    const Ray &r, const Intersection &isect) {
    // Estimate the lighting from this intersection coming directly from a
    // light. To implement importance sampling, sample only from lights, not
    // uniformly in a hemisphere.

    // make a coordinate system for a hit point
    // with N aligned with the Z direction.
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    // w_out points towards the source of the ray (e.g.,
    // toward the camera if this is a primary ray)
    const Vector3D hit_p = r.o + r.d * isect.t;
    const Vector3D w_out = w2o * (-r.d);

    Vector3D L_out;

    for (auto light : scene->lights) {
        int num_samples = 0;
        Vector3D w_in;
        Vector3D dir;
        Vector3D L;
        Vector3D L_light;
        double dist2Light = 0;
        double pdf;
        double w_cos_theta;

        for (int i = 0; i < ns_area_light; i++) {
            L = light->sample_L(hit_p, &w_in, &dist2Light, &pdf);
            w_cos_theta = dot(w_in, isect.n);
            dir = w2o * w_in;
            Vector3D d_next = w_in;
            Vector3D o_next = hit_p;

            Ray r_next = Ray(o_next, d_next);
            r_next.min_t = EPS_F;
            r_next.max_t = (double)(dist2Light - EPS_F);

            Intersection i_next;
            if (!(this->bvh->intersect(r_next, &i_next))) {
                Vector3D f = isect.bsdf->f(w_out, dir);
                L_light += L * f * w_cos_theta / pdf;
            }
            num_samples++;

            if (light->is_delta_light()) {
                break;
            }
        }
        L_out += L_light / num_samples;
    }
    return L_out;
}
Vector3D PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
    // TODO: Part 3, Task 2
    // Returns the light that results from no bounces of light

    return isect.bsdf->get_emission();
}

Vector3D PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
    // TODO: Part 3, Task 3
    // Returns either the direct illumination by hemisphere or importance
    // sampling depending on `direct_hemisphere_sample`
    if (direct_hemisphere_sample) {
        return estimate_direct_lighting_hemisphere(r, isect);
    } else {
        return estimate_direct_lighting_importance(r, isect);
    }
}

Vector3D PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    Vector3D hit_p = r.o + r.d * isect.t;
    Vector3D w_out = w2o * (-r.d);

    Vector3D L_out(0, 0, 0);

    // TODO: Part 4, Task 2
    // Returns the one bounce radiance + radiance from extra bounces at this
    // point. Should be called recursively to simulate extra bounces.

    L_out += one_bounce_radiance(r, isect);

    double cpdf = 0.65;

    if (coin_flip(cpdf)) {
        Vector3D w_in;
        double pdf;

        Vector3D f = isect.bsdf->sample_f(w_out, &w_in, &pdf);
        double w_cos_theta = cos_theta(w_in);

        Vector3D d_next = o2w * w_in;
        Vector3D o_next = hit_p;

        Ray r_next = Ray(o_next, d_next);
        r_next.depth = r.depth - 1;
        r_next.min_t = EPS_F;
        Intersection i_next;

        if (r_next.depth > 1 && w_cos_theta > 0 &&
            bvh->intersect(r_next, &i_next)) {
            Vector3D L = at_least_one_bounce_radiance(r_next, i_next);
            L_out += L * f * w_cos_theta / pdf / cpdf;
        }
    }

    return L_out;
}

Vector3D PathTracer::est_radiance_global_illumination(const Ray &r) {
    Intersection isect;
    Vector3D L_out;

    // You will extend this in assignment 3-2.
    // If no intersection occurs, we simply return black.
    // This changes if you implement hemispherical lighting for extra
    // credit.

    // The following line of code returns a debug color depending
    // on whether ray intersection with triangles or spheres has
    // been implemented.
    //
    // REMOVE THIS LINE when you are ready to begin Part 3.

    if (!bvh->intersect(r, &isect))
        return envLight ? envLight->sample_dir(r) : L_out;

    L_out = (isect.t == INF_D) ? debug_shading(r.d) : normal_shading(isect.n);

    // TODO (Part 3): Return the direct illumination.

    L_out = zero_bounce_radiance(r, isect);
    L_out += at_least_one_bounce_radiance(r, isect);
    // TODO (Part 4): Accumulate the "direct" and "indirect"
    // parts of global illumination into L_out rather than just direct

    return L_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {
    // TODO (Part 1.2):
    // Make a loop that generates num_samples camera rays and traces them
    // through the scene. Return the average Vector3D.
    // You should call est_radiance_global_illumination in this function.

    // TODO (Part 5):
    // Modify your implementation to include adaptive sampling.
    // Use the command line parameters "samplesPerBatch" and "maxTolerance"

    int num_samples = ns_aa;           // total samples to evaluate
    Vector2D origin = Vector2D(x, y);  // bottom left corner of the pixel
    Vector3D sample = Vector3D(0, 0, 0);

    Vector2D rs = Vector2D();
    Vector2D ps = Vector2D();
    Ray r = Ray();
    int batch = 0;
    int n = 0;
    double mu;
    double s1 = 0, s2 = 0;
    double var;
    double I;
    Vector3D radiance;

    for (int i = 0; i < num_samples; i++, batch++, n++) {
        if (batch == samplesPerBatch) {
            batch = 0;
            mu = s1 / double(n);
            var = 1.0 / (double(n) - 1.0) * (s2 - s1 * s1 / double(n));
            I = 1.96 * sqrt(var / double(n));
            if (I <= maxTolerance * mu) {
                break;
            }
        }
        rs = gridSampler->get_sample();
        ps = origin + rs;
        r = camera->generate_ray(1.0 * ps.x / sampleBuffer.w,
                                 1.0 * ps.y / sampleBuffer.h);
        r.depth = max_ray_depth;
        radiance = est_radiance_global_illumination(r);
        double illum = radiance.illum();
        s1 += illum;
        s2 += illum * illum;
        sample += radiance;
    }
    sample /= n;
    sampleBuffer.update_pixel(sample, x, y);
    sampleCountBuffer[x + y * sampleBuffer.w] = n;
}

void PathTracer::autofocus(Vector2D loc) {
    Ray r =
        camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
    Intersection isect;

    bvh->intersect(r, &isect);

    camera->focalDistance = isect.t;
}

}  // namespace CGL
