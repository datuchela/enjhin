#include <stdio.h>
#include <stdbool.h>
#include "raylib.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int _argc, char *_argv[]) 
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enjhin");
    SetTargetFPS(60);
    SetWindowMonitor(0);

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    Vector2 prev_mouse_pos;
    Vector2 mouse_pos;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if(IsMouseButtonPressed(0))
        {
            mouse_pos = GetMousePosition();
            prev_mouse_pos = mouse_pos;
        }

        if(IsMouseButtonDown(0))
        {
            mouse_pos = GetMousePosition();
            DrawCircleV(mouse_pos, 5, RED);
            DrawLineEx(prev_mouse_pos, mouse_pos, 10.0f, RED);
            //            DrawLineBezier(prev_mouse_pos, mouse_pos, 10.0f, RED);
            prev_mouse_pos = mouse_pos;
        }

        if(IsMouseButtonDown(1))
        {
            ClearBackground(BLACK);
        }

        EndDrawing();
    }
    return 0;
}
