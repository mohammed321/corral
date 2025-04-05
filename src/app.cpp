#include <cstdlib>

#include "app.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "resource_manager.h"
#include "ui/elements/button.h"
#include "ui/elements/dropdown.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

using ViewStyle = View::ViewStyle;
using ButtonStyle = Button::ButtonStyle;
using DropdownStyle = Dropdown::DropdownStyle;

View* build_ui_tree(SDL_Renderer *renderer) {
    auto root = new Div(ViewStyle{
        .flexDirection = YGFlexDirectionColumn,
        .alignItems = YGAlignCenter,
        .gapLength = 5
    }, renderer);

    auto game = new Game(renderer);

    auto header = new Div(ViewStyle{
                            .flexDirection = YGFlexDirectionRow,
                            .justify_content = YGJustifySpaceAround,
                            .padding = 6.0f,
                            .backgroundColor = SDL_Color{225, 238, 243, 255}
                        }, renderer);
    
    auto difficulty_dropdown = new Dropdown(
        {
            .view_style = {},
        }, 
        renderer,
        { "easy 4x4", "medium 6x6", "hard 10x10"},
        [](void* ctx, size_t index){Game::set_current_grid(static_cast<Game*>(ctx), index);},
        static_cast<void*>(game)
    );
   

    auto reset_puzzle_button = new Button({
        .view_style = { .border_radius = 10.0f },
    }, renderer, "reset", [](void* game){ static_cast<Game*>(game)->reset_puzzle(); }, game);

    auto new_puzzle_button = new Button({
        .view_style = { .border_radius = 10.0f },
    }, renderer, "new puzzle", [](void* game){ static_cast<Game*>(game)->new_puzzle(); }, game);

    header->insert_child(difficulty_dropdown);
    header->insert_child(reset_puzzle_button);
    header->insert_child(new_puzzle_button);

    auto footer = new Div(ViewStyle{
        .flexDirection = YGFlexDirectionRow,
        .justify_content = YGJustifySpaceAround,
        .backgroundColor = SDL_Color{130, 130, 130, 255}
    }, renderer);

    root->insert_child(header);
    root->insert_child(game);
    root->insert_child(footer);

    return root;
}

Application::Application(const ApplicationSpecification &spec)
{
    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    if (!SDL_CreateWindowAndRenderer(spec.title, spec.width, spec.height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY, &m_window, &m_renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialise SDL_ttf: %s\n", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    load_resources();

    ui_view_controller = new ViewController(m_renderer, build_ui_tree(m_renderer));
}

Application::~Application()
{
    delete ui_view_controller;
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Application::run()
{
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(loop, 0, 1);
#else
    constexpr int fps = 60;
    constexpr uint64_t frame_delay = 1000/fps;
    m_running = true;
    while (m_running) {
        uint64_t frame_start = SDL_GetTicks();
        loop();
        uint64_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < frame_delay) {
            SDL_Delay(frame_delay - frame_time);
        }
    }
#endif
}

inline void Application::loop()
{

#if defined(PLATFORM_WEB)
    if (!m_running) {
        emscripten_cancel_main_loop();
        return;
    }
#endif

    update();
    render();
}

void Application::update()
{
    handle_input();
    if (m_resized) {
        int w,h;
        if(!SDL_GetWindowSizeInPixels(m_window, &w, &h)) {
            SDL_Log("Couldn't get window size: %s", SDL_GetError());
            std::exit(EXIT_FAILURE);
        }
        ui_view_controller->resize(w, h);
        m_resized = false;
    }
    ui_view_controller->update();
}

void Application::render()
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(m_renderer);
    ui_view_controller->render();
    SDL_RenderPresent(m_renderer);
}

void Application::handle_input()
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                m_running = false;
                break;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_EXPOSED:
                m_resized = true;
                break;
        }
    }
}
