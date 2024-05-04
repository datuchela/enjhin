#include "fizziks.h"

const Color NODE_COLOR = YELLOW;
const float SIMULATION_SPEED = 2.0;
const float MAX_VELOCITY_VALUE = 5000;
const float NODE_RADIUS = 5.0;
const float SPRING_THICKNESS = 3.0;
const float DEFAULT_SPRING_STIFFNESS = 400.0;
const float DEFAULT_SPRING_DAMPENING = 50.0;
const float FRICTION = 0.0125;

const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 800;

typedef struct {
    char *label;
    float value;
    char *value_format;
} DebugInfo;

DebugInfo* constants[] = {
    &((DebugInfo) { "FRICTION", FRICTION, "%.3f" }),
    &((DebugInfo) { "SPRING STIFFNESS", DEFAULT_SPRING_STIFFNESS, "%.1f" }),
    &((DebugInfo) { "SPRING DAMPENING", DEFAULT_SPRING_DAMPENING, "%.1f" }),
    &((DebugInfo) { "MAX VELOCITY", MAX_VELOCITY_VALUE, "%.1f" }),
    &((DebugInfo) { "SIMULATION SPEED", SIMULATION_SPEED, "%.1f" }),
};

int constants_length = sizeof(constants) / sizeof(constants[0]);

void DEBUG_Draw_Stat(char *label, float value, const char *value_format, Vector2 position);
void DEBUG_Draw_Stats(DebugInfo* debug_infos[], int debug_infos_length, Vector2 starting_position);
void DEBUG_Draw_Particle_Stats(SoftBody *soft_body, Vector2 position);

Font font;

int main(int _argc, char *_argv[]) 
{
    SoftBody soft_bodies[2] = {};

    Particle particles1[4] = {
        CreateParticle((Vector2) {250.0, 250.0}, 10.0),
        CreateParticle((Vector2) {300.0, 250.0}, 10.0),
        CreateParticle((Vector2) {300.0, 300.0}, 10.0),
        CreateParticle((Vector2) {250.0, 300.0}, 10.0),
    };
    Particle particles2[4] = {
        CreateParticle((Vector2) {350.0, 350.0}, 10.0),
        CreateParticle((Vector2) {450.0, 350.0}, 10.0),
        CreateParticle((Vector2) {450.0, 450.0}, 10.0),
        CreateParticle((Vector2) {350.0, 450.0}, 10.0),
    };
    Spring springs1[6] = {
        CreateSpring(&particles1[0], &particles1[1], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles1[0], &particles1[2], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles1[0], &particles1[3], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles1[1], &particles1[2], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles1[1], &particles1[3], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles1[2], &particles1[3], DEFAULT_SPRING_STIFFNESS),
    };
    Spring springs2[6] = {
        CreateSpring(&particles2[0], &particles2[1], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles2[0], &particles2[2], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles2[0], &particles2[3], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles2[1], &particles2[2], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles2[1], &particles2[3], DEFAULT_SPRING_STIFFNESS),
        CreateSpring(&particles2[2], &particles2[3], DEFAULT_SPRING_STIFFNESS),
    };

    int particles1_length = sizeof(particles1) / sizeof(particles1[0]);
    int springs1_length = sizeof(springs1) / sizeof(springs1[0]);

    int particles2_length = sizeof(particles2) / sizeof(particles2[0]);
    int springs2_length = sizeof(springs2) / sizeof(springs2[0]);

    soft_bodies[0] = CreateSoftBody(particles1, particles1_length, springs1, springs1_length);
    soft_bodies[1] = CreateSoftBody(particles2, particles2_length, springs2, springs2_length);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(120);
    SetWindowMonitor(0);

    font = LoadFont("assets/fonts/mecha.png");

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    double time_now = 0;
    double time_prev;

    bool is_dragging = false;

    while (!WindowShouldClose())
    {
        time_prev = time_now;
        time_now = GetTime();
        double dt = (time_now - time_prev) * SIMULATION_SPEED;

        AttachMouseControls(&soft_bodies[0], is_dragging);

        ResetSoftBodyCollisions(&soft_bodies[0]);
        ResetSoftBodyCollisions(&soft_bodies[1]);
        HandleCollisionSoftBodies(&soft_bodies[0], &soft_bodies[1]);
        HandleCollisionSoftBodies(&soft_bodies[1], &soft_bodies[0]);

        UpdateSoftBody(&soft_bodies[0], dt);
        UpdateSoftBody(&soft_bodies[1], dt);

        BeginDrawing();
        ClearBackground(BLACK);

        DrawFPS(10, 10);
        DrawSoftBody(&soft_bodies[0]);
        DrawSoftBody(&soft_bodies[1]);

        DEBUG_Draw_Stats(constants, constants_length, (Vector2){10, 30});
        DEBUG_Draw_Particle_Stats(&soft_bodies[0], (Vector2){WINDOW_WIDTH - 250, 10});
        DEBUG_Draw_Particle_Stats(&soft_bodies[1], (Vector2){WINDOW_WIDTH - 250, 280});

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

void DEBUG_Draw_Stats(DebugInfo *debug_infos[], int debug_infos_length, Vector2 starting_position)
{
    int line_height = 16;
    for(int i = 0; i < debug_infos_length; i++)
    {
        Vector2 position = Vector2Add(starting_position, (Vector2){0, line_height * i});
        DEBUG_Draw_Stat(debug_infos[i]->label, debug_infos[i]->value, debug_infos[i]->value_format, position);
    }
}

void DEBUG_Draw_Particle_Stats(SoftBody *soft_body, Vector2 position)
{
    int gap_x = 128;
    int gap_y = 16;
    int particle_gap_y = 64;

    for(int i = 0; i < soft_body->particles_length; i++)
    {
        DEBUG_Draw_Stat("x", soft_body->particles[i].position.x, "%.1f", Vector2Add(position, (Vector2){0, 0}));
        DEBUG_Draw_Stat("y", soft_body->particles[i].position.y, "%.1f", Vector2Add(position, (Vector2){gap_x, 0}));
        DEBUG_Draw_Stat("vx", soft_body->particles[i].velocity.x, "%.1f", Vector2Add(position, (Vector2){0, gap_y * 1}));
        DEBUG_Draw_Stat("vy", soft_body->particles[i].velocity.y, "%.1f", Vector2Add(position, (Vector2){gap_x, gap_y * 1}));
        DEBUG_Draw_Stat("ax", soft_body->particles[i].velocity.x, "%.1f", Vector2Add(position, (Vector2){0, gap_y * 2}));
        DEBUG_Draw_Stat("ay", soft_body->particles[i].velocity.y, "%.1f", Vector2Add(position, (Vector2){gap_x, gap_y * 2}));
        DEBUG_Draw_Stat("is_colliding", soft_body->particles[i].is_colliding, "%.1f", Vector2Add(position, (Vector2){0, gap_y * 3}));
        position = Vector2Add(position, (Vector2){0, particle_gap_y});
    }
}
