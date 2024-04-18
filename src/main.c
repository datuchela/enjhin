#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

const float NODE_RADIUS = 5.0;
const float DEFAULT_SPRING_STIFFNESS = 50.0;

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

Spring SpringCreate(Particle *particle1, Particle *particle2, float stiffness)
{
    float rest_length = Vector2Distance(particle1->position, particle2->position);
    Spring spring = {{particle1, particle2}, stiffness, rest_length}; 
    return spring;
}

void SpringUpdate(Spring *spring)
{
    Vector2 spring_vector = Vector2Subtract(spring->particles[0]->position, spring->particles[1]->position);
    float spring_length = Vector2Length(spring_vector);
    float delta_length = spring_length - spring->rest_length;
    Vector2 tension_direction = Vector2Normalize(spring_vector);

    Vector2 tension0 = Vector2Scale(tension_direction, -delta_length);
    Vector2 tension1 = Vector2Scale(tension_direction, delta_length);
    
    spring->particles[0]->force = Vector2Add(spring->particles[0]->force, Vector2Scale(tension0, spring->stiffness));
    spring->particles[1]->force = Vector2Add(spring->particles[1]->force, Vector2Scale(tension1, spring->stiffness));
}

Particle ParticleCreate(Vector2 position, Vector2 velocity, Vector2 acceleration, Vector2 force, float mass)
{
    Particle particle = {position, velocity, acceleration, force, mass};
    return particle;
}

void ParticleDraw(Particle *particle)
{
    DrawCircleV(particle->position, NODE_RADIUS, YELLOW);
}

void ParticlePositionUpdate(Particle *particle, double dt)
{
    Vector2 position_displacement = Vector2Scale(particle->velocity, dt);
    particle->position = Vector2Add(particle->position, position_displacement); 
}

void ParticleVelocityUpdate(Particle *particle, double dt)
{
    Vector2 velocity_displacement = Vector2Scale(particle->acceleration, dt);
    particle->velocity = Vector2Add(particle->velocity, velocity_displacement);
}

void ParticleAccelerationUpdate(Particle *particle, double dt)
{
   particle->acceleration = Vector2Scale(particle->force, 1/(particle->mass));
}

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int _argc, char *_argv[]) 
{
    Particle particle1 = ParticleCreate
        (
            (Vector2) {69.0, 220.0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            (Vector2) {900, 0},
            9.0
        );

    Particle particle2 = ParticleCreate
        (
            (Vector2) {199.0, 270.0},
            (Vector2) {0, 0},
            (Vector2) {0, 0},
            (Vector2) {-900, 0},
            9.0
        );

    Spring spring = SpringCreate(&particle1, &particle2, DEFAULT_SPRING_STIFFNESS);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(60);
    SetWindowMonitor(0);

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    double time_now = 0;
    double time_prev;

    while (!WindowShouldClose())
    {
        time_prev = time_now;
        time_now = GetTime();
        double dt = time_now - time_prev;

        BeginDrawing();
        ClearBackground(BLACK);

        // debug
        DrawFPS(10, 10);
        DrawText(TextFormat("x: %f y: %f", particle1.position.x, particle1.position.y), 500, 36, 18, WHITE);
        DrawText(TextFormat("vx: %f vy: %f", particle1.velocity.x, particle1.velocity.y), 500, 66, 18, WHITE);
        DrawText(TextFormat("ax: %f ay: %f", particle1.acceleration.x, particle1.acceleration.y), 500, 96, 18, WHITE);

        DrawText(TextFormat("x: %f y: %f", particle2.position.x, particle2.position.y), 500, 136, 18, WHITE);
        DrawText(TextFormat("vx: %f vy: %f", particle2.velocity.x, particle2.velocity.y), 500, 166, 18, WHITE);
        DrawText(TextFormat("ax: %f ay: %f", particle2.acceleration.x, particle2.acceleration.y), 500, 196, 18, WHITE);

        // logic
        SpringUpdate(&spring);
        ParticleAccelerationUpdate(&particle1, dt);
        ParticleAccelerationUpdate(&particle2, dt);
        ParticleVelocityUpdate(&particle1, dt);
        ParticleVelocityUpdate(&particle2, dt);
        ParticlePositionUpdate(&particle1, dt);
        ParticlePositionUpdate(&particle2, dt);

        DrawLineEx(spring.particles[0]->position, spring.particles[1]->position, 3, WHITE);
        ParticleDraw(&particle1);
        ParticleDraw(&particle2);

        particle1.force = Vector2Zero();
        particle2.force = Vector2Zero();

        EndDrawing();
    }
    return 0;
}
