#include "dropdown.h"
#include "button.h"

Dropdown::Dropdown(const DropdownStyle &style, SDL_Renderer *renderer, const std::vector<ListItem>& items)
: View(style.view_style, renderer)
{
    YGNodeStyleSetWidth(m_layout_node, 160);
    // YGNodeStyleSetHeight(m_layout_node, 40);
    style.set_style_for_dropdown(*this);
    dropdown_list = new DropdownList(View::ViewStyle{ .backgroundColor = SDL_Color{255, 0, 0, 255}, z_index = 1}, renderer, items);
    dropdown_list->hide();
    insert_child(dropdown_list);
}

void Dropdown::on_update()
{
    switch (drop_down_list_state)
    {
    case open:
        if (!hovered && !dropdown_list->hovered) {
            drop_down_list_state = closing;
        }
        break;
    case opening:
        dropdown_list->show();
        drop_down_list_state = open;
        break;
    case closed:
        if (hovered) {
            drop_down_list_state = opening;
        }
        break;
    case closing:
            dropdown_list->hide();
            drop_down_list_state = closed;
        break;
    default:
        break;
    }
}

void Dropdown::on_render()
{
    SDL_SetRenderDrawColor(m_renderer, m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
    SDL_RenderFillRect(m_renderer, &m_bounds);
}

void Dropdown::on_enter(InputState& input_state)
{
    hovered = true;
}

void Dropdown::on_leave(InputState &input_state)
{
    hovered = false;
}

//**********************************************************//
//********************** DropdownList **********************//
//**********************************************************//

DropdownList::DropdownList(const ViewStyle &style, SDL_Renderer *renderer, const std::vector<ListItem>& items)
: View(style, renderer) {
    YGNodeStyleSetPositionType(m_layout_node, YGPositionTypeAbsolute);
    YGNodeStyleSetWidthPercent(m_layout_node, 100.0f);
    YGNodeStyleSetHeight(m_layout_node, 200.0f);
    // YGNodeStyleSetPosition(m_layout_node, YGEdgeLeft, 0);
    // YGNodeStyleSetPositionPercent(m_layout_node, YGEdgeLeft, 0);
    YGNodeStyleSetPositionPercent(m_layout_node, YGEdgeTop, 100.0f);

    for (const auto& item : items) {
        insert_child(new Button({}, renderer, item, nullptr, nullptr));
    }
}

void DropdownList::on_update()
{
}

void DropdownList::on_render()
{
    SDL_SetRenderDrawColor(m_renderer, m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
    SDL_RenderFillRect(m_renderer, &m_bounds);
}

void DropdownList::on_enter(InputState &input_state)
{
    hovered = true;
}

void DropdownList::on_leave(InputState &input_state)
{
    hovered = false;
}

DropdownListItem::DropdownListItem(const ViewStyle &style, SDL_Renderer *renderer, const std::string &text)
:   View(style, renderer),
    m_text(text)
{
}

void DropdownListItem::on_update()
{
}

void DropdownListItem::on_render()
{
}

void DropdownListItem::on_enter(InputState &input_state)
{
}

void DropdownListItem::on_leave(InputState &input_state)
{
}
