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
Font font;

typedef struct {
    char *label;
    float value;
    char *value_format;
} DebugInfo;

DebugInfo* constants[] = {
    &((DebugInfo) { "FRICTION", FRICTION, "%.3f" }),
    &((DebugInfo) { "SPRING_STIFFNESS", DEFAULT_SPRING_STIFFNESS, "%.1f" }),
    &((DebugInfo) { "SPRING_DAMPENING", DEFAULT_SPRING_DAMPENING, "%.1f" }),
    &((DebugInfo) { "MAX_VELOCITY", MAX_VELOCITY_VALUE, "%.1f" }),
};

int constants_length = sizeof(constants) / sizeof(constants[0]);

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

typedef struct SoftBody{
    Spring* springs;
    Particle* particles;
    int springs_length;
    int particles_length;
} SoftBody;

void DEBUG_Draw_Stat(char *label, float value, const char *value_format, Vector2 position);
void DEBUG_Draw_Stats(DebugInfo* debug_infos[], int debug_infos_length, Vector2 starting_position);
void DEBUG_Draw_Particle_Stats(SoftBody *soft_body);

Spring CreateSpring(Particle *particle1, Particle *particle2, float stiffness);
void UpdateSpring(Spring *spring);
void DrawSpring(Spring *spring);
void DampenSpring(Spring *spring, Vector2 tension_direction);
void UpdateAllSprings(Spring* springs, int springs_length);
void DrawAllSprings(Spring* springs, int springs_length);

Particle CreateParticle(Vector2 position, Vector2 velocity, Vector2 acceleration, Vector2 force, float mass);
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

Particle* FindNearestParticleV(Vector2 position, SoftBody *soft_body);
void DragParticleByMouse(Particle *particle, Vector2 mouse_position, bool *is_dragging);

SoftBody CreateSoftBody(Particle* particles, int particles_length, Spring* springs, int springs_length);
void UpdateSoftBody(SoftBody *soft_body, double dt);
void DrawSoftBody(SoftBody *soft_body);

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

    int particles_length = sizeof(particles) / sizeof(particles[0]);
    int springs_length = sizeof(springs) / sizeof(springs[0]);

    SoftBody soft_body1 = CreateSoftBody(particles, particles_length, springs, springs_length);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(60);
    SetWindowMonitor(0);

    font = LoadFont("assets/fonts/mecha.png");

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    double time_now = 0;
    double time_prev;
    float simulation_speed = 1.0;

    Particle* nearest_particle;
    bool is_dragging = false;

    while (!WindowShouldClose())
    {
        time_prev = time_now;
        time_now = GetTime();
        double dt = (time_now - time_prev) * simulation_speed;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawFPS(10, 10);

        if(IsMouseButtonDown(0))
        {
            Vector2 mouse_position = GetMousePosition();
            if(is_dragging == false)
            {
                nearest_particle = FindNearestParticleV(mouse_position, &soft_body1);
            }
            DragParticleByMouse(nearest_particle, mouse_position, &is_dragging);
        }

        if(IsMouseButtonReleased(0))
        {
            is_dragging = false;
        }

        UpdateSoftBody(&soft_body1, dt);
        DrawSoftBody(&soft_body1);

        DEBUG_Draw_Stats(constants, constants_length, (Vector2){10, 30});
        DEBUG_Draw_Particle_Stats(&soft_body1);

        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();

    return 0;
}

void DEBUG_Draw_Stat(char *label, float value, const char *value_format, Vector2 position)
{
    const char *formatted_label = TextFormat("%s: ", label);
    const char *formatted_value = TextFormat(value_format, value);
    DrawTextEx(font, TextFormat("%s%s", formatted_label, formatted_value), position, 16, 8, WHITE);
}

void DEBUG_Draw_Stats(DebugInfo* debug_infos[], int debug_infos_length, Vector2 starting_position)
{
    int line_height = 16;
    for(int i = 0; i < debug_infos_length; i++)
    {
        Vector2 position = Vector2Add(starting_position, (Vector2){0, line_height * i});
        DEBUG_Draw_Stat(debug_infos[i]->label, debug_infos[i]->value, debug_infos[i]->value_format, position);
    }
}

void DEBUG_Draw_Particle_Stats(SoftBody *soft_body)
{
    int gap_x = 128;
    int gap_y = 16;
    int particle_gap_y = 64;
    Vector2 position = { WINDOW_WIDTH - 250, 10 };

    for(int i = 0; i < soft_body->particles_length; i++)
    {
        DEBUG_Draw_Stat("x", soft_body->particles[i].position.x, "%.1f", Vector2Add(position, (Vector2){0, 0}));
        DEBUG_Draw_Stat("y", soft_body->particles[i].position.y, "%.1f", Vector2Add(position, (Vector2){gap_x, 0}));
        DEBUG_Draw_Stat("vx", soft_body->particles[i].velocity.x, "%.1f", Vector2Add(position, (Vector2){0, gap_y * 1}));
        DEBUG_Draw_Stat("vy", soft_body->particles[i].velocity.y, "%.1f", Vector2Add(position, (Vector2){gap_x, gap_y * 1}));
        DEBUG_Draw_Stat("ax", soft_body->particles[i].velocity.x, "%.1f", Vector2Add(position, (Vector2){0, gap_y * 2}));
        DEBUG_Draw_Stat("ay", soft_body->particles[i].velocity.y, "%.1f", Vector2Add(position, (Vector2){gap_x, gap_y * 2}));
        position = Vector2Add(position, (Vector2){0, particle_gap_y});
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
    DrawLineEx(spring->particles[0]->position, spring->particles[1]->position, 3, RAYWHITE);
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

void UpdateParticle(Particle *particle, double dt)
{
    UpdateParticleAcceleration(particle, dt);
    UpdateParticleVelocity(particle, dt);
    UpdateParticlePosition(particle, dt);
}

void DrawParticle(Particle *particle)
{
    DrawCircleV(particle->position, NODE_RADIUS, MAGENTA);
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

void DrawAllParticles(Particle* particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        DrawParticle(&particles[i]);
    }
}

void UpdateAllParticles(Particle* particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticle(&particles[i], dt);
    }
}

void ResetAllParticleForces(Particle* particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        ResetParticleForces(&particles[i]);
    }
}

void UpdateAllParticleAccelerations(Particle* particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticleAcceleration(&particles[i], dt);
    }
}

void UpdateAllParticleVelocities(Particle* particles, int particles_length, double dt)
{
    for(int i = 0; i < particles_length; i++)
    {
        UpdateParticleVelocity(&particles[i], dt);
    }
}

void ClampAllParticleVelocities(Particle* particles, int particles_length)
{
    for(int i = 0; i < particles_length; i++)
    {
        ClampParticleVelocity(&particles[i]);
    }
}

void UpdateAllParticlePositions(Particle* particles, int particles_length, double dt)
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

void DragParticleByMouse(Particle *particle, Vector2 mouse_position, bool *is_dragging)
{
    *is_dragging = true;
    particle->acceleration = Vector2Zero();
    particle->velocity = Vector2Zero();
    particle->position = mouse_position;
}

SoftBody CreateSoftBody(Particle* particles, int particles_length, Spring* springs, int springs_length)
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
