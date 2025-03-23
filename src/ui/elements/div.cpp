#include "div.h"
#include "SDL3/SDL.h"

Div::Div(const ViewStyle &style, SDL_Renderer *renderer) : View(style, renderer)
{
}

void Div::on_update()
{
}

void Div::on_render()
{
    SDL_SetRenderDrawColor(m_renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(m_renderer, &m_bounds);
}