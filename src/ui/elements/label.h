#pragma once

#include <string>

#include "../view.h"

class Label : public View {
public:
    Label(const ViewStyle& style, SDL_Renderer* renderer, const std::string& text, int point_size = 48, SDL_Color text_color = {255, 255, 255, SDL_ALPHA_OPAQUE});
    void on_update() override;
    void on_render() override;

    void set_text(const std::string& text);
    const std::string& get_text();

    void set_point_size(int point_size);

protected:
    bool handle_event(Event& event) override;

private:

    std::string m_text;
    int m_point_size;
    SDL_Color m_text_color;
    SDL_Texture* m_text_texture = nullptr;
    void set_text_texture();
    
};