#pragma once

#include "../view.h"

class Button : public View {
public:
    Button(const ViewStyle& style, SDL_Renderer* renderer);
    void on_update() override;
    void on_render() override;
};