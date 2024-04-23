#include <float.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

const float MAX_VELOCITY_VALUE = 250;
const float NODE_RADIUS = 5.0;
const float DEFAULT_SPRING_STIFFNESS = 200.0;
const float DEFAULT_SPRING_DAMPENING = 70.0;
const float FRICTION = 0.005;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 force;
    float mass;
} Particle;

typedef struct {
    Particle* particles[2];
    float stiffness;
    float rest_length;
} Spring;

void DEBUG_Draw_Particle_Stats(Particle* particles, int particles_length);

Spring CreateSpring(Particle *particle1, Particle *particle2, float stiffness);
void DrawSpring(Spring *spring);
void UpdateSpring(Spring *spring);
void DampenSpring(Spring *spring, Vector2 tension_direction);
void UpdateAllSprings(Spring* spring, int springs_length);
void DrawAllSprings(Spring* spring, int springs_length);

Particle CreateParticle(Vector2 position, Vector2 velocity, Vector2 acceleration, Vector2 force, float mass);
void DrawParticle(Particle *particle);
void UpdateParticlePosition(Particle *particle, double dt);
void UpdateParticleVelocity(Particle *particle, double dt);
void ClampParticleVelocity(Particle *particles);
void UpdateParticleAcceleration(Particle *particle, double dt);
void ResetParticleForces(Particle *particles);

void DrawAllParticles(Particle* particles, int particles_length);
void UpdateAllParticlePositions(Particle* particles, int particles_length, double dt);
void UpdateAllParticleVelocities(Particle* particles, int particles_length, double dt);
void UpdateAllParticleAccelerations(Particle* particles, int particles_length, double dt);
void ClampAllParticleVelocities(Particle* particles, int particles_length);
void ResetAllParticleForces(Particle* particles, int particles_length);

Particle* FindNearestParticleV(Vector2 position, Particle* particles, int particles_length);
void DragParticleByMouse(Particle *particle, Vector2 mouse_position, bool *is_dragging);

int main(int _argc, char *_argv[]) 
{
    Particle particles[4] = {};
    Spring springs[6] = {};

    particles[0] = CreateParticle
        (
            (Vector2) {100.0, 100.0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            10.0
        );
    particles[1] = CreateParticle
        (
            (Vector2) {150.0, 100.0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            10.0
        );
    particles[2] = CreateParticle
        (
            (Vector2) {150.0, 150.0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            10.0
        );
    particles[3] = CreateParticle
        (
            (Vector2) {100.0, 150.0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            10.0
        );

    springs[0] = CreateSpring(&particles[0], &particles[1], DEFAULT_SPRING_STIFFNESS);
    springs[1] = CreateSpring(&particles[0], &particles[2], DEFAULT_SPRING_STIFFNESS);
    springs[2] = CreateSpring(&particles[0], &particles[3], DEFAULT_SPRING_STIFFNESS);
    springs[3] = CreateSpring(&particles[1], &particles[2], DEFAULT_SPRING_STIFFNESS);
    springs[4] = CreateSpring(&particles[1], &particles[3], DEFAULT_SPRING_STIFFNESS);
    springs[5] = CreateSpring(&particles[2], &particles[3], DEFAULT_SPRING_STIFFNESS);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(60);
    SetWindowMonitor(0);

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    int particles_length = sizeof(particles) / sizeof(particles[0]);
    int springs_length = sizeof(springs) / sizeof(springs[0]);

    double time_now = 0;
    double time_prev;
    float simulation_speed = 1.0;

    bool is_dragging = false;
    Particle* nearest_particle;

    while (!WindowShouldClose())
    {
        time_prev = time_now;
        time_now = GetTime();
        double dt = (time_now - time_prev) * simulation_speed;

        if(IsMouseButtonDown(0))
        {
            Vector2 mouse_position = GetMousePosition();
            if(is_dragging == false)
            {
                nearest_particle = FindNearestParticleV(mouse_position, particles, particles_length);
            }
            DragParticleByMouse(nearest_particle, mouse_position, &is_dragging);
        }

        if(IsMouseButtonReleased(0))
        {
            is_dragging = false;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawFPS(10, 10);

        UpdateAllSprings(springs, springs_length);
        UpdateAllParticleAccelerations(particles, particles_length, dt);
        UpdateAllParticleVelocities(particles, particles_length, dt);
        ClampAllParticleVelocities(particles, particles_length);
        UpdateAllParticlePositions(particles, particles_length, dt);

        DrawAllSprings(springs, springs_length);
        DrawAllParticles(particles, particles_length);
        DEBUG_Draw_Particle_Stats(particles, particles_length);

        ResetAllParticleForces(particles, particles_length);

        EndDrawing();
    }
    return 0;
}

void DEBUG_Draw_Particle_Stats(Particle* particles, int particles_length)
{
    int line_height = 20;
    int gap = 70;
    int position_left = WINDOW_WIDTH - 200;

    for(int i = 0; i < particles_length; i++)
    {
        DrawText(TextFormat("p%i x: %.1f y: %.1f",i, particles[i].position.x, particles[i].position.y), position_left, line_height + i*gap, 16, WHITE);
        DrawText(TextFormat("vx: %.1f vy: %.1f", particles[i].velocity.x, particles[i].velocity.y), position_left, 2*line_height + i*gap, 16, WHITE);
        DrawText(TextFormat("ax: %.1f ay: %.1f", particles[i].acceleration.x, particles[i].acceleration.y), position_left, 3*line_height + i*gap, 16, WHITE);
    }
}


Spring CreateSpring(Particle *particle1, Particle *particle2, float stiffness)
{
    float rest_length = Vector2Distance(particle1->position, particle2->position);
    Spring spring = {{particle1, particle2}, stiffness, rest_length}; 
    return spring;
}

void DrawSpring(Spring *spring)
{
    DrawLineEx(spring->particles[0]->position, spring->particles[1]->position, 3, WHITE);
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

void DrawAllSprings(Spring* springs, int springs_length)
{
    for(int i = 0; i < springs_length; i++)
    {
        DrawSpring(&springs[i]);
    }
}

Particle CreateParticle(Vector2 position, Vector2 velocity, Vector2 acceleration, Vector2 force, float mass)
{
    Particle particle = {position, velocity, acceleration, force, mass};
    return particle;
}

void DrawParticle(Particle *particle)
{
    DrawCircleV(particle->position, NODE_RADIUS, ORANGE);
}

void UpdateParticlePosition(Particle *particle, double dt)
{
    Vector2 position_displacement = Vector2Scale(particle->velocity, dt);
    particle->position = Vector2Add(particle->position, position_displacement); 
}

void AddParticleFriction(Particle *particle)
{
    particle->velocity = Vector2Scale(particle->velocity, 1 - FRICTION);
}

void UpdateParticleVelocity(Particle *particle, double dt)
{
    Vector2 velocity_displacement = Vector2Scale(particle->acceleration, dt);
    particle->velocity = Vector2Add(particle->velocity, velocity_displacement);
    AddParticleFriction(particle);
}

void UpdateParticleAcceleration(Particle *particle, double dt)
{
    particle->acceleration = Vector2Scale(particle->force, 1/(particle->mass));
}

void ClampParticleVelocity(Particle *particle)
{
    particle->velocity = Vector2ClampValue(particle->velocity, -MAX_VELOCITY_VALUE, MAX_VELOCITY_VALUE);
}

void ResetParticleForces(Particle *particle)
{
    particle->force = Vector2Zero();
}

void DrawAllParticles(Particle* particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        DrawParticle(&particles[i]);
    }
}

void UpdateAllParticlePositions(Particle* particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticlePosition(&particles[i], dt);
    }
}
void UpdateAllParticleVelocities(Particle* particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticleVelocity(&particles[i], dt);
    }
}
void UpdateAllParticleAccelerations(Particle* particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticleAcceleration(&particles[i], dt);
    }
}

void ClampAllParticleVelocities(Particle* particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        ClampParticleVelocity(&particles[i]);
    }
}

void ResetAllParticleForces(Particle* particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        ResetParticleForces(&particles[i]);
    }
}

Particle* FindNearestParticleV(Vector2 position, Particle* particles, int particles_length)
{
    float closest_distance = FLT_MAX;
    Particle* nearest_particle;
    for(int i = 0; i < particles_length; i++)
    {
        float curr_distance = Vector2Distance(particles[i].position, position);
        if(curr_distance < closest_distance)
        {
            closest_distance = curr_distance;
            nearest_particle = &particles[i];
        }
    }
    return nearest_particle;
}

void DragParticleByMouse(Particle *particle, Vector2 mouse_position, bool *is_dragging)
{
    *is_dragging = true;
    particle->acceleration = Vector2Zero();
    particle->velocity = Vector2Zero();
    particle->position = mouse_position;
}
