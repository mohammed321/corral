#pragma once

#include <string>
#include <vector>

#include "../view.h"
#include "label.h"
#include "icon.h"
#include "button.h"

typedef void (*dropdown_selected_callback)(void* ctx, size_t index);

class Dropdown;

class DropdownListItem : public View {
    friend class Dropdown;

public:
    DropdownListItem(const ViewStyle &style, SDL_Renderer *renderer, Dropdown* dropdown, const std::string &text, size_t index);
    const std::string& get_text();
protected:
    void on_update() override;
    void on_render() override;
    bool filter_event(View* watched_view, Event& event) override;

private:

    size_t m_index;
    Button* m_button;
    Dropdown* m_dropdown;
};

class DropdownList : public View
{
    friend class Dropdown;
public:
    DropdownList(const ViewStyle &style, SDL_Renderer *renderer, Dropdown* dropdown, const std::vector<std::string>& items);

protected:
    void on_update() override;
    void on_render() override;
    void on_enter(InputState* input_state) override;
    void on_leave(InputState* input_state) override;
    bool filter_event(View* watched_view, Event& event) override;

private:
    bool m_hovered = false;
    std::vector<DropdownListItem*> m_items;
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
                dropdown.m_background_color = { 67, 99, 167, SDL_ALPHA_OPAQUE };
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
    Dropdown(const DropdownStyle &style, SDL_Renderer *renderer, const std::vector<std::string>& items, dropdown_selected_callback selected_callback, void* ctx);
    void select(size_t index);

protected:
    void on_update() override;
    void on_render() override;
    void on_enter(InputState* input_state) override;
    void on_leave(InputState* input_state) override;

private:
    SDL_Color m_hover_color;

    SDL_Texture *m_Dropdown_texture = nullptr;
    SDL_Texture *m_Dropdown_hovered_texture = nullptr;

    DropdownList *m_dropdown_list = nullptr;
    Label* m_selected_label = nullptr;
    Icon* m_icon = nullptr;
    dropdown_selected_callback m_selected_callback; 
    void* m_selected_callback_ctx;

    bool m_hovered = false;
    size_t m_selected = 0;
    enum {
        opening,
        open,
        closing,
        closed,
        selection_changed
    } m_drop_down_list_state = closed;
};