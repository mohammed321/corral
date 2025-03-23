#include "button.h"
#include "SDL3/SDL.h"

Button::Button(const ViewStyle &style, SDL_Renderer *renderer) : View(style, renderer)
{
    YGNodeStyleSetWidth(m_layout_node, 50);
    YGNodeStyleSetHeight(m_layout_node, 50);
}

void Button::on_update()
{
}

void Button::on_render()
{
    SDL_SetRenderDrawColor(m_renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(m_renderer, &m_bounds);
}