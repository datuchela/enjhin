#include "fizziks.h"

void AttachMouseControls(SoftBody *soft_body, bool is_dragging)
{
    if(IsMouseButtonDown(0))
    {
        Particle *nearest_particle;
        Vector2 mouse_position = GetMousePosition();
        if(is_dragging == false)
        {
            nearest_particle = FindNearestParticleV(mouse_position, soft_body);
            is_dragging = true;
        }
        DragParticleByMouse(nearest_particle, mouse_position);
    } else {
        is_dragging = false;
    }
}

void GetSoftBodySides(SoftBody *soft_body, Segment *sides)
{
    Particle *particle;
    Particle *next_particle;
    for(int i = 0; i < soft_body->particles_length - 1; i++)
    {
        particle = &soft_body->particles[i];
        next_particle = &soft_body->particles[i+1];
        sides[i] = (Segment){particle->position, next_particle->position};
    }
    sides[soft_body->particles_length - 1] = (Segment){soft_body->particles[soft_body->particles_length - 1].position, soft_body->particles[0].position};
}

void UpdateSoftBodyBoundingRect(SoftBody *soft_body)
{
    float top = FLT_MAX;
    float right = FLT_MIN;
    float bottom = FLT_MIN;
    float left = FLT_MAX;

    Particle *particle;
    for(int i = 0; i < soft_body->particles_length; i++)
    {
        particle = &soft_body->particles[i];
        if(particle->position.y < top)
        {
            top = particle->position.y;
        }
        if(particle->position.x > right)
        {
            right = particle->position.x;
        }
        if(particle->position.y > bottom)
        {
            bottom = particle->position.y;
        }
        if(particle->position.x < left)
        {
            left = particle->position.x;
        }
    }

    soft_body->bounding_rect.top = top;
    soft_body->bounding_rect.right = right;
    soft_body->bounding_rect.bottom = bottom;
    soft_body->bounding_rect.left = left;
    soft_body->bounding_rect.top_left = (Vector2){left, top};
    soft_body->bounding_rect.top_right = (Vector2){right, top};
    soft_body->bounding_rect.bottom_right = (Vector2){right, bottom};
    soft_body->bounding_rect.bottom_left = (Vector2){left, bottom};
}

float Vector2CrossProduct(Vector2 vector1, Vector2 vector2)
{
    return vector1.x * vector2.y - vector1.y * vector2.x;
}

int GetSegmentIntersectionAmount(Segment segment1, Segment segment2)
{
    Vector2 A = segment1.start;
    Vector2 B = segment1.end;
    Vector2 C = segment2.start;
    Vector2 D = segment2.end;
    Vector2 AB = Vector2Subtract(B, A);
    Vector2 CD = Vector2Subtract(D, C);
    Vector2 AC = Vector2Subtract(C, A);

    float common_denominator = Vector2CrossProduct(CD, AB);
    if(common_denominator == 0) return false;

    float t = Vector2CrossProduct(CD, AC) / common_denominator;
    Vector2 M = Vector2Add(A, Vector2Scale(AB, t));

    bool is_inside_segment = 0 < t && t < 1;

    bool is_on_segment_down_right = C.x <= M.x && M.x <= D.x && C.y <= M.y && M.y <= D.y;
    bool is_on_segment_down_left = C.x >= M.x && M.x >= D.x && C.y <= M.y && M.y <= D.y;
    bool is_on_segment_up_left = C.x >= M.x && M.x >= D.x && C.y >= M.y && M.y >= D.y;
    bool is_on_segment_up_right = C.x <= M.x && M.x <= D.x && C.y >= M.y && M.y >= D.y;

    if(
        is_inside_segment &
        (is_on_segment_down_right |
        is_on_segment_down_left |
        is_on_segment_up_left |
        is_on_segment_up_right)
    )
    {
        return 1;
    }

    return 0;
}

int GetPointToBodyIntersections(Vector2 *point, SoftBody *soft_body)
{
    int intersections = 0;
    UpdateSoftBodyBoundingRect(soft_body);
    BoundingRect rect = soft_body->bounding_rect;
    Vector2 right_point = {rect.right + 10, point->y};
    Segment horizontal_segment = {*point, right_point};

    int sides_length = soft_body->particles_length;
    Segment sides[sides_length] = {};
    GetSoftBodySides(soft_body, sides);
    for(int i = 0; i < sides_length; i++)
    {
        intersections += GetSegmentIntersectionAmount(horizontal_segment, sides[i]);
    }
    return intersections;
}

void ResetSoftBodyCollisions(SoftBody *soft_body)
{
    for(int i = 0; i < soft_body->particles_length; i++)
    {
        soft_body->particles[i].is_colliding = false;
    }
}

bool IsParticleIntersectingSoftBody(Particle *particle, SoftBody *soft_body)
{ 
    return GetPointToBodyIntersections(&particle->position, soft_body) % 2 == 1;
}

void HandleCollisionSoftBodies(SoftBody *soft_body1, SoftBody *soft_body2)
{
    Particle *particle;
    for(int i = 0; i < soft_body1->particles_length; i++)
    {
        particle = &soft_body1->particles[i]; 
        particle->is_colliding = IsParticleIntersectingSoftBody(particle, soft_body2);
    }
}

Spring CreateSpring(Particle *particle1, Particle *particle2, float stiffness)
{
    float rest_length = Vector2Distance(particle1->position, particle2->position);
    Spring spring = {{particle1, particle2}, stiffness, rest_length}; 
    return spring;
}

void UpdateSpring(Spring *spring)
{
    Vector2 spring_vector = Vector2Subtract(spring->particles[0]->position, spring->particles[1]->position);
    float spring_length = Vector2Length(spring_vector);
    float delta_length = spring_length - spring->rest_length;
    Vector2 tension_direction = Vector2Normalize(spring_vector);

    Vector2 tension0 = Vector2Scale(tension_direction, -delta_length);
    Vector2 tension1 = Vector2Scale(tension_direction, delta_length);
    
    spring->particles[0]->force = Vector2Add(spring->particles[0]->force, Vector2Scale(tension0, spring->stiffness));
    spring->particles[1]->force = Vector2Add(spring->particles[1]->force, Vector2Scale(tension1, spring->stiffness));

    DampenSpring(spring, tension_direction);
}

void DrawSpring(Spring *spring)
{
    DrawLineEx(spring->particles[0]->position, spring->particles[1]->position, SPRING_THICKNESS, RAYWHITE);
}

void DampenSpring(Spring *spring, Vector2 tension_direction)
{
    Vector2 relative_velocity = Vector2Subtract(spring->particles[1]->velocity, spring->particles[0]->velocity);
    Vector2 dampening_force = Vector2Scale(tension_direction, Vector2DotProduct(tension_direction, relative_velocity) * DEFAULT_SPRING_DAMPENING);

    spring->particles[0]->force = Vector2Add(spring->particles[0]->force, dampening_force);
    spring->particles[1]->force = Vector2Add(spring->particles[1]->force, Vector2Scale(dampening_force, -1));
}

void UpdateAllSprings(Spring* springs, int springs_length)
{
    for(int i = 0; i < springs_length; i++)
    {
        UpdateSpring(&springs[i]);
    }
}

void DrawAllSprings(Spring *springs, int springs_length)
{
    for(int i = 0; i < springs_length; i++)
    {
        DrawSpring(&springs[i]);
    }
}

Particle CreateParticle(Vector2 position, float mass)
{
    Particle particle = {position, Vector2Zero(), Vector2Zero(), Vector2Zero(), mass, false};
    return particle;
}

void UpdateParticle(Particle *particle, double dt)
{
    UpdateParticleAcceleration(particle, dt);
    UpdateParticleVelocity(particle, dt);
    UpdateParticlePosition(particle, dt);
}

void DrawParticle(Particle *particle)
{
    if(particle->is_colliding == true)
    {
        return DrawCircleV(particle->position, NODE_RADIUS, RED);
    }
    DrawCircleV(particle->position, NODE_RADIUS, NODE_COLOR);
}

void ResetParticleForces(Particle *particle)
{
    particle->force = Vector2Zero();
}

void UpdateParticleAcceleration(Particle *particle, double dt)
{
    particle->acceleration = Vector2Scale(particle->force, 1/(particle->mass));
}

void UpdateParticleVelocity(Particle *particle, double dt)
{
    Vector2 velocity_displacement = Vector2Scale(particle->acceleration, dt);
    particle->velocity = Vector2Add(particle->velocity, velocity_displacement);
    ClampParticleVelocity(particle);
    AddParticleFriction(particle);
}

void AddParticleFriction(Particle *particle)
{
    particle->velocity = Vector2Scale(particle->velocity, 1 - FRICTION);
}

void ClampParticleVelocity(Particle *particle)
{
    particle->velocity = Vector2ClampValue(particle->velocity, -MAX_VELOCITY_VALUE, MAX_VELOCITY_VALUE);
}

void UpdateParticlePosition(Particle *particle, double dt)
{
    Vector2 position_displacement = Vector2Scale(particle->velocity, dt);
    particle->position = Vector2Add(particle->position, position_displacement);
}

void DrawAllParticles(Particle *particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        DrawParticle(&particles[i]);
    }
}

void UpdateAllParticles(Particle *particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticle(&particles[i], dt);
    }
}

void ResetAllParticleForces(Particle *particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        ResetParticleForces(&particles[i]);
    }
}

void UpdateAllParticleAccelerations(Particle *particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticleAcceleration(&particles[i], dt);
    }
}

void UpdateAllParticleVelocities(Particle *particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticleVelocity(&particles[i], dt);
    }
}

void ClampAllParticleVelocities(Particle *particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        ClampParticleVelocity(&particles[i]);
    }
}

void UpdateAllParticlePositions(Particle *particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticlePosition(&particles[i], dt);
    }
}

Particle* FindNearestParticleV(Vector2 position, SoftBody *soft_body)
{
    float closest_distance = FLT_MAX;
    Particle* nearest_particle;
    for(int i = 0; i < soft_body->particles_length; i++)
    {
        float curr_distance = Vector2Distance(soft_body->particles[i].position, position);
        if(curr_distance < closest_distance)
        {
            closest_distance = curr_distance;
            nearest_particle = &soft_body->particles[i];
        }
    }
    return nearest_particle;
}

void DragParticleByMouse(Particle *particle, Vector2 mouse_position)
{
    particle->acceleration = Vector2Zero();
    particle->velocity = Vector2Zero();
    particle->position = mouse_position;
}

SoftBody CreateSoftBody(Particle *particles, int particles_length, Spring *springs, int springs_length)
{
    SoftBody soft_body;
    soft_body.particles = particles;
    soft_body.springs = springs;
    soft_body.particles_length = particles_length;
    soft_body.springs_length = springs_length;
    return soft_body;
}

void UpdateSoftBody(SoftBody *soft_body, double dt)
{
    ResetAllParticleForces(soft_body->particles, soft_body->particles_length);
    UpdateAllSprings(soft_body->springs, soft_body->springs_length);
    UpdateAllParticles(soft_body->particles, soft_body->particles_length, dt);
}

void DrawSoftBody(SoftBody *soft_body)
{
    for(int i = 0; i < soft_body->springs_length; i++)
    {
        DrawSpring(&soft_body->springs[i]);
        DrawParticle(soft_body->springs[i].particles[0]);
        DrawParticle(soft_body->springs[i].particles[1]);
    }
}
