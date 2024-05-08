#include "fizziks.h"
#include "debug.h"

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
Font font;

DebugInfo* constants[] = {
    &((DebugInfo) { "FRICTION", FRICTION, "%.3f" }),
    &((DebugInfo) { "SPRING STIFFNESS", DEFAULT_SPRING_STIFFNESS, "%.1f" }),
    &((DebugInfo) { "SPRING DAMPENING", DEFAULT_SPRING_DAMPENING, "%.1f" }),
    &((DebugInfo) { "MAX VELOCITY", MAX_VELOCITY_VALUE, "%.1f" }),
    &((DebugInfo) { "SIMULATION SPEED", SIMULATION_SPEED, "%.1f" }),
};

int constants_length = sizeof(constants) / sizeof(constants[0]);

int main(int _argc, char *_argv[]) 
{
    SoftBody soft_bodies[2] = {};

    Particle particles1[4];
    Particle particles2[4];
    Spring springs1[6];
    Spring springs2[6];

    soft_bodies[0] = CreateSquare(particles1, springs1, 250, 250, 50, 10, DEFAULT_SPRING_STIFFNESS, 2, false);
    soft_bodies[1] = CreateSquare(particles2, springs2, 350, 350, 100, 10, DEFAULT_SPRING_STIFFNESS, 2, true);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(120);
    SetWindowMonitor(0);

    font = LoadFont("assets/fonts/mecha.png");

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    double time_now = 0;
    double time_prev;

    MouseState mouse_state;

    while (!WindowShouldClose())
    {
        time_prev = time_now;
        time_now = GetTime();
        double dt = (time_now - time_prev) * SIMULATION_SPEED;

        AttachMouseControls(soft_bodies, 2, &mouse_state);

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

        DebugInfo* variables[] = {
            &((DebugInfo) { "dt", dt, "%.3f" }),
            &((DebugInfo) { "is_dragging", mouse_state.is_dragging, "%.1f" }),
        };
        int variables_length = sizeof(variables) / sizeof(variables[0]);

        DEBUG_Draw_Stats(constants, constants_length, (Vector2){10, 30});
        DEBUG_Draw_Stats(variables, variables_length, (Vector2){10, 132});
        DEBUG_Draw_Particle_Stats(&soft_bodies[0], (Vector2){GetScreenWidth() - 400, 10});
        DEBUG_Draw_Particle_Stats(&soft_bodies[1], (Vector2){GetScreenWidth() - 400, 280});

        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();

    return 0;
}
