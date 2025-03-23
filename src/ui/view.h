#pragma once

#include <optional>

#include "SDL3/SDL.h"
#include "yoga/Yoga.h"

using std::optional;


class View {

public:
    struct ViewStyle {
        optional<YGFlexDirection> flexDirection;
        optional<float> flexGrow;
        optional<float> flexShrink;
        optional<float> padding;
        optional<float> gapLength;
        optional<SDL_Color> backgroundColor;

        inline void set_style_for_view(View& view) const {
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
            if (backgroundColor.has_value()) {
                view.backgroundColor = backgroundColor.value();
            }
            else {
                view.backgroundColor = { 100, 100, 100, SDL_ALPHA_OPAQUE };
            }
        }
    };

public:
    View(const ViewStyle& style, SDL_Renderer* renderer);
    virtual ~View();

    virtual void on_update() = 0;
    virtual void on_render() = 0;
    virtual void on_resize();

    void render();
    void update();

    void show();
    void hide();

    void insert_child(View& child, size_t index);
    void calc_layout(float w, float h);

protected:
    YGNodeRef m_layout_node;
    SDL_Renderer *m_renderer;
    SDL_FRect m_bounds;
    SDL_Color backgroundColor;

private:
    void update_bounds();
};