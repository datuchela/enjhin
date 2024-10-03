#ifndef DEBUG_H
#define DEBUG_H

#include "fizziks.h"

#ifndef DEBUG_VARS
#define FONT_SIZE 16.0
#define SPACING 8.0
#define COLOR WHITE
#endif

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern Font font;

typedef enum
{
    TYPE_CONSTANT = 0,
    TYPE_EDITABLE = 1,
} DebugInfoType;

typedef struct
{
    char *label;
    float *value;
    float min_value;
    float max_value;
    char *value_format;
    DebugInfoType type;
    bool editMode;
} DebugInfo;

void DEBUG_Draw_Stat(char *label, float value, const char *value_format,
                     Vector2 position);
void DEBUG_Draw_Stats(DebugInfo *debug_infos[], int debug_infos_length,
                      Vector2 starting_position);
void DEBUG_Draw_Particle_Stats(SoftBody *soft_body, Vector2 position);

Particle DEBUG_CreateParticle(Vector2 position, float mass, int id);
void DEBUG_Draw_Particle_Ids(SoftBody *soft_body);

#endif /* DEBUG_H */
