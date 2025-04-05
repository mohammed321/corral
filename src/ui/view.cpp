#include "view.h"

View::View(const ViewStyle& style, SDL_Renderer *renderer) :  m_renderer(renderer)
{
    m_layout_node = YGNodeNew();
    YGNodeSetContext(m_layout_node, this);
    style.set_style_for_view(*this);
}

View::~View()
{
    const size_t child_count = YGNodeGetChildCount(m_layout_node);
    for (size_t i = 0; i < child_count; i++) {
        YGNodeRef child = YGNodeGetChild(m_layout_node, 0);
        YGNodeRemoveChild(m_layout_node, child);
        delete static_cast<View*>(YGNodeGetContext(child));
    }
    YGNodeFree(m_layout_node);
}

bool View::handle_event(Event& event)
{
    if (m_filter_view) {
        m_filter_view->filter_event(this, event);
    }

    switch (event.get_type())
    {
        case Event::enter:
            on_enter(event.m_input_state);
            break;
        case Event::leave:
            on_leave(event.m_input_state);
            break;
        case Event::mouse_down:
            on_mouse_down(event.m_input_state);
            break;
        case Event::mouse_up:
            on_mouse_up(event.m_input_state);
            break;
        case Event::mouse_move:
            on_mouse_move(event.m_input_state);
            break;
    
    default:
        return false;
    }

    return true;
}

bool View::filter_event(View* watched_view, Event& event)
{
    return true;
}

void View::on_resize()
{
    // code that should run if bounds change
}

void View::on_enter(InputState* input_state)
{
}

void View::on_leave(InputState* input_state)
{
}

void View::on_mouse_down(InputState* input_state)
{
}

void View::on_mouse_up(InputState* input_state)
{
}

void View::on_mouse_move(InputState* input_state)
{
}

void View::render()
{
    on_render();
}

void set_all_children_has_new_layout(YGNodeRef node) {
    const size_t child_count = YGNodeGetChildCount(node);
    for (size_t i = 0; i < child_count; i++) {
        YGNodeRef child = YGNodeGetChild(node, i);
        if (YGNodeStyleGetDisplay(child) != YGDisplayNone) {
            YGNodeSetHasNewLayout(child, true);
        }
    }
}

void View::update()
{
    if (YGNodeGetHasNewLayout(m_layout_node)) {
        update_bounds();
        on_resize();
        YGNodeSetHasNewLayout(m_layout_node, false);
    }
    on_update();
    // update children
    const size_t child_count = YGNodeGetChildCount(m_layout_node);
    for (size_t i = 0; i < child_count; i++) {
        YGNodeRef child = YGNodeGetChild(m_layout_node, i);
        if (YGNodeStyleGetDisplay(child) != YGDisplayNone) {
            View* child_view = static_cast<View*>(YGNodeGetContext(child));
            child_view->update();
        }
    }
}

void View::show()
{
    YGNodeStyleSetDisplay(m_layout_node, YGDisplayFlex);
}

void View::hide()
{
    YGNodeStyleSetDisplay(m_layout_node, YGDisplayNone);
}

void View::insert_child(const View* child)
{
    size_t index = YGNodeGetChildCount(m_layout_node);
    YGNodeInsertChild(m_layout_node, child->m_layout_node, index);
}

void View::calc_layout(float w, float h)
{
    YGNodeCalculateLayout(m_layout_node, w, h, YGDirectionLTR);
}

bool View::is_dirty()
{
    return YGNodeIsDirty(m_layout_node);
}

bool View::is_showing()
{
    return YGNodeStyleGetDisplay(m_layout_node) != YGDisplayNone; 
}

bool View::contains_point(const SDL_FPoint& point)
{
    return SDL_PointInRectFloat(&point, &m_bounds);
}

size_t View::get_child_count()
{
    return YGNodeGetChildCount(m_layout_node);
}

View *View::get_child(size_t index)
{
    YGNodeRef child = YGNodeGetChild(m_layout_node, index);
    View* child_view = static_cast<View*>(YGNodeGetContext(child));
    return child_view;
}

void View::update_bounds()
{
    float ax = 0.0;
    float ay = 0.0;
    YGNodeRef parent = YGNodeGetParent(m_layout_node);
    if (parent) {
        View* parent_view = static_cast<View*>(YGNodeGetContext(parent));
        ax = parent_view->m_bounds.x;
        ay = parent_view->m_bounds.y;
    }

    ax += YGNodeLayoutGetLeft(m_layout_node);
    ay += YGNodeLayoutGetTop(m_layout_node);
    // we need this check because yg keeps track of relative position and not
    // absolute position so if an abs pos changes but the relative pos is the same 
    // it counts that as no layout change we add this check to make sure we update the children layout
    if (m_bounds.x != ax || m_bounds.y != ay) {
        set_all_children_has_new_layout(m_layout_node);
    }
    m_bounds.x = ax;
    m_bounds.y = ay;
    m_bounds.w = YGNodeLayoutGetWidth(m_layout_node);
    m_bounds.h = YGNodeLayoutGetHeight(m_layout_node);
}

bool View::is_ancestor_of(View *potential_descendant)
{
    if (!potential_descendant) {
        return false;
    }

    if (this == potential_descendant) {
        return true;
    }

    return is_ancestor_of(potential_descendant->parent_view());
}
