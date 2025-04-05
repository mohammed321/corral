#include "icon.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "../../resource_manager.h"

Icon::Icon(const ViewStyle &style, SDL_Renderer *renderer, const uint32_t code_point)
:   View(style, renderer),
    m_code_point(code_point)
{
    set_icon_texture();
}

void Icon::set_icon_texture() {
    float label_w = 0.0f;
    float label_h = 0.0f;

    if (m_icon_texture) {
        SDL_DestroyTexture(m_icon_texture);
    }
    
    SDL_Surface* icon_surface = TTF_RenderGlyph_Blended(font_manager.get_icon_font_for_point_size(42), m_code_point, {255, 255, 255, 255});

    if (!icon_surface) {
        m_icon_texture = nullptr;
    }
    else {
        m_icon_texture = SDL_CreateTextureFromSurface(m_renderer, icon_surface);
        SDL_DestroySurface(icon_surface);
        label_w = m_icon_texture->w;
        label_h = m_icon_texture->h;
    }

    YGNodeStyleSetWidth(m_layout_node, label_w);
    YGNodeStyleSetHeight(m_layout_node, label_h);
}

void Icon::on_update()
{
}

void Icon::on_render()
{
    if (m_icon_texture) SDL_RenderTexture(m_renderer, m_icon_texture, nullptr, &m_bounds);
}

void Icon::set_code_point(const uint32_t code_point)
{
    m_code_point = code_point;
    set_icon_texture();
}

bool Icon::handle_event(Event &event)
{
    return false;
}
