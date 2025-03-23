#pragma once

#include "../view.h"

class Div : public View {
public:
    Div(const ViewStyle& style, SDL_Renderer* renderer);
    void on_update() override;
    void on_render() override;
};