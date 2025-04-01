#pragma once

#include <optional>

#include "SDL3/SDL.h"
#include "yoga/Yoga.h"
#include "../input.h"

using std::optional;

class ViewController ;

class View {

public:
    struct ViewStyle {
        optional<YGFlexDirection> flexDirection;
        optional<YGJustify> justify_content;
        optional<YGAlign> alignContent;
        optional<YGAlign> alignItems;
        optional<float> flexGrow;
        optional<float> flexShrink;
        optional<float> padding;
        optional<float> gapLength;
        optional<SDL_Color> backgroundColor;
        optional<int> z_index;

        inline void set_style_for_view(View& view) const {
            // layout props
            YGNodeRef layout_node = view.m_layout_node;
            if (flexDirection.has_value()) {
                YGNodeStyleSetFlexDirection(layout_node, flexDirection.value());
            }
            if (flexGrow.has_value()) {
                YGNodeStyleSetFlexGrow(layout_node, flexGrow.value());
            }
            if (flexShrink.has_value()) {
                YGNodeStyleSetFlexShrink(layout_node, flexShrink.value());
            }
            if (padding.has_value()) {
                YGNodeStyleSetPadding(layout_node, YGEdgeAll, padding.value());
            }
            if (gapLength.has_value()) {
                YGNodeStyleSetGap(layout_node, YGGutterRow, gapLength.value());
            }
            if (justify_content.has_value()) {
                YGNodeStyleSetJustifyContent(layout_node, justify_content.value());
            }
            if (alignContent.has_value()) {
                YGNodeStyleSetAlignContent(layout_node, alignContent.value());
            }
            if (alignItems.has_value()) {
                YGNodeStyleSetAlignItems(layout_node, alignItems.value());
            }
            // view props
            if (backgroundColor.has_value()) {
                view.m_background_color = backgroundColor.value();
            }
            else {
                // view.m_background_color = { 100, 100, 100, SDL_ALPHA_OPAQUE };
                view.m_background_color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
            }
            if (z_index.has_value()) {
                view.z_index = z_index.value();
            }
        }
    };

public:
    View(const ViewStyle& style, SDL_Renderer* renderer);
    virtual ~View();

    void render();
    void update();

    void show();
    void hide();

    void insert_child(const View* child);
    void calc_layout(float w, float h);
    bool is_dirty();
    bool is_showing();
    bool contains_point(const SDL_FPoint& point);
    inline int get_z_index() {
        return z_index;
    }

    size_t get_child_count();
    View* get_child(size_t index);

protected:
    YGNodeRef m_layout_node;
    SDL_Renderer *m_renderer;
    SDL_FRect m_bounds;
    SDL_Color m_background_color;
    int z_index = 0;

    virtual bool handle_event();
    virtual void on_update() = 0;
    virtual void on_render() = 0;
    virtual void on_resize();

    virtual void on_enter(InputState& input_state);
    virtual void on_leave(InputState& input_state);
    virtual void on_mouse_down(InputState& input_state);
    virtual void on_mouse_up(InputState& input_state);
    virtual void on_mouse_move(InputState& input_state);

private:
    void update_bounds();
    
    inline View* parent_view() {
        const YGNodeRef parent_layout_node = YGNodeGetParent(m_layout_node);
        if (parent_layout_node) {
            return static_cast<View*>(YGNodeGetContext(parent_layout_node));
        }
        return nullptr;
    }

    bool is_ancestor_of(View* potential_descendant);

public:
    friend class ViewController;
};