#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

const float NODE_RADIUS = 5.0;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float mass;
} Particle;

Particle ParticleCreate(Vector2 position, Vector2 velocity, Vector2 acceleration, float mass)
{
    Particle particle = {position, velocity, acceleration, mass};
    return particle;
}

void ParticleDraw(Particle *particle)
{
    DrawCircleV(particle->position, NODE_RADIUS, WHITE);
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

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int _argc, char *_argv[]) 
{
    Particle particle1 = ParticleCreate
        (
            (Vector2) {69.0, 220.0},
            (Vector2) {69, 42},
            (Vector2) {0.0, 191.0},
            69.0
        );

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
        ParticleVelocityUpdate(&particle1, dt);
        ParticlePositionUpdate(&particle1, dt);
        ParticleDraw(&particle1);
        EndDrawing();
    }
    return 0;
}
