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

void View::on_resize()
{
}

void View::render()
{
    on_render();
    // render children
    const size_t child_count = YGNodeGetChildCount(m_layout_node);
    for (size_t i = 0; i < child_count; i++) {
        YGNodeRef child = YGNodeGetChild(m_layout_node, i);
        if (YGNodeStyleGetDisplay(child) != YGDisplayNone) {
            View* child_view = static_cast<View*>(YGNodeGetContext(child));
            child_view->render();
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

void View::insert_child(View &child, size_t index)
{
    YGNodeInsertChild(m_layout_node, child.m_layout_node, index);
}

void View::calc_layout(float w, float h)
{
    YGNodeCalculateLayout(m_layout_node, w, h, YGDirectionLTR);
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

    m_bounds.x = ax + YGNodeLayoutGetLeft(m_layout_node);
    m_bounds.y = ay + YGNodeLayoutGetTop(m_layout_node);
    m_bounds.w = YGNodeLayoutGetWidth(m_layout_node);
    m_bounds.h = YGNodeLayoutGetHeight(m_layout_node);
}
