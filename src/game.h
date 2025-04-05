#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include "puzzle.h"
#include "grid.h"
#include "ui/view.h"

class Game : public View {
public:
    Game(SDL_Renderer* renderer);
    ~Game() override;

    void on_render() override;
    void on_update() override;

    void new_puzzle();
    void reset_puzzle();

    static void set_current_grid(Game* game, size_t index);

private:
    Grid* m_grid4x4;
    Grid* m_grid6x6;
    Grid* m_grid10x10;
    Grid* m_current_grid;
};