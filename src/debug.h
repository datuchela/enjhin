#ifndef DEBUG_H
#define DEBUG_H

#include "fizziks.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern Font font;

typedef struct {
    char *label;
    float value;
    char *value_format;
} DebugInfo;

void DEBUG_Draw_Stat(char *label, float value, const char *value_format, Vector2 position);
void DEBUG_Draw_Stats(DebugInfo* debug_infos[], int debug_infos_length, Vector2 starting_position);
void DEBUG_Draw_Particle_Stats(SoftBody *soft_body, Vector2 position);

#endif /* DEBUG_H */
