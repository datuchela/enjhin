#include "debug.h"

void DEBUG_Draw_Stat(char *label, float value, const char *value_format, Vector2 position)
{
    const char *formatted_label = TextFormat("%s: ", label);
    const char *formatted_value = TextFormat(value_format, value);
    DrawTextEx(font, TextFormat("%s%s", formatted_label, formatted_value), position, 16, 8, WHITE);
}

void DEBUG_Draw_Stats(DebugInfo *debug_infos[], int debug_infos_length, Vector2 starting_position)
{
    float line_height = 16;
    for(int i = 0; i < debug_infos_length; i++)
    {
        Vector2 position = Vector2Add(starting_position, (Vector2){0, line_height * i});
        DEBUG_Draw_Stat(debug_infos[i]->label, debug_infos[i]->value, debug_infos[i]->value_format, position);
    }
}

void DEBUG_Draw_Particle_Stats(SoftBody *soft_body, Vector2 position)
{
    float gap_x = 128;
    float gap_y = 16;
    float particle_gap_y = 64;

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