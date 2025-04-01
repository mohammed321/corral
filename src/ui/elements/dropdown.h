#pragma once

#include <string>
#include <vector>

#include "../view.h"

using ListItem = std::string;

class DropdownListItem : public View {
    DropdownListItem(const ViewStyle &style, SDL_Renderer *renderer, const std::string& text);

protected:
    void on_update() override;
    void on_render() override;
    void on_enter(InputState &input_state) override;
    void on_leave(InputState &input_state) override;

private:
    std::string m_text;
};

class DropdownList : public View
{
    friend class Dropdown;
public:
    DropdownList(const ViewStyle &style, SDL_Renderer *renderer, const std::vector<ListItem>& items);

protected:
    void on_update() override;
    void on_render() override;
    void on_enter(InputState &input_state) override;
    void on_leave(InputState &input_state) override;

private:
    bool hovered = false;
};

class Dropdown : public View
{

public:
    struct DropdownStyle
    {
        ViewStyle view_style;
        optional<SDL_Color> hover_color;
        inline void set_style_for_dropdown(Dropdown &dropdown) const
        {
            if (!view_style.backgroundColor.has_value())
            {
                dropdown.m_background_color = {2, 80, 112, SDL_ALPHA_OPAQUE};
            }
            if (hover_color.has_value())
            {
                dropdown.m_hover_color = hover_color.value();
            }
            else
            {
                dropdown.m_hover_color = {67, 150, 183, SDL_ALPHA_OPAQUE};
            }
        }
    };

public:
    Dropdown(const DropdownStyle &style, SDL_Renderer *renderer, const std::vector<ListItem>& items);

protected:
    void on_update() override;
    void on_render() override;
    void on_enter(InputState &input_state) override;
    void on_leave(InputState &input_state) override;
    // void on_mouse_down() override;
    // void on_click() override;
    // void on_resize() override;

private:
    SDL_Color m_hover_color;
    SDL_Color m_pressed_color;

    SDL_Texture *m_text_texture;
    SDL_Texture *m_Dropdown_texture = nullptr;
    SDL_Texture *m_Dropdown_hovered_texture = nullptr;

    DropdownList *dropdown_list;

    bool hovered = false;
    enum {
        opening,
        open,
        closing,
        closed
    } drop_down_list_state = closed;

    // void set_text_texture();
    // void set_Dropdown_texture();
};