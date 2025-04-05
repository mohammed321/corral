#pragma once

#include <string>

#include "../view.h"

class Icon : public View {
public:
    Icon(const ViewStyle& style, SDL_Renderer* renderer, const uint32_t code_point);
    void on_update() override;
    void on_render() override;

    void set_code_point(const uint32_t code_point);

protected:
    bool handle_event(Event& event) override;

private:

    uint32_t m_code_point;
    SDL_Texture* m_icon_texture = nullptr;
    void set_icon_texture();
    
};