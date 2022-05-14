#include <algorithm>
#include <iostream>
#include <utility>

#include "application/visual_debugger.h"
#include "bsdf.h"

using std::max;
using std::min;
using std::swap;

namespace CGL {

// Mirror BSDF //

Vector3D MirrorBSDF::f(const Vector3D wo, const Vector3D wi) {
    return Vector3D();
}

Vector3D MirrorBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {
    // TODO Assignment 7: Part 1
    // Implement MirrorBSDF

    BSDF::reflect(wo, wi);
    *pdf = 1;
    return reflectance / abs_cos_theta(*wi);
}

void MirrorBSDF::render_debugger_node() {
    if (ImGui::TreeNode(this, "Mirror BSDF")) {
        DragDouble3("Reflectance", &reflectance[0], 0.005);
        ImGui::TreePop();
    }
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D wo, const Vector3D wi) {
    return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D h) {
    // TODO Assignment 7: Part 2
    // Compute Beckmann normal distribution function (NDF) here.
    // You will need the roughness alpha.
    double theta_h = getTheta(h);

    return exp(-(tan(theta_h) * tan(theta_h)) / (alpha * alpha)) /
           (PI * alpha * alpha * cos(theta_h) * cos(theta_h) * cos(theta_h) *
            cos(theta_h));
}

Vector3D MicrofacetBSDF::F(const Vector3D wi) {
    // TODO Assignment 7: Part 2
    // Compute Fresnel term for reflection on dielectric-conductor interface.
    // You will need both eta and etaK, both of which are Vector3D.
    Vector3D rs, rp;
    Vector3D sqsum = (eta * eta + k * k);
    double theta_wi = getTheta(wi);
    rs = (sqsum - 2 * eta * cos(theta_wi) + cos(theta_wi) * cos(theta_wi)) /
         (sqsum + 2 * eta * cos(theta_wi) + cos(theta_wi) * cos(theta_wi));
    rp = (sqsum * cos(theta_wi) * cos(theta_wi) - 2 * eta * cos(theta_wi) + 1) /
         (sqsum * cos(theta_wi) * cos(theta_wi) + 2 * eta * cos(theta_wi) + 1);
    return (rs + rp) / 2;
}

Vector3D MicrofacetBSDF::f(const Vector3D wo, const Vector3D wi) {
    // TODO Assignment 7: Part 2
    // Implement microfacet model here.
    Vector3D h = (wi + wo);
    h.normalize();
    if (wo.z >= 0 && wi.z >= 0)
        return MicrofacetBSDF::F(wi) * MicrofacetBSDF::G(wo, wi) *
               MicrofacetBSDF::D(h) / (4 * wo.z * wi.z);
    else
        return {};
}

Vector3D MicrofacetBSDF::sample_f(const Vector3D wo, Vector3D* wi,
                                  double* pdf) {
    // TODO Assignment 7: Part 2
    // *Importance* sample Beckmann normal distribution function (NDF) here.
    // Note: You should fill in the sampled direction *wi and the corresponding
    // *pdf,
    //       and return the sampled BRDF value.
    Vector2D r = sampler.get_sample();
    double theta_h = atan(sqrt(-alpha * alpha * log(1 - r.x)));
    double phi_h = 2 * PI * r.y;

    double p_theta =
        (2 * sin(theta_h)) /
        (alpha * alpha * cos(theta_h) * cos(theta_h) * cos(theta_h)) *
        exp((-tan(theta_h) * tan(theta_h)) / alpha / alpha);
    double p_phi = 1 / (2 * PI);

    double p_w = p_theta * p_phi / sin(theta_h);
    Vector3D h = Vector3D(sin(theta_h) * cos(phi_h), sin(theta_h) * sin(phi_h),
                          cos(theta_h));
    *wi = -wo + 2 * dot(wo, h) * h;
    if (wi->z > 0 && wo.z > 0) {
        *pdf = p_w / (4 * dot((*wi), h));
        return MicrofacetBSDF::f(wo, *wi);
    }
    // *wi = cosineHemisphereSampler.get_sample(pdf);
    return {};
}

void MicrofacetBSDF::render_debugger_node() {
    if (ImGui::TreeNode(this, "Micofacet BSDF")) {
        DragDouble3("eta", &eta[0], 0.005);
        DragDouble3("K", &k[0], 0.005);
        DragDouble("alpha", &alpha, 0.005);
        ImGui::TreePop();
    }
}

// Refraction BSDF //

Vector3D RefractionBSDF::f(const Vector3D wo, const Vector3D wi) {
    return Vector3D();
}

Vector3D RefractionBSDF::sample_f(const Vector3D wo, Vector3D* wi,
                                  double* pdf) {
    // TODO Assignment 7: Part 1
    // Implement RefractionBSDF
    bool reflect_flag = BSDF::refract(wo, wi, ior);
    double eta;
    if (wo.z > 0) {
        eta = 1.0 / ior;
    } else {
        eta = ior;
    }
    if (reflect_flag) {
        *pdf = 1;
        return transmittance / abs_cos_theta(*wi) / (eta * eta);
    } else
        return {};
}

void RefractionBSDF::render_debugger_node() {
    if (ImGui::TreeNode(this, "Refraction BSDF")) {
        DragDouble3("Transmittance", &transmittance[0], 0.005);
        DragDouble("ior", &ior, 0.005);
        ImGui::TreePop();
    }
}

// Glass BSDF //

Vector3D GlassBSDF::f(const Vector3D wo, const Vector3D wi) {
    return Vector3D();
}

Vector3D GlassBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {
    // TODO Assignment 7: Part 1
    // Compute Fresnel coefficient and either reflect or refract based on it.

    // compute Fresnel coefficient and use it as the probability of reflection
    // - Fundamentals of Computer Graphics page 305
    bool reflect_flag = BSDF::refract(wo, wi, ior);

    if (!reflect_flag) {
        *pdf = 1;
        BSDF::reflect(wo, wi);
        return transmittance / abs_cos_theta(*wi);
    } else {
        double cos_theta = abs_cos_theta(wo);
        double ratio = (ior - 1) / (ior + 1);
        double r0 = ratio * ratio;
        double r5 = (1 - cos_theta) * (1 - cos_theta) * (1 - cos_theta) *
                    (1 - cos_theta) * (1 - cos_theta);
        double R = r0 + (1 - r0) * r5;
        if (coin_flip(R)) {
            BSDF::reflect(wo, wi);
            *pdf = R;
            return R * reflectance / abs_cos_theta(*wi);
        } else {
            BSDF::refract(wo, wi, ior);
            *pdf = 1 - R;
            double eta;
            if (wo.z > 0) {
                eta = 1.0 / ior;
            } else {
                eta = ior;
            }
            return (1 - R) * transmittance / abs_cos_theta(*wi) / eta / eta;
        }
    }
}

void GlassBSDF::render_debugger_node() {
    if (ImGui::TreeNode(this, "Refraction BSDF")) {
        DragDouble3("Reflectance", &reflectance[0], 0.005);
        DragDouble3("Transmittance", &transmittance[0], 0.005);
        DragDouble("ior", &ior, 0.005);
        ImGui::TreePop();
    }
}

void BSDF::reflect(const Vector3D wo, Vector3D* wi) {
    // TODO Assignment 7: Part 1
    // Implement reflection of wo about normal (0,0,1) and store result in wi.
    *wi = Vector3D(-wo.x, -wo.y, wo.z);
}

bool BSDF::refract(const Vector3D wo, Vector3D* wi, double ior) {
    // TODO Assignment 7: Part 1
    // Use Snell's Law to refract wo surface and store result ray in wi.
    // Return false if refraction does not occur due to total internal
    // reflection and true otherwise. When dot(wo,n) is positive, then wo
    // corresponds to a ray entering the surface through vacuum.
    double eta;
    if (wo.z > 0) {
        eta = 1.0 / ior;
    } else {
        eta = ior;
    }
    if (1 - eta * eta * (1 - wo.z * wo.z) > 0) {
        wi->x = -eta * wo.x;
        wi->y = -eta * wo.y;
        wi->z = (wo.z > 0 ? -1 : 1) * sqrt(1 - eta * eta * (1 - wo.z * wo.z));
        return true;
    }
    return false;
}

}  // namespace CGL
