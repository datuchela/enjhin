#include <float.h>
#include "raylib.h"
#include "raymath.h"

#ifndef FIZZIKS_H
#define FIZZIKS_H

extern const Color NODE_COLOR;
extern const float SIMULATION_SPEED;
extern const float MAX_VELOCITY_VALUE;
extern const float NODE_RADIUS;
extern const float SPRING_THICKNESS;
extern const float DEFAULT_SPRING_STIFFNESS;
extern const float DEFAULT_SPRING_DAMPENING;
extern const float FRICTION;

typedef struct Segment {
    Vector2 start;
    Vector2 end;
} Segment;

typedef struct BoundingRect {
    float top;
    float right;
    float bottom;
    float left;
    Vector2 top_left;
    Vector2 top_right;
    Vector2 bottom_right;
    Vector2 bottom_left;
} BoundingRect;

typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 force;
    float mass;
    bool is_colliding;
} Particle;

typedef struct Spring {
    Particle* particles[2];
    float stiffness;
    float rest_length;
} Spring;

typedef struct SoftBody {
    Spring* springs;
    Particle* particles;
    int springs_length;
    int particles_length;
    BoundingRect bounding_rect;
} SoftBody;

Spring CreateSpring(Particle *particle1, Particle *particle2, float stiffness);
void UpdateSpring(Spring *spring);
void DrawSpring(Spring *spring);
void DampenSpring(Spring *spring, Vector2 tension_direction);
void UpdateAllSprings(Spring* springs, int springs_length);
void DrawAllSprings(Spring* springs, int springs_length);

Particle CreateParticle(Vector2 position, float mass);
void UpdateParticle(Particle *particle, double dt);
void DrawParticle(Particle *particle);

void ResetParticleForces(Particle *particle);
void UpdateParticleAcceleration(Particle *particle, double dt);
void UpdateParticleVelocity(Particle *particle, double dt);
void ClampParticleVelocity(Particle *particle);
void AddParticleFriction(Particle *particle);
void UpdateParticlePosition(Particle *particle, double dt);

void ResetAllParticleForces(Particle* particles, int particles_length);
void UpdateAllParticleAccelerations(Particle* particles, int particles_length, double dt);
void UpdateAllParticleVelocities(Particle* particles, int particles_length, double dt);
void ClampAllParticleVelocities(Particle* particles, int particles_length);
void UpdateAllParticlePositions(Particle* particles, int particles_length, double dt);
void UpdateAllParticles(Particle* particles, int particles_length, double dt);
void DrawAllParticles(Particle* particles, int particles_length);

void DragParticleByMouse(Particle *particle, Vector2 mouse_position);
Particle* FindNearestParticleV(Vector2 position, SoftBody *soft_body);
void AttachMouseControls(SoftBody *soft_body, bool is_dragging);

SoftBody CreateSoftBody(Particle* particles, int particles_length, Spring* springs, int springs_length);
void UpdateSoftBody(SoftBody *soft_body, double dt);
void DrawSoftBody(SoftBody *soft_body);

void GetSoftBodySides(SoftBody *soft_body, Segment sides[]);
void UpdateSoftBodyBoundingRect(SoftBody *soft_body);
int GetPointToBodyIntersections(Vector2 *point, SoftBody *soft_body);
bool IsParticleIntersectingSoftBody(Particle *particle, SoftBody *soft_body);
void HandleCollisionSoftBodies(SoftBody *soft_body1, SoftBody *soft_body2);
void ResetSoftBodyCollisions(SoftBody *soft_body);
float Vector2CrossProduct(Vector2 vector1, Vector2 vector2);
int GetSegmentIntersectionAmount(Segment segment1, Segment segment2);

#endif /* FIZZIKS_H */
