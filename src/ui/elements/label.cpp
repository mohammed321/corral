#include "label.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "../../resource_manager.h"

Label::Label(const ViewStyle &style, SDL_Renderer *renderer, const std::string &text, int point_size, SDL_Color text_color)
:   View(style, renderer),
    m_text(text),
    m_point_size(point_size),
    m_text_color(text_color)
{
    set_text_texture();
}

void Label::set_text_texture() {
    float label_w = 0.0f;
    float label_h = 0.0f;

    if (m_text_texture) {
        SDL_DestroyTexture(m_text_texture);
    }
    
    SDL_Surface* text_surface = TTF_RenderText_Blended(font_manager.get_font_for_point_size(m_point_size), m_text.c_str(), 0, m_text_color);

    if (!text_surface) {
        m_text_texture = nullptr;
    }
    else {
        m_text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
        SDL_DestroySurface(text_surface);
        label_w = m_text_texture->w;
        label_h = m_text_texture->h;
    }

    YGNodeStyleSetWidth(m_layout_node, label_w);
    YGNodeStyleSetHeight(m_layout_node, label_h);
}

void Label::on_update()
{
}

void Label::on_render()
{
    if (m_text_texture) SDL_RenderTexture(m_renderer, m_text_texture, nullptr, &m_bounds);
}

void Label::set_text(const std::string &text)
{
    m_text = text;
    set_text_texture();
}

const std::string &Label::get_text()
{
    return m_text;
}

void Label::set_point_size(int point_size)
{
    m_point_size = point_size;
    set_text_texture();
}

bool Label::handle_event(Event &event)
{
    return false;
}
