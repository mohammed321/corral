#include <cstdlib>

#include "app.h"
#include "path.h"

#include "ui/elements/button.h"

using ViewStyle = View::ViewStyle;

Application::Application(const ApplicationSpecification &spec)
{
    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    if (!SDL_CreateWindowAndRenderer(spec.title, spec.width, spec.height, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    m_ui_root = new Div(ViewStyle{
        .flexDirection = YGFlexDirectionColumn,
        .padding = 10.0,
        .gapLength = 5
    }, m_renderer);

    m_header = new Div(ViewStyle{
                            .flexDirection = YGFlexDirectionRow,
                            .padding = 10.0f,
                            .backgroundColor = SDL_Color{50, 50, 0, 255}
                        }, m_renderer);

    auto button = new Button(ViewStyle{
        .backgroundColor = SDL_Color{200, 0, 0, 255}
    }, m_renderer);

    m_header->insert_child(*button, 0);

    m_game = new Game(m_renderer);

    m_ui_root->insert_child(*m_header, 0);
    m_ui_root->insert_child(*m_game, 1);
}

Application::~Application()
{
    delete m_ui_root;
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Application::run()
{
    uint64_t frame_delay = 1000/60;
    m_running = true;
    m_calc_layout = true;
    while (m_running) {
        uint64_t frame_start = SDL_GetTicks();
        loop();
        uint64_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < frame_delay) {
            SDL_Delay(frame_delay - frame_time);
        }
    }
}

inline void Application::loop()
{
    update();
    render();
}

void Application::update()
{
    handle_input();
    if (m_calc_layout) {
        int w,h;
        if(!SDL_GetWindowSize(m_window, &w, &h)) {
            SDL_Log("Couldn't get window size: %s", SDL_GetError());
            std::exit(EXIT_FAILURE);
        }
        m_ui_root->calc_layout(w, h);
        m_calc_layout = false;
    }

    m_ui_root->update();
}

void Application::render()
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(m_renderer);
    m_ui_root->render();
    SDL_RenderPresent(m_renderer);
}

void Application::handle_input()
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_running = false;
        }

        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            m_calc_layout = true;
        }
    }
}
