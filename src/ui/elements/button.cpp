#include "button.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "../../resource_manager.h"
#include "../../common.h"
#include "plutovg/plutovg.h"


Button::Button(const ButtonStyle &style, SDL_Renderer *renderer, std::string text, FuncPtr on_click_callback, void* ctx) 
:   View(style.view_style, renderer),
    m_text(text), 
    m_on_click_callback(on_click_callback),
    m_ctx(ctx)
{
    YGNodeStyleSetWidth(m_layout_node, 160);
    YGNodeStyleSetHeight(m_layout_node, 40);
    style.set_style_for_button(*this);
}

void Button::set_text_texture() {
    const float w = SDL_ceilf(m_bounds.w);
    const float h = SDL_ceilf(m_bounds.h);
    SDL_Surface* text_surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ARGB8888);
    SDL_Surface* text = TTF_RenderText_Blended(font_manager.get_font_for_point_size(28), m_text.c_str(), 0, {255, 255, 255,255});
    SDL_Rect dst_rect = {
        .x =  static_cast<int>((w - text->w) / 2),
        .y =  static_cast<int>((h - text->h) / 2),
        .w = text->w,
        .h = text->h,
    };

    SDL_BlitSurface(text, nullptr, text_surface, &dst_rect);

    if (m_text_texture) {
        SDL_DestroyTexture(m_text_texture);
    }
    m_text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
    SDL_DestroySurface(text_surface);
}

void Button::set_button_texture()
{
    const float border_size = 1.0f;
    const float w = SDL_ceilf(m_bounds.w);
    const float h = SDL_ceilf(m_bounds.h);
    
    plutovg_surface_t* surface = plutovg_surface_create(w, h);
    plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

    plutovg_canvas_round_rect(canvas, border_size, border_size, w - 2*border_size, h - 2*border_size, 10.0f, 10.0f);
    plutovg_canvas_save(canvas);

    // normall state
    plutovg_canvas_set_rgb(canvas, m_background_color.r/255.0, m_background_color.g/255.0, m_background_color.b/255.0);
    plutovg_canvas_fill_preserve(canvas);

    plutovg_canvas_set_line_width(canvas, border_size);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_stroke_preserve(canvas);

    SDL_Surface* sdl_surface = SDL_CreateSurfaceFrom(
        plutovg_surface_get_width(surface), 
        plutovg_surface_get_height(surface),
        SDL_PIXELFORMAT_ARGB8888,
        plutovg_surface_get_data(surface),
        plutovg_surface_get_stride(surface)
    );

    if (m_button_texture) {
        SDL_DestroyTexture(m_button_texture);
    }
    m_button_texture = SDL_CreateTextureFromSurface(m_renderer, sdl_surface);
    SDL_DestroySurface(sdl_surface);
    plutovg_canvas_restore(canvas);

    // hovered state
    plutovg_canvas_set_rgb(canvas, m_hover_color.r/255.0, m_hover_color.g/255.0, m_hover_color.b/255.0);
    plutovg_canvas_fill_preserve(canvas);

    plutovg_canvas_set_line_width(canvas, border_size);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_stroke_preserve(canvas);

    sdl_surface = SDL_CreateSurfaceFrom(
        plutovg_surface_get_width(surface), 
        plutovg_surface_get_height(surface),
        SDL_PIXELFORMAT_ARGB8888,
        plutovg_surface_get_data(surface),
        plutovg_surface_get_stride(surface)
    );

    if (m_button_hovered_texture) {
        SDL_DestroyTexture(m_button_hovered_texture);
    }
    m_button_hovered_texture = SDL_CreateTextureFromSurface(m_renderer, sdl_surface);
    SDL_DestroySurface(sdl_surface);
    plutovg_canvas_restore(canvas);

    // pressed state
    plutovg_canvas_set_rgb(canvas, m_pressed_color.r/255.0, m_pressed_color.g/255.0, m_pressed_color.b/255.0);
    plutovg_canvas_fill_preserve(canvas);

    plutovg_canvas_set_line_width(canvas, border_size);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_stroke_preserve(canvas);

    sdl_surface = SDL_CreateSurfaceFrom(
        plutovg_surface_get_width(surface), 
        plutovg_surface_get_height(surface),
        SDL_PIXELFORMAT_ARGB8888,
        plutovg_surface_get_data(surface),
        plutovg_surface_get_stride(surface)
    );

    if (m_button_pressed_texture) {
        SDL_DestroyTexture(m_button_pressed_texture);
    }
    m_button_pressed_texture = SDL_CreateTextureFromSurface(m_renderer, sdl_surface);
    SDL_DestroySurface(sdl_surface);

    plutovg_canvas_destroy(canvas);
    plutovg_surface_destroy(surface);
}

void Button::on_update()
{
}

void Button::on_render()
{
    SDL_Texture* current_texture;
    if (m_button_state.button_down) {
        current_texture = m_button_pressed_texture;
    }
    else if (m_button_state.button_hovered) {
        current_texture = m_button_hovered_texture;
    }
    else {
        current_texture = m_button_texture;
    }

    if (current_texture) SDL_RenderTexture(m_renderer, current_texture, nullptr, &m_bounds);
    if (m_text_texture) SDL_RenderTexture(m_renderer, m_text_texture, nullptr, &m_bounds);
}

void Button::on_enter(InputState& input_state)
{
    m_button_state.button_hovered = true;
}

void Button::on_leave(InputState &input_state)
{
    m_button_state.button_hovered = false;
}

void Button::on_mouse_down(InputState& input_state)
{
    m_button_state.button_down = true;
}

void Button::on_mouse_up(InputState &input_state)
{
    if (m_button_state.button_down) {
        m_button_state.button_down = false;
        on_click(input_state);
    }
}

void Button::on_click(InputState& input_state)
{
    m_on_click_callback(m_ctx);
}

void Button::on_resize()
{
    set_button_texture();
    set_text_texture();
}
