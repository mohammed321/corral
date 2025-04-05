#pragma once

#include <vector>
#include <unordered_map>

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

class FontManager {
public:
    ~FontManager();
    TTF_Font* get_font_for_point_size(int point_size);
    TTF_Font* get_icon_font_for_point_size(int point_size);
private:
    std::unordered_map<int, TTF_Font*> m_fonts;
    std::unordered_map<int, TTF_Font*> m_icon_fonts;
};

extern FontManager font_manager;

SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* file);
