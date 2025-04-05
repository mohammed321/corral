#pragma once

#include <optional>
#include <utility>

#include "SDL3/SDL.h"
#include "yoga/Yoga.h"
#include "../input.h"
#include "../events/event.h"

using std::optional;

class ViewController ;

class View {

public:
    struct ViewStyle {
        optional<YGFlexDirection> flexDirection;
        optional<YGJustify> justify_content;
        optional<YGAlign> alignContent;
        optional<YGAlign> alignItems;
        optional<YGAlign> align_self;
        optional<float> flexGrow;
        optional<float> flexShrink;
        optional<float> padding;
        optional<float> margin;
        optional<float> margin_right;
        optional<float> border;
        optional<float> gapLength;
        optional<float> border_radius;
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
            if (margin.has_value()) {
                YGNodeStyleSetMargin(layout_node, YGEdgeAll, margin.value());
            }
            if (margin_right.has_value()) {
                YGNodeStyleSetMargin(layout_node, YGEdgeRight, margin_right.value());
            }
            if (border.has_value()) {
                YGNodeStyleSetBorder(layout_node, YGEdgeAll, border.value());
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
            if (align_self.has_value()) {
                YGNodeStyleSetAlignSelf(layout_node, align_self.value());
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
                view.m_z_index = z_index.value();
            }
            if (border_radius.has_value()) {
                view.m_border_radius = border_radius.value();
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
        return m_z_index;
    }

    inline bool is_enabled() {
        return m_enabled;
    }

    inline void set_filter(View* filter_view) {
        m_filter_view = filter_view;
    }

    size_t get_child_count();
    View* get_child(size_t index);

protected:
    YGNodeRef m_layout_node;
    SDL_Renderer *m_renderer;
    SDL_FRect m_bounds;
    SDL_Color m_background_color;
    int m_z_index = 0;
    float m_border_radius = 0.0f;

    bool m_enabled = true;

    virtual void on_update() = 0;
    virtual void on_render() = 0;
    virtual void on_resize();
    
    virtual bool handle_event(Event& event);
    virtual bool filter_event(View* watched_view, Event& event);
    // event handlers
    virtual void on_enter(InputState* input_state);
    virtual void on_leave(InputState* input_state);
    virtual void on_mouse_down(InputState* input_state);
    virtual void on_mouse_up(InputState* input_state);
    virtual void on_mouse_move(InputState* input_state);

private:
    View* m_filter_view = nullptr;

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