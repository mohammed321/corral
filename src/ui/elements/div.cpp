#include "div.h"
#include "SDL3/SDL.h"

Div::Div(const ViewStyle &style, SDL_Renderer *renderer) : View(style, renderer)
{
    YGNodeStyleSetWidthPercent(m_layout_node, 100.0f);
}

void Div::on_update()
{
}

void Div::on_render()
{
    SDL_SetRenderDrawColor(m_renderer, m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
    SDL_RenderFillRect(m_renderer, &m_bounds);
}