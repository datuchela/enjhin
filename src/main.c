#include<stdio.h>
#include "raylib.h"

int main() {
  InitWindow(800, 600, "My Game");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawCircle(400, 300, 40.0f, RED);
    EndDrawing();
  }
  return 0;
}
