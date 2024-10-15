#include "fizziks.h"
#include "raylib.h"
#include <assert.h>
#include <jansson.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>

bool LessOrEquals(float a, float b)
{
    return a < b || FloatEquals(a, b);
}

bool GreaterOrEquals(float a, float b)
{
    return a > b || FloatEquals(a, b);
}

float Vector2CrossProduct(Vector2 vector1, Vector2 vector2)
{
    return vector1.x * vector2.y - vector1.y * vector2.x;
}

bool IsPointOnSegment(Vector2 point, Segment segment)
{
    // bool is_on_segment_down_right = C.x <= M.x && M.x <= D.x && C.y <= M.y
    // && M.y <= D.y; bool is_on_segment_down_left = C.x >= M.x && M.x >= D.x
    // && C.y <= M.y && M.y <= D.y; bool is_on_segment_up_left = C.x >= M.x &&
    // M.x
    // >= D.x && C.y >= M.y && M.y >= D.y; bool is_on_segment_up_right = C.x <=
    // M.x && M.x <= D.x && C.y >= M.y && M.y >= D.y;

    bool is_on_segment_down_right = LessOrEquals(segment.start.x, point.x)
                                    && LessOrEquals(point.x, segment.end.x)
                                    && LessOrEquals(segment.start.y, point.y)
                                    && LessOrEquals(point.y, segment.end.y);

    bool is_on_segment_down_left = GreaterOrEquals(segment.start.x, point.x)
                                   && GreaterOrEquals(point.x, segment.end.x)
                                   && LessOrEquals(segment.start.y, point.y)
                                   && LessOrEquals(point.y, segment.end.y);

    bool is_on_segment_up_left = GreaterOrEquals(segment.start.x, point.x)
                                 && GreaterOrEquals(point.x, segment.end.x)
                                 && GreaterOrEquals(segment.start.y, point.y)
                                 && GreaterOrEquals(point.y, segment.end.y);

    bool is_on_segment_up_right = LessOrEquals(segment.start.x, point.x)
                                  && LessOrEquals(point.x, segment.end.x)
                                  && GreaterOrEquals(segment.start.y, point.y)
                                  && GreaterOrEquals(point.y, segment.end.y);

    return (is_on_segment_down_right || is_on_segment_down_left
            || is_on_segment_up_left || is_on_segment_up_right);
}

void AttachMouseControls(SoftBody *soft_bodies, int soft_bodies_length,
                         MouseState *mouse_state)
{
    Vector2 mouse_position = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)
        && !CheckCollisionPointRec(mouse_position, mouse_state->deadzone))
        {
            if (!mouse_state->is_dragging)
                {
                    float closest_distance = FLT_MAX;
                    float curr_distance;
                    Particle *curr_particle;
                    for (int i = 0; i < soft_bodies_length; i++)
                        {
                            curr_particle = FindNearestParticleV(
                                mouse_position, &soft_bodies[i]);
                            curr_distance = Vector2Distance(
                                curr_particle->position, mouse_position);
                            if (curr_distance < closest_distance)
                                {
                                    mouse_state->particle = curr_particle;
                                    closest_distance = curr_distance;
                                }
                        }
                }
            DragParticleByMouse(mouse_state->particle, mouse_position);
            mouse_state->is_dragging = true;
        }
    else
        {
            mouse_state->is_dragging = false;
        }
}

void GetSoftBodySides(SoftBody *soft_body, Segment *sides)
{
    Particle *particle;
    Particle *next_particle;
    for (int i = 0; i < soft_body->particles_length - 1; i++)
        {
            particle = &soft_body->particles[i];
            next_particle = &soft_body->particles[i + 1];
            sides[i]
                = (Segment){ particle->position, next_particle->position };
        }
    sides[soft_body->particles_length - 1] = (Segment){
        soft_body->particles[soft_body->particles_length - 1].position,
        soft_body->particles[0].position
    };
}

void UpdateSoftBodyBoundingRect(SoftBody *soft_body)
{
    float top = FLT_MAX;
    float right = FLT_MIN;
    float bottom = FLT_MIN;
    float left = FLT_MAX;

    Particle *particle;
    for (int i = 0; i < soft_body->particles_length; i++)
        {
            particle = &soft_body->particles[i];
            if (particle->position.y < top)
                {
                    top = particle->position.y;
                }
            if (particle->position.x > right)
                {
                    right = particle->position.x;
                }
            if (particle->position.y > bottom)
                {
                    bottom = particle->position.y;
                }
            if (particle->position.x < left)
                {
                    left = particle->position.x;
                }
        }

    soft_body->bounding_rect.top = top;
    soft_body->bounding_rect.right = right;
    soft_body->bounding_rect.bottom = bottom;
    soft_body->bounding_rect.left = left;
    soft_body->bounding_rect.top_left = (Vector2){ left, top };
    soft_body->bounding_rect.top_right = (Vector2){ right, top };
    soft_body->bounding_rect.bottom_right = (Vector2){ right, bottom };
    soft_body->bounding_rect.bottom_left = (Vector2){ left, bottom };
}

int AreHSegmentNPointIntersecting(Segment horizontal_segment, Vector2 point)
{
    if (horizontal_segment.start.x <= point.x
        && point.x <= horizontal_segment.end.x)
        return FloatEquals(point.y, horizontal_segment.start.y);

    return 0;
}

bool AreSegmentsIntersecting(Segment segment1, Segment segment2)
{
    Vector2 A = segment1.start;
    Vector2 B = segment1.end;
    Vector2 C = segment2.start;
    Vector2 D = segment2.end;
    Vector2 AB = Vector2Subtract(B, A);
    Vector2 CD = Vector2Subtract(D, C);
    Vector2 AC = Vector2Subtract(C, A);

    float common_denominator = Vector2CrossProduct(CD, AB);
    if (common_denominator == 0)
        return false;

    float t = Vector2CrossProduct(CD, AC) / common_denominator;
    Vector2 M = Vector2Add(A, Vector2Scale(AB, t));

    bool is_inside_segment = 0 < t && t < 1;
    return is_inside_segment && IsPointOnSegment(M, segment2);
}

int GetPointToBodyIntersections(Vector2 *point, SoftBody *soft_body)
{
    int intersections = 0;
    UpdateSoftBodyBoundingRect(soft_body);
    BoundingRect rect = soft_body->bounding_rect;
    Vector2 right_point = { rect.right + 10, point->y };
    Segment horizontal_segment = { *point, right_point };

    // DrawLineV(horizontal_segment.start, horizontal_segment.end, BLUE);

    int sides_length = soft_body->particles_length;
    Segment sides[sides_length];
    GetSoftBodySides(soft_body, sides);
    for (int i = 0; i < sides_length; i++)
        {
            if (FloatEquals(point->y, soft_body->bounding_rect.top)
                || FloatEquals(point->y, soft_body->bounding_rect.bottom))
                continue;

            intersections += AreHSegmentNPointIntersecting(horizontal_segment,
                                                           sides[i].start);
            intersections
                += AreSegmentsIntersecting(horizontal_segment, sides[i]);
        }
    return intersections;
}

void BoundaryCollisionBox(Rectangle box, SoftBody *soft_body)
{
    for (int i = 0; i < soft_body->particles_length; ++i)
        {
            Particle *p = &soft_body->particles[i];
            if (p->position.x >= box.x + box.width)
                {
                    p->position.x = box.x + box.width;
                    p->velocity.x = -p->velocity.x;
                }
            if (p->position.x <= box.x)
                {
                    p->position.x = box.x;
                    p->velocity.x = -p->velocity.x;
                }
            if (p->position.y >= box.y + box.height)
                {
                    p->position.y = box.y + box.height;
                    p->velocity.y = -p->velocity.y;
                }
            if (p->position.y <= box.y)
                {
                    p->position.y = box.y;
                    p->velocity.y = -p->velocity.y;
                }
        }
}

void ResetSoftBodyCollisions(SoftBody *soft_body)
{
    for (int i = 0; i < soft_body->particles_length; i++)
        {
            soft_body->particles[i].is_colliding = false;
        }
}

bool IsPointOnSoftBody(Vector2 *point, SoftBody *soft_body)
{
    int sides_length = soft_body->particles_length;
    Segment sides[sides_length];
    GetSoftBodySides(soft_body, sides);
    for (int i = 0; i < soft_body->particles_length; i++)
        {
            if (IsPointOnSegment(*point, sides[i]))
                {
                    bool is_on_line
                        = Vector2CrossProduct(
                              Vector2Subtract(sides[i].end, sides[i].start),
                              Vector2Subtract(*point, sides[i].start))
                          == 0;
                    if (is_on_line)
                        {
                            return true;
                        }
                }
        }
    return false;
}

bool IsParticleIntersectingSoftBody(Particle *particle, SoftBody *soft_body)
{
    if (IsPointOnSoftBody(&particle->position, soft_body))
        return false;

    particle->num_intesecting
        = GetPointToBodyIntersections(&particle->position, soft_body);
    return particle->num_intesecting % 2 == 1;
}

void DetectCollisionSoftBodies(SoftBody *soft_body1, SoftBody *soft_body2)
{
    Particle *particle;
    for (int i = 0; i < soft_body1->particles_length; i++)
        {
            particle = &soft_body1->particles[i];
            particle->is_colliding
                = IsParticleIntersectingSoftBody(particle, soft_body2);
        }
}

Spring *GetClosestEdgeToPoint(SoftBody *soft_body, Vector2 point)
{
    Spring *closest_edge = &soft_body->springs[0];
    Vector2 closest_edge_middle = Vector2Add(
        Vector2Scale(Vector2Subtract(closest_edge->particles[0]->position,
                                     closest_edge->particles[1]->position),
                     0.5),
        closest_edge->particles[1]->position);
    float closest_distance
        = Vector2Length(Vector2Subtract(point, closest_edge_middle));
    Spring *curr_edge;
    for (int i = 0; i < soft_body->springs_length; ++i)
        {
            curr_edge = &soft_body->springs[i];
            Vector2 curr_edge_middle = Vector2Add(
                Vector2Scale(
                    Vector2Subtract(curr_edge->particles[0]->position,
                                    curr_edge->particles[1]->position),
                    0.5),
                curr_edge->particles[1]->position);
            float curr_distance
                = Vector2Length(Vector2Subtract(point, curr_edge_middle));
            if (curr_distance < closest_distance)
                {
                    closest_edge = curr_edge;
                    closest_edge_middle = Vector2Add(
                        Vector2Scale(Vector2Subtract(
                                         closest_edge->particles[0]->position,
                                         closest_edge->particles[1]->position),
                                     0.5),
                        closest_edge->particles[1]->position);
                    closest_distance = Vector2Length(
                        Vector2Subtract(point, closest_edge_middle));
                }
        }
    return closest_edge;
}

void HandleCollisionSoftBodies(SoftBody *soft_body1, SoftBody *soft_body2)
{
    Particle *p;
    for (int i = 0; i < soft_body1->particles_length; ++i)
        {
            p = &soft_body1->particles[i];
            if (!p->is_colliding)
                continue;
            Spring *edge = GetClosestEdgeToPoint(soft_body2, p->position);
            edge->color = YELLOW;
        }
}

Spring CreateSpring(Particle *particle1, Particle *particle2, float stiffness)
{
    float rest_length
        = Vector2Distance(particle1->position, particle2->position);
    Spring spring = {
        { particle1, particle2 },
        stiffness, rest_length, RAYWHITE
    };
    return spring;
}

void UpdateSpring(Spring *spring)
{
    Vector2 spring_vector = Vector2Subtract(spring->particles[0]->position,
                                            spring->particles[1]->position);
    float spring_length = Vector2Length(spring_vector);
    float delta_length = spring_length - spring->rest_length;
    Vector2 tension_direction = Vector2Normalize(spring_vector);

    Vector2 tension0 = Vector2Scale(tension_direction, -delta_length);
    Vector2 tension1 = Vector2Scale(tension_direction, delta_length);

    // TODO: use springs stiffness instead of default value
    spring->particles[0]->force = Vector2Add(
        spring->particles[0]->force,
        Vector2Scale(tension0, variable_constants.spring_stiffness));
    spring->particles[1]->force = Vector2Add(
        spring->particles[1]->force,
        Vector2Scale(tension1, variable_constants.spring_stiffness));

    DampenSpring(spring, tension_direction);
}

void DrawSpring(Spring *spring)
{
    DrawLineEx(spring->particles[0]->position, spring->particles[1]->position,
               variable_constants.spring_thickness, spring->color);
    // TODO: for debug only:
    spring->color = RAYWHITE;
}

void DampenSpring(Spring *spring, Vector2 tension_direction)
{
    Vector2 relative_velocity = Vector2Subtract(
        spring->particles[1]->velocity, spring->particles[0]->velocity);
    Vector2 dampening_force
        = Vector2Scale(tension_direction,
                       Vector2DotProduct(tension_direction, relative_velocity)
                           * variable_constants.spring_dampening);

    spring->particles[0]->force
        = Vector2Add(spring->particles[0]->force, dampening_force);
    spring->particles[1]->force = Vector2Add(
        spring->particles[1]->force, Vector2Scale(dampening_force, -1));
}

void UpdateAllSprings(Spring *springs, int springs_length)
{
    for (int i = 0; i < springs_length; i++)
        {
            UpdateSpring(&springs[i]);
        }
}

void DrawAllSprings(Spring *springs, int springs_length)
{
    for (int i = 0; i < springs_length; i++)
        {
            DrawSpring(&springs[i]);
        }
}

Particle CreateParticle(const char *id, Vector2 position, float mass)
{
    Particle particle = {
        .id = id,
        .position = position,
        .velocity = Vector2Zero(),
        .acceleration = Vector2Zero(),
        .force = Vector2Zero(),
        .mass = mass,
        .is_colliding = false,
    };
    return particle;
}

void UpdateParticle(Particle *particle, double dt)
{
    UpdateParticleAcceleration(particle);
    UpdateParticleVelocity(particle, dt);
    UpdateParticlePosition(particle, dt);
}

void DrawParticle(Particle *particle)
{
    Color draw_color = variable_constants.node_color;
    if (particle->is_colliding == true)
        {
            draw_color = RED;
        }
    DrawCircleV(particle->position, variable_constants.node_radius,
                draw_color);
}

void ResetParticleForces(Particle *particle)
{
    particle->force = Vector2Zero();
}

void UpdateParticleAcceleration(Particle *particle)
{
    particle->acceleration
        = Vector2Scale(particle->force, 1 / (particle->mass));
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
    particle->velocity
        = Vector2Scale(particle->velocity, 1 - variable_constants.friction);
}

void ClampParticleVelocity(Particle *particle)
{
    particle->velocity = Vector2ClampValue(
        particle->velocity, -variable_constants.max_velocity_value,
        variable_constants.max_velocity_value);
}

void UpdateParticlePosition(Particle *particle, double dt)
{
    Vector2 position_displacement = Vector2Scale(particle->velocity, dt);
    particle->position = Vector2Add(particle->position, position_displacement);
}

void DrawAllParticles(Particle *particles, int particles_length)
{
    for (int i = 0; i < particles_length; i++)
        {
            DrawParticle(&particles[i]);
        }
}

void UpdateAllParticles(Particle *particles, int particles_length, double dt)
{
    for (int i = 0; i < particles_length; i++)
        {
            UpdateParticle(&particles[i], dt);
        }
}

void ResetAllParticleForces(Particle *particles, int particles_length)
{
    for (int i = 0; i < particles_length; i++)
        {
            ResetParticleForces(&particles[i]);
        }
}

void UpdateAllParticleAccelerations(Particle *particles, int particles_length)
{
    for (int i = 0; i < particles_length; i++)
        {
            UpdateParticleAcceleration(&particles[i]);
        }
}

void UpdateAllParticleVelocities(Particle *particles, int particles_length,
                                 double dt)
{
    for (int i = 0; i < particles_length; i++)
        {
            UpdateParticleVelocity(&particles[i], dt);
        }
}

void ClampAllParticleVelocities(Particle *particles, int particles_length)
{
    for (int i = 0; i < particles_length; i++)
        {
            ClampParticleVelocity(&particles[i]);
        }
}

void UpdateAllParticlePositions(Particle *particles, int particles_length,
                                double dt)
{
    for (int i = 0; i < particles_length; i++)
        {
            UpdateParticlePosition(&particles[i], dt);
        }
}

Particle *FindNearestParticleV(Vector2 position, SoftBody *soft_body)
{
    float closest_distance = FLT_MAX;
    Particle *nearest_particle;
    for (int i = 0; i < soft_body->particles_length; i++)
        {
            float curr_distance
                = Vector2Distance(soft_body->particles[i].position, position);
            if (curr_distance < closest_distance)
                {
                    closest_distance = curr_distance;
                    nearest_particle = &soft_body->particles[i];
                }
        }
    return nearest_particle;
}

void DragParticleByMouse(Particle *particle, Vector2 mouse_position)
{
    particle->acceleration = Vector2Zero();
    particle->velocity = Vector2Zero();
    particle->position = mouse_position;
}

SoftBody CreateSoftBody(
    Particle *particles, int particles_length,
    //                        Particle *edge_particles, int
    //                        edge_particles_length,
    Spring *springs, int springs_length
    //                        Spring *edge_springs, int edge_springs_length
)
{
    SoftBody soft_body = {
        .particles = particles,
        .springs = springs,
        .particles_length = particles_length,
        .springs_length = springs_length,
        //        .edge_particles = edge_particles,
        //        .edge_springs = edge_springs,
        //        .edge_particles_length = edge_particles_length,
        //        .edge_springs_length = edge_springs_length,
    };
    return soft_body;
}

void Gravity(SoftBody *soft_body, Vector2 g)
{
    for (int i = 0; i < soft_body->particles_length; ++i)
        {
            Particle *p = &soft_body->particles[i];
            p->force = Vector2Add(p->force, Vector2Scale(g, p->mass));
        }
}

void UpdateSoftBody(SoftBody *soft_body, double dt)
{
    ResetAllParticleForces(soft_body->particles, soft_body->particles_length);
    Gravity(soft_body, variable_constants.gravity);
    UpdateAllSprings(soft_body->springs, soft_body->springs_length);
    UpdateAllParticles(soft_body->particles, soft_body->particles_length, dt);
}

void DrawSoftBody(SoftBody *soft_body)
{
    for (int i = 0; i < soft_body->springs_length; i++)
        {
            DrawSpring(&soft_body->springs[i]);
        }
    for (int i = 0; i < soft_body->particles_length; ++i)
        {
            DrawAllParticles(soft_body->particles,
                             soft_body->particles_length);
        }
}

Particle *GetParticleById(const char *id, Particle *particles,
                          size_t particles_length)
{
    Particle *particle = NULL;
    for (size_t i = 0; i < particles_length; ++i)
        {
            if (strcmp(id, particles[i].id) == 0)
                {
                    particle = &particles[i];
                    return particle;
                }
        }
    return particle;
}

// Methods for Simple Shapes
void InitSquare(SoftBody *soft_body, Particle *particles, Spring *springs,
                float init_x, float init_y, float width, float part_mass,
                float stiffness, int num_diagonals, bool flag_right_diagonal)
{
    assert(num_diagonals < 3);

    int num_vertices = 4;

    particles[0]
        = CreateParticle("p1", (Vector2){ init_x, init_y }, part_mass);
    particles[1]
        = CreateParticle("p2", (Vector2){ init_x + width, init_y }, part_mass);
    particles[2] = CreateParticle(
        "p3", (Vector2){ init_x + width, init_y + width }, part_mass);
    particles[3]
        = CreateParticle("p4", (Vector2){ init_x, init_y + width }, part_mass);

    springs[0] = CreateSpring(&particles[0], &particles[1], stiffness);
    springs[1] = CreateSpring(&particles[1], &particles[2], stiffness);
    springs[2] = CreateSpring(&particles[2], &particles[3], stiffness);
    springs[3] = CreateSpring(&particles[0], &particles[3], stiffness);

    int order_for_diagonals[4];
    if (flag_right_diagonal)
        {
            order_for_diagonals[0] = 1;
            order_for_diagonals[1] = 3;
            order_for_diagonals[2] = 0;
            order_for_diagonals[3] = 2;
        }
    else
        {
            order_for_diagonals[0] = 0;
            order_for_diagonals[1] = 2;
            order_for_diagonals[2] = 1;
            order_for_diagonals[3] = 3;
        }
    if (num_diagonals > 0)
        springs[4]
            = CreateSpring(&particles[order_for_diagonals[0]],
                           &particles[order_for_diagonals[1]], stiffness);
    if (num_diagonals == 2)
        springs[5]
            = CreateSpring(&particles[order_for_diagonals[2]],
                           &particles[order_for_diagonals[3]], stiffness);

    *soft_body = CreateSoftBody(particles, num_vertices, springs,
                                num_vertices + num_diagonals);
}

void ResetCollisions(SoftBody *soft_bodies, size_t soft_bodies_length)
{
    for (size_t i = 0; i < soft_bodies_length; ++i)
        {
            ResetSoftBodyCollisions(&soft_bodies[i]);
        }
}

void Collisions(SoftBody *soft_bodies, size_t soft_bodies_length)
{
    // WARNING: This is not working as expected.
    for (size_t i = 0; i < soft_bodies_length; ++i)
        {
            for (size_t j = 0; j < soft_bodies_length; ++j)
                {
                    if (i != j)
                        {
                            DetectCollisionSoftBodies(&soft_bodies[i],
                                                      &soft_bodies[j]);
                        }
                }
        }
}

void UpdateSoftBodies(SoftBody *soft_bodies, size_t soft_bodies_length,
                      double dt)
{
    for (size_t i = 0; i < soft_bodies_length; ++i)
        {
            UpdateSoftBody(&soft_bodies[i], dt);
        }
}

void BoundaryCollisionBoxAll(SoftBody *soft_bodies, size_t soft_bodies_length,
                             Rectangle box)
{
    for (size_t i = 0; i < soft_bodies_length; ++i)
        {
            BoundaryCollisionBox(box, &soft_bodies[i]);
        }
}

void DrawSoftBodies(SoftBody *soft_bodies, size_t soft_bodies_length)
{
    for (size_t i = 0; i < soft_bodies_length; ++i)
        {
            DrawSoftBody(&soft_bodies[i]);
        }
}

int JsonParseSoftBody(const char *file_path, JsonSoftBody *soft_body_json)
{
    json_t *root;
    json_error_t error;

    root = json_load_file(file_path, 0, &error);

    if (!root)
        {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return 1;
        }

    soft_body_json->particles_json = json_object_get(root, "particles");
    soft_body_json->springs_json = json_object_get(root, "springs");

    *soft_body_json->particles_length
        = json_array_size(soft_body_json->particles_json);
    *soft_body_json->springs_length
        = json_array_size(soft_body_json->springs_json);

    return 0;
}

void JsonInitSoftBody(const char *file_path, SoftBody *soft_body,
                      JsonSoftBody *soft_body_json)
{

    json_t *particle_json;
    json_t *spring_json;

    for (size_t i = 0; i < soft_body->particles_length; ++i)
        {
            particle_json = json_array_get(soft_body_json->particles_json, i);
            const char *particle_id
                = json_string_value(json_object_get(particle_json, "id"));

            json_t *position_x_json = json_object_get(particle_json, "x");
            json_t *position_y_json = json_object_get(particle_json, "y");

            float position_x;
            float position_y;

            if (json_is_real(position_x_json))
                {
                    position_x = json_real_value(position_x_json);
                    position_y = json_real_value(position_y_json);
                }
            else
                {
                    position_x = json_integer_value(position_x_json);
                    position_y = json_integer_value(position_y_json);
                }

            soft_body->particles[i] = CreateParticle(
                particle_id, (Vector2){ position_x, position_y }, 5.0);
        }

    for (size_t i = 0; i < soft_body->springs_length; ++i)
        {

            spring_json = json_array_get(soft_body_json->springs_json, i);
            const char *p1_id
                = json_string_value(json_object_get(spring_json, "p1"));
            const char *p2_id
                = json_string_value(json_object_get(spring_json, "p2"));

            Particle *p1_ptr = GetParticleById(p1_id, soft_body->particles,
                                               soft_body->particles_length);
            Particle *p2_ptr = GetParticleById(p2_id, soft_body->particles,
                                               soft_body->particles_length);

            soft_body->springs[i]
                = CreateSpring(p1_ptr, p2_ptr, DEFAULT_SPRING_STIFFNESS);
        }
}
