#include "dropdown.h"
#include "div.h"

Dropdown::Dropdown(const DropdownStyle &style, SDL_Renderer *renderer, const std::vector<std::string>& items, dropdown_selected_callback selected_callback, void* ctx)
: View({
    .flexDirection = YGFlexDirectionRow,
    .justify_content = YGJustifySpaceBetween,
    .alignItems = YGAlignCenter,
    .padding = 16.0f,
    }, renderer),
    m_selected_callback(selected_callback),
    m_selected_callback_ctx(ctx)
{
    style.set_style_for_dropdown(*this);

    m_dropdown_list = new DropdownList(View::ViewStyle{.z_index = 1}, renderer, this, items);
    m_dropdown_list->hide();
    insert_child(m_dropdown_list);

    m_selected_label = new Label({.margin_right = 32.0}, renderer, m_dropdown_list->m_items[m_selected]->get_text());
    insert_child(m_selected_label);

    m_icon = new Icon({}, renderer, 0xea43);
    insert_child(m_icon);
}

void Dropdown::select(size_t index)
{
    m_drop_down_list_state = closing;
    if (m_selected != index) {
        m_selected = index;
        m_drop_down_list_state = selection_changed;
        m_selected_callback(m_selected_callback_ctx, index);
    }
}

void Dropdown::on_update()
{
    switch (m_drop_down_list_state)
    {
    case open:
        if (!m_hovered && !m_dropdown_list->m_hovered) {
            m_drop_down_list_state = closing;
        }
        break;
    case opening:
        m_dropdown_list->show();
        m_drop_down_list_state = open;
        break;
    case closed:
        if (m_hovered) {
            m_drop_down_list_state = opening;
        }
        break;
    case closing:
            m_dropdown_list->hide();
            m_drop_down_list_state = closed;
        break;
    case selection_changed:
        m_selected_label->set_text(m_dropdown_list->m_items[m_selected]->get_text());
        m_drop_down_list_state = closing;
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

void Dropdown::on_enter(InputState* input_state)
{
    m_hovered = true;
}

void Dropdown::on_leave(InputState* input_state)
{
    m_hovered = false;
}

//**********************************************************//
//********************** DropdownList **********************//
//**********************************************************//

DropdownList::DropdownList(const ViewStyle &style, SDL_Renderer *renderer, Dropdown* dropdown, const std::vector<std::string>& items)
: View(style, renderer) {
    YGNodeStyleSetPositionType(m_layout_node, YGPositionTypeAbsolute);
    YGNodeStyleSetMinWidthPercent(m_layout_node, 100.0f);
    YGNodeStyleSetPosition(m_layout_node, YGEdgeLeft, 0);
    // YGNodeStyleSetPositionPercent(m_layout_node, YGEdgeLeft, 0);
    YGNodeStyleSetPositionPercent(m_layout_node, YGEdgeTop, 100.0f);

    for (size_t i = 0; i < items.size(); ++i) {
        DropdownListItem* child = new DropdownListItem({}, renderer, dropdown, items[i], i);
        child->set_filter(this);
        insert_child(child);
        m_items.push_back(child);
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

void DropdownList::on_enter(InputState* input_state)
{
    m_hovered = true;
}

void DropdownList::on_leave(InputState* input_state)
{
    m_hovered = false;
}

bool DropdownList::filter_event(View *watched_view, Event &event)
{
    switch (event.get_type())
    {
    case Event::enter:
    case Event::leave:
        handle_event(event);
        break;
    default:
        break;
    }

    return true;
}

DropdownListItem::DropdownListItem(const ViewStyle &style, SDL_Renderer *renderer, Dropdown* dropdown, const std::string &text, size_t index)
:   View(style, renderer),
    m_dropdown(dropdown),
    m_index(index)
{
    m_button = new Button({.view_style = {.border = 0.0}}, renderer, text, [](void* ctx) {
        auto item = static_cast<DropdownListItem*>(ctx);
        item->m_dropdown->select(item->m_index);
    }, this);
    m_button->set_filter(this);
    insert_child(m_button);
}

const std::string &DropdownListItem::get_text()
{
    return m_button->get_text();
}

void DropdownListItem::on_update()
{
}

void DropdownListItem::on_render()
{
}

bool DropdownListItem::filter_event(View *watched_view, Event &event)
{
    switch (event.get_type())
    {
    case Event::enter:
    case Event::leave:
        handle_event(event);
        break;
    default:
        break;
    }

    return true;
}
