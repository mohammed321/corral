#pragma once

#include <string>

#include "SDL3/SDL.h"
#include "yoga/Yoga.h"
#include "ui/elements/div.h"
#include "game.h"

struct ApplicationSpecification {
    int width = 1000;
    int height = 1000;
    const char *title = "app title";
};

class Application {
public:
    Application(const ApplicationSpecification& spec = ApplicationSpecification());
    ~Application();

    void run();

private:
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_renderer = nullptr;

    bool m_running = true;
    bool m_calc_layout = true;

    Div* m_ui_root;
    Div* m_header;
    Game* m_game;

    inline void loop();
    void update();
    void render();
    void handle_input();

    inline void render_ui(YGNodeRef root, uint8_t c);
};
