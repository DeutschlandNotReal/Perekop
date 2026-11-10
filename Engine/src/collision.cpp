#include <PK/Core/array.hpp>
#include <PK/Physics/collision.hpp>
#include <PK/Physics/overlap.hpp>
#include <PK/world.hpp>
#include <PK/Math/clamp.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

using namespace pk;
using namespace pk::physics;

constexpr float eps  = 1e-5f;
constexpr float eps2 = eps * eps;
constexpr float inf  = std::numeric_limits<float>::infinity();
constexpr vec3 xaxis{1,0,0}, 
               yaxis{0,1,0},
               zaxis{0,0,1};

float len2(vec3 v) noexcept { return dot(v, v); }

overlapResult physics::getOverlap(const Body& bodyA, const Body& bodyB, AABB boundA, AABB boundB) noexcept {

    if (bodyA.shapeType == shapeType::precise || bodyB.shapeType == shapeType::precise) {
        return {};
    }

    vec3 hA = boundA.getExtents(), hB = boundB.getExtents();
    if (bodyA.shapeType == shapeType::box && bodyB.shapeType == shapeType::box) {
        pose disp = localspace(bodyA.pose, bodyB.pose);
        mat3 R = (mat3) disp;
        vec3 l = normalize(R[0]), 
             r = normalize(R[1]), 
             u = normalize(R[2]);

        vec3 axes[15] = {
            xaxis, yaxis, zaxis,
            l, r, u,
            
            // cross products
            {0, -l.z, l.y}, {0, -r.z, r.y}, {0, -u.z, u.y},
            {l.z, 0, -l.x}, {r.z, 0, -r.x}, {u.z, 0, -u.x},
            {-l.y, l.x, 0}, {-r.y, r.x, 0}, {-u.y, u.x, 0}
        };

        float minO{inf}; vec3 minaxis;

        mat3 tR = transpose(R);
        for (uint i{0}; i < 15; i++) {
            vec3 axis{axes[i]};
            float axisLength2 = dot(axis, axis);
            if (axisLength2 < eps2) continue;
            axis *= 1.f / sqrt(axisLength2);

            // cheeky analytical way rather than trying every Vertex for cube
            float extOA = dot(hA, abs(axis));
            float extOB = dot(hB, abs(tR * axis));

            float dst = abs(dot(disp.pos, axis));
            float overlap = (extOA + extOB) - dst;

            if (overlap < -eps) return {};

            if (overlap < minO) { minaxis = axis; minO = overlap; }
        }

        return {worldspace(minaxis, bodyA.pose.rot), minO};
    } else if (bodyA.shapeType == shapeType::ball && bodyB.shapeType == shapeType::ball) {
        float r0 = length(hA), r1 = length(hB);

        vec3 disp = (bodyA.pose - bodyB.pose).pos;
        float r = r0 + r1;
        float dst2 = dot(disp, disp);

        if (dst2 > r * r + eps2) return {};
        float dst = sqrt(dst2); 
        vec3 axis = dst2 > eps2 ? disp / dst : xaxis;

        return {axis, r - dst};
    } else {
        auto f = ([](const Body& box, const Body& ball, AABB boxext, AABB ballext)->overlapResult{
            pose rel = localspace(ball.pose, box.pose);

            vec3 h = boxext.getExtents();
            vec3 near = clamp(rel.pos, -h, h);

            if (dot(near, near) < eps2) near = vec3{eps};
            
            vec3 axes[4] = {xaxis, yaxis, zaxis, normalize(near)};

                
            float minO{inf};
            vec3 minaxis;

            
            for (uint i{0}; i < 4; i++) {
                vec3 axis = axes[i];

                float extBox = dot(h, abs(axis));
                float dst = dot(rel.pos, axis);
                float overlap = (extBox + length(ballext.getExtents())) - abs(dst);
      
                if (overlap < -eps) return {};

                if (overlap < minO) { 
                    minO = overlap; 
                    minaxis = (dst < 0.f) ? -axis : axis;
                }
            }

            return {worldspace(minaxis, box.pose.rot), minO};
        });

        return bodyA.shapeType == shapeType::box ? f(bodyA, bodyB, boundA, boundB) : f(bodyB, bodyA, boundB, boundA);
    }
}

void physics::collide(Body &A, Body &B, float) noexcept {
    overlapResult result = getOverlap(A, B, A.box, B.box);
    if (result.overlap < eps) return;

    float invMassSum = A.iMass + B.iMass;
    if (invMassSum <= 0.f) return;

    vec3 centerDelta = B.pose.pos - A.pose.pos;
    vec3 n = len2(result.axis) > eps2 ? normalize(result.axis) : xaxis;
    
    float penetration = pk::max(result.overlap - eps * 100.f, 0.f);
    vec3 correction = n * (.8f * penetration / invMassSum);
    A.pose -= correction * A.iMass;
    B.pose += correction * B.iMass;

    mat3 invIA{0.f}, invIB{0.f};

    if (A.iMass > 0.f) invIA = A.getWorldInertia();

    if (B.iMass > 0.f) invIB = B.getWorldInertia();

    vec3 rA = centerDelta * .5f;
    vec3 rB = -centerDelta * .5f;

    vec3 contactPos = A.pose.pos + rA;

    vec3 velA = A.getVelAt(contactPos);
    vec3 velB = B.getVelAt(contactPos);
    vec3 relativeVel = velB - velA;

    float normalVelocity = dot(relativeVel, n);

    if (normalVelocity < 0.f) {
        float restitution = 0.1f; 

        vec3 raCrossN = cross(rA, n);
        vec3 rbCrossN = cross(rB, n);
        
        vec3 angularA = cross(invIA * raCrossN, rA);
        vec3 angularB = cross(invIB * rbCrossN, rB);

        float denom = A.iMass + B.iMass + dot(angularA + angularB, n);

        if (denom > eps) {
            float j = -(1.0f + restitution) * normalVelocity / denom;
            vec3 impulse = n * j;

            A.vel -= impulse * A.iMass;
            A.avel -= invIA * cross(rA, impulse);

            B.vel += impulse * B.iMass;
            B.avel += invIB * cross(rB, impulse);
        }
    }
}