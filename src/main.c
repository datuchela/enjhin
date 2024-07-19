#include "debug.h"
#include "fizziks.h"
#include "raygui.h"
#include "raylib.h"

float epsilon = EPSILON;
Color NODE_COLOR = YELLOW;
float SIMULATION_SPEED = 2.0;
float MAX_VELOCITY_VALUE = 5000;
float NODE_RADIUS = 5.0;
float SPRING_THICKNESS = 2.5;
float DEFAULT_SPRING_STIFFNESS = 400.0;
float DEFAULT_SPRING_DAMPENING = 50.0;
float FRICTION = 0.0125;

const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 800;
Font font;

int main(int _argc, char *_argv[])
{
    SoftBody soft_bodies[2] = {};

    Particle particles1[4];
    Particle particles2[4];
    Spring springs1[6];
    Spring springs2[6];

    soft_bodies[0] = CreateSquare(particles1, springs1, 350, 400, 50, 10,
                                  DEFAULT_SPRING_STIFFNESS, 2, false);
    soft_bodies[1] = CreateSquare(particles2, springs2, 350, 350, 100, 10,
                                  DEFAULT_SPRING_STIFFNESS, 2, true);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(120);
    SetWindowMonitor(0);
    font = LoadFont("assets/fonts/mecha.png");

    float dt;
    MouseState mouse_state;
    bool isPaused = false;
    GuiLock();

    DebugInfo *constants[] = {
        &((DebugInfo){ "FRICTION", &FRICTION, 0.0, 1.0, "%f", TYPE_EDITABLE,
                       false }),
        &((DebugInfo){ "SPRING STIFFNESS", &DEFAULT_SPRING_STIFFNESS, 0.0,
                       2000, "%.1f", TYPE_EDITABLE, false }),
        &((DebugInfo){ "SPRING DAMPENING", &DEFAULT_SPRING_DAMPENING, 0.0,
                       300.0, "%.1f", TYPE_EDITABLE, false }),
        &((DebugInfo){ "MAX VELOCITY", &MAX_VELOCITY_VALUE, 0.0, 10000, "%.1f",
                       TYPE_EDITABLE, false }),
        &((DebugInfo){ "SIMULATION SPEED", &SIMULATION_SPEED, 0.0, 10.0,
                       "%.1f", TYPE_EDITABLE, false }),
        &((DebugInfo){ "EPSILON", &epsilon, 0.0, 10.0, "%f", TYPE_CONSTANT,
                       false }),
        &((DebugInfo){ "dt", &dt, 0.0, 10.0, "%f", TYPE_CONSTANT, false }),
    };
    int constants_length = sizeof(constants) / sizeof(constants[0]);

    while (!WindowShouldClose())
        {
            if (IsKeyPressed(KEY_P))
                {
                    isPaused = !isPaused;
                    isPaused ? GuiUnlock() : GuiLock();
                }

            dt = GetFrameTime() * SIMULATION_SPEED;

            if (!isPaused)
                {
                    AttachMouseControls(soft_bodies, 2, &mouse_state);

                    ResetSoftBodyCollisions(&soft_bodies[0]);
                    ResetSoftBodyCollisions(&soft_bodies[1]);
                    DetectCollisionSoftBodies(&soft_bodies[0],
                                              &soft_bodies[1]);
                    DetectCollisionSoftBodies(&soft_bodies[1],
                                              &soft_bodies[0]);

                    UpdateSoftBody(&soft_bodies[0], dt);
                    UpdateSoftBody(&soft_bodies[1], dt);
                }

            BeginDrawing();
            ClearBackground(BLACK);

            DrawFPS(10, 10);
            DrawSoftBody(&soft_bodies[0]);
            DrawSoftBody(&soft_bodies[1]);

            GuiLabel((Rectangle){ 16, GetScreenHeight() - 32, 200, 24 },
                     TextFormat("%s", isPaused ? "PAUSED (press 'p' to resume)"
                                               : "press 'p' to PAUSE"));
            DEBUG_Draw_Stats(constants, constants_length, (Vector2){ 10, 40 },
                             isPaused);

            DEBUG_Draw_Particle_Stats(&soft_bodies[0],
                                      (Vector2){ GetScreenWidth() - 400, 10 });
            DEBUG_Draw_Particle_Stats(
                &soft_bodies[1], (Vector2){ GetScreenWidth() - 400, 280 });

            EndDrawing();
        }

    UnloadFont(font);
    CloseWindow();

    return 0;
}
