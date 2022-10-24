#include "box.h"

#include "../../../base/tools/render.h"

void box_t::get_box_bounds(c_baseentity* entity)
{
    vector3d min = entity->mins();
    vector3d max = entity->maxs();

    vector3d points[8] =
    {
        vector3d(min.x, min.y, min.z),
        vector3d(min.x, max.y, min.z),
        vector3d(max.x, max.y, min.z),
        vector3d(max.x, min.y, min.z),
        vector3d(max.x, max.y, max.z),
        vector3d(min.x, max.y, max.z),
        vector3d(min.x, min.y, max.z),
        vector3d(max.x, min.y, max.z)
    };

    vector3d transformed_points[8] = {};

    unsigned int valid_bounds = 0;

    vector2d points_to_screen[8] = {};
    for (int i = 0; i < 8; i++)
    {
        math::vector_transform(points[i], entity->coordinate_frame(), transformed_points[i]);

        if (!render::world_to_screen(transformed_points[i], points_to_screen[i]))
            continue;

        valid_bounds |= 1 << i;
    }

    this->valid = valid_bounds & 255; // bits from 1 to 128

    if (!this->valid)
        return;

    float left = points_to_screen[3].x;
    float top = points_to_screen[3].y;
    float right = points_to_screen[3].x;
    float bottom = points_to_screen[3].y;

    for (auto i = 0; i < 8; i++)
    {
        if (left > points_to_screen[i].x)
            left = points_to_screen[i].x;
        if (top < points_to_screen[i].y)
            top = points_to_screen[i].y;
        if (right < points_to_screen[i].x)
            right = points_to_screen[i].x;
        if (bottom > points_to_screen[i].y)
            bottom = points_to_screen[i].y;
    }

    this->x = left;
    this->y = bottom;
    this->w = right - left;
    this->h = top - bottom;
}