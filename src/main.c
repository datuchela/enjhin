#include "debug.h"
#include "fizziks.h"
#include "raygui.h"
#include "raylib.h"
#include <raymath.h>
#include <stdio.h>
#include <string.h>

const float epsilon = EPSILON;
const Color NODE_COLOR = YELLOW;
const float SIMULATION_SPEED = 2.0;
const float MAX_VELOCITY_VALUE = 5000;
const float NODE_RADIUS = 5.0;
const float SPRING_THICKNESS = 2.5;
const float DEFAULT_SPRING_STIFFNESS = 700.0;
const float DEFAULT_SPRING_DAMPENING = 50.0;
const float FRICTION = 0.0075;
const Vector2 DEFAULT_GRAVITY = { 0, 100.0 };

VariableConstants variable_constants = {
    .epsilon = epsilon,
    .node_color = NODE_COLOR,
    .simulation_speed = SIMULATION_SPEED,
    .max_velocity_value = MAX_VELOCITY_VALUE,
    .node_radius = NODE_RADIUS,
    .spring_thickness = SPRING_THICKNESS,
    .spring_stiffness = DEFAULT_SPRING_STIFFNESS,
    .spring_dampening = DEFAULT_SPRING_DAMPENING,
    .friction = FRICTION,
    .gravity = DEFAULT_GRAVITY,
};

DebugInfo *constants[] = {
    &((DebugInfo){ "FRICTION", &variable_constants.friction, 0.0, 1.0, "%f",
                   TYPE_EDITABLE, false }),
    &((DebugInfo){ "SPRING STIFFNESS", &variable_constants.spring_stiffness,
                   0.0, 9000, "%.1f", TYPE_EDITABLE, false }),
    &((DebugInfo){ "SPRING DAMPENING", &variable_constants.spring_dampening,
                   0.0, 900.0, "%.1f", TYPE_EDITABLE, false }),
    &((DebugInfo){ "MAX VELOCITY", &variable_constants.max_velocity_value, 0.0,
                   10000, "%.1f", TYPE_EDITABLE, false }),
    &((DebugInfo){ "GRAVITY (HOR)", &variable_constants.gravity.x, -5000, 5000,
                   "%.1f", TYPE_EDITABLE, false }),
    &((DebugInfo){ "GRAVITY (VER)", &variable_constants.gravity.y, -5000, 5000,
                   "%.1f", TYPE_EDITABLE, false }),
    &((DebugInfo){ "SIMULATION SPEED", &variable_constants.simulation_speed,
                   0.0, 10.0, "%.1f", TYPE_EDITABLE, false }),
    &((DebugInfo){ "EPSILON", &variable_constants.epsilon, 0.0, 10.0, "%f",
                   TYPE_CONSTANT, false }),
};
size_t constants_length = LENGTH(constants);

const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 800;
Font font;

int main(int argc, char *argv[])
{
    const char *file_path;
    if (argc < 2)
        {
            file_path = "assets/objects/star.json";
        }
    else
        {
            file_path = argv[1];
        }

    size_t particles_length;
    size_t springs_length;

    JsonSoftBody sb_json = {
        .particles_length = &particles_length,
        .springs_length = &springs_length,
    };

    if (JsonParseSoftBody(file_path, &sb_json) == 1)
        return 1;

    Particle particles[particles_length];
    Spring springs[springs_length];

    SoftBody sb = {
        .particles = particles,
        .springs = springs,
        .particles_length = particles_length,
        .springs_length = springs_length,
    };

    JsonInitSoftBody(file_path, &sb, &sb_json);

    size_t soft_bodies_length = 3;
    SoftBody soft_bodies[soft_bodies_length] = {};

    Particle particles1[4];
    Particle particles2[4];
    Spring springs1[6];
    Spring springs2[6];

    soft_bodies[1] = sb;
    InitSquare(&soft_bodies[2], particles1, springs1, 350, 400, 50, 10,
               DEFAULT_SPRING_STIFFNESS, 2, false);
    InitSquare(&soft_bodies[0], particles2, springs2, 350, 350, 100, 10,
               DEFAULT_SPRING_STIFFNESS, 2, true);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(165);
    SetWindowMonitor(0);
    SetExitKey(KEY_Q);

    font = LoadFont("assets/fonts/mecha.png");

    float dt;

    Rectangle world_boundary = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - 4 };
    MouseState mouse_state = { 0 };

    // Deadzone for GUI controls, hardcoded for now, preferably should be
    // calculated (once) when GUI is drawn.
    mouse_state.deadzone = (Rectangle){ 0, 0, 330, 230 };

    bool is_paused = false;

    while (!WindowShouldClose())
        {
            if (IsKeyPressed(KEY_P))
                {
                    is_paused = !is_paused;
                }
            if (IsKeyPressed(KEY_R))
                {
                    variable_constants.epsilon = epsilon;
                    variable_constants.node_color = NODE_COLOR;
                    variable_constants.simulation_speed = SIMULATION_SPEED;
                    variable_constants.max_velocity_value = MAX_VELOCITY_VALUE;
                    variable_constants.node_radius = NODE_RADIUS;
                    variable_constants.spring_thickness = SPRING_THICKNESS;
                    variable_constants.spring_stiffness
                        = DEFAULT_SPRING_STIFFNESS;
                    variable_constants.spring_dampening
                        = DEFAULT_SPRING_DAMPENING;
                    variable_constants.friction = FRICTION;
                    variable_constants.gravity = DEFAULT_GRAVITY;
                }

            dt = GetFrameTime() * variable_constants.simulation_speed;

            if (!is_paused)
                {
                    AttachMouseControls(soft_bodies, soft_bodies_length,
                                        &mouse_state);

                    ResetCollisions(soft_bodies, soft_bodies_length);
                    Collisions(soft_bodies, soft_bodies_length);

                    UpdateSoftBodies(soft_bodies, soft_bodies_length, dt);
                    BoundaryCollisionBoxAll(soft_bodies, soft_bodies_length,
                                            world_boundary);
                }

            BeginDrawing();
            ClearBackground(BLACK);

            DrawSoftBodies(soft_bodies, soft_bodies_length);

            if (is_paused)
                DrawTextEx(font, "PAUSED",
                           (Vector2){ (float)GetScreenWidth() / 2 - 96,
                                      (float)GetScreenHeight() / 2 - 48 },
                           48, SPACING, COLOR);

            GuiLabel((Rectangle){ 16, GetScreenHeight() - 80, 200, 24 },
                     "[P] to PAUSE/UNPAUSE");
            GuiLabel((Rectangle){ 16, GetScreenHeight() - 56, 200, 24 },
                     "[R] to RESET");
            GuiLabel((Rectangle){ 16, GetScreenHeight() - 32, 200, 24 },
                     "[Q] to QUIT");

            DEBUG_Draw_Stats(constants, constants_length, (Vector2){ 10, 40 });
            DrawFPS(10, 10);

            EndDrawing();
        }

    UnloadFont(font);
    CloseWindow();

    return 0;
}
