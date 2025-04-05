#include "button.h"
#include "SDL3/SDL.h"
#include "../../common.h"
#include "plutovg.h"
#include <tuple>


Button::Button(const ButtonStyle &style, SDL_Renderer *renderer, const std::string& text, FuncPtr on_click_callback, void* ctx) 
:   View(style.view_style, renderer),
    m_text(text), 
    m_on_click_callback(on_click_callback),
    m_ctx(ctx)
{
    style.set_style_for_button(*this);
    m_label = new Label({}, renderer, text);
    insert_child(m_label);
}

const std::string &Button::get_text()
{
    return m_label->get_text();
}

void Button::set_button_texture()
{
    const float border_width = YGNodeLayoutGetBorder(m_layout_node, YGEdgeLeft);
    const float w = SDL_ceilf(m_bounds.w);
    const float h = SDL_ceilf(m_bounds.h);
    
    plutovg_surface_t* surface = plutovg_surface_create(w, h);
    plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

    if (m_border_radius > 0) {
        plutovg_canvas_round_rect(canvas, border_width, border_width, w - 2*border_width, h - 2*border_width, m_border_radius, m_border_radius);
    } else {
        plutovg_canvas_rect(canvas, border_width, border_width, w - 2*border_width, h - 2*border_width);
    }
    plutovg_canvas_save(canvas);

    const auto button_texture_data = {   
        std::tie(m_button_texture, m_background_color),
        std::tie(m_button_hovered_texture, m_hover_color),
        std::tie(m_button_pressed_texture, m_pressed_color),
    };

    for (auto& [texture, button_color] : button_texture_data) {
        plutovg_canvas_set_rgb(canvas, button_color.r/255.0, button_color.g/255.0, button_color.b/255.0);
        plutovg_canvas_fill_preserve(canvas);

        if (border_width > 0) {
            plutovg_canvas_set_line_width(canvas, border_width);
            plutovg_canvas_set_rgb(canvas, 0, 0, 0);
            plutovg_canvas_stroke_preserve(canvas);
        }

        SDL_Surface* sdl_surface = SDL_CreateSurfaceFrom(
            plutovg_surface_get_width(surface), 
            plutovg_surface_get_height(surface),
            SDL_PIXELFORMAT_ARGB8888,
            plutovg_surface_get_data(surface),
            plutovg_surface_get_stride(surface)
        );

        if (texture) {
            SDL_DestroyTexture(texture);
        }
        texture = SDL_CreateTextureFromSurface(m_renderer, sdl_surface);
        SDL_DestroySurface(sdl_surface);
        plutovg_canvas_restore(canvas);
    }

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
    
}

void Button::on_enter(InputState* input_state)
{
    m_button_state.button_hovered = true;
}

void Button::on_leave(InputState* input_state)
{
    m_button_state.button_hovered = false;
}

void Button::on_mouse_down(InputState* input_state)
{
    m_button_state.button_down = true;
}

void Button::on_mouse_up(InputState* input_state)
{
    m_button_state.button_down = false;
    if (contains_point(input_state->pointer_pos)) {
        on_click(input_state);
    }
}

void Button::on_click(InputState* input_state)
{
    m_on_click_callback(m_ctx);
}

void Button::on_resize()
{
    set_button_texture();
}
