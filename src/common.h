#pragma once

#include "SDL3/SDL.h"

SDL_Rect rect_from_frect(SDL_FRect frect) {
    return SDL_Rect{
        .x = static_cast<int>(frect.x),
        .y = static_cast<int>(frect.y),
        .w = static_cast<int>(frect.w),
        .h = static_cast<int>(frect.h)
    };

}