#include <iostream>
#include "resource_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

FontManager font_manager;

// Function to load an image using stb_image and create an SDL_Texture
SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* file) {
    int width, height, channels;
    
    // Load image using stb_image
    unsigned char* data = stbi_load(file, &width, &height, &channels, 4); // Force 4 channels (RGBA)
    if (!data) {
        std::cerr << "Failed to load image: " << file << " - " << stbi_failure_reason() << std::endl;
        return nullptr;
    }

    // Create SDL surface
    SDL_Surface* surface = SDL_CreateSurfaceFrom(
        width,
        height,
        SDL_PIXELFORMAT_RGBA8888,
        data,
        width * 4
    );

    if (!surface) {
        std::cerr << "Failed to create SDL surface: " << SDL_GetError() << std::endl;
        stbi_image_free(data);
        return nullptr;
    }

    // Convert to SDL Texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface); // Free SDL surface
    stbi_image_free(data);    // Free stb_image raw data

    if (!texture) {
        std::cerr << "Failed to create SDL texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    return texture;
}

FontManager::~FontManager()
{
    for (auto& [pt, font] : m_fonts) {
        TTF_CloseFont(font);
    }
}

TTF_Font *FontManager::get_font_for_point_size(int point_size)
{
    if (!m_fonts.contains(point_size)) {
        TTF_Font* font = TTF_OpenFont("res/OpenSans-Medium.ttf", static_cast<float>(point_size));
        if (!font) {
            SDL_Log("Couldn't load font: %s\n", SDL_GetError());
            std::exit(EXIT_FAILURE);
        }
        m_fonts[point_size] = font;
    }

    return m_fonts[point_size];
}

TTF_Font *FontManager::get_icon_font_for_point_size(int point_size)
{
    if (!m_icon_fonts.contains(point_size)) {
        TTF_Font* font = TTF_OpenFont("res/icomoon.ttf", static_cast<float>(point_size));
        if (!font) {
            SDL_Log("Couldn't load font: %s\n", SDL_GetError());
            std::exit(EXIT_FAILURE);
        }
        m_icon_fonts[point_size] = font;
    }

    return m_icon_fonts[point_size];
}
