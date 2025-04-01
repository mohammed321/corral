#pragma once

#include <string>

#include "../view.h"

typedef void (*FuncPtr)(void*);

class Button : public View {

public:
    struct ButtonStyle {
        ViewStyle view_style;
        optional<SDL_Color> hover_color;
        optional<SDL_Color> pressed_color;
        inline void set_style_for_button(Button& button) const {
            if (!view_style.backgroundColor.has_value()) {
                button.m_background_color = { 90, 123, 192, SDL_ALPHA_OPAQUE };
            }
            if (hover_color.has_value()) {
                button.m_hover_color = hover_color.value();
            }
            else {
                button.m_hover_color = { 112, 145, 213, SDL_ALPHA_OPAQUE };
            }
            if (pressed_color.has_value()) {
                button.m_pressed_color = pressed_color.value();
            }
            else {
                button.m_pressed_color = { 67, 99, 167, SDL_ALPHA_OPAQUE };
            }

            if (!view_style.padding.has_value()) {
                YGNodeStyleSetPadding(button.m_layout_node, YGEdgeAll, 8.0f);
            }
        }
    };

public:
    Button(const ButtonStyle& style, SDL_Renderer* renderer, std::string text, FuncPtr on_click_callback, void* ctx);

protected:
    void on_update() override;
    void on_render() override;
    void on_enter(InputState& input_state) override;
    void on_leave(InputState& input_state) override;
    void on_mouse_down(InputState& input_state) override;
    void on_mouse_up(InputState& input_state) override;
    void on_resize() override;

    void on_click(InputState& input_state);

private:
    SDL_Color m_hover_color;
    SDL_Color m_pressed_color;
    std::string m_text;
    void* m_ctx;
    FuncPtr m_on_click_callback;
    SDL_Texture* m_text_texture;
    SDL_Texture* m_button_texture = nullptr;
    SDL_Texture* m_button_hovered_texture = nullptr;
    SDL_Texture* m_button_pressed_texture = nullptr;

    struct ButtonState {
        bool button_down = false;
        bool button_hovered = false;
    } m_button_state;

    void set_text_texture();
    void set_button_texture();
    
};