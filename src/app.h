#pragma once

#include <string>

#include "SDL3/SDL.h"
#include "yoga/Yoga.h"
#include "ui/elements/div.h"
#include "game.h"
#include "ui/view_controller.h"

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
    bool m_resized = true;

    ViewController* ui_view_controller;

    inline void loop();
    void update();
    void render();
    void handle_input();
};
