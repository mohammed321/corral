#pragma once

#include <vector>

#include "SDL3/SDL.h"

// extern SDL_Texture* arrow_icon;
// extern std::vector<Font> fonts;
// extern std::vector<uint16_t> font_sizes;

SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* file);
void load_resources();
void unload_resources();