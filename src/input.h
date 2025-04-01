#pragma once

#include "SDL3/SDL.h"

struct InputState {
    SDL_FPoint pointer_pos;
    SDL_FPoint prev_pointer_pos;
    bool mouse_is_down = false;
    bool mouse_is_pressed_down_this_frame = false;
    bool mouse_is_released_up_this_frame = false;
};