#include <memory>
#include <format>

#include "game.h"
#include "resource_manager.h"


Game::Game(SDL_Renderer* renderer) : View(ViewStyle{ .flexDirection = YGFlexDirectionColumn, .flexShrink = 1.0f, .padding = 10.0f }, renderer)
{
    YGNodeStyleSetAlignItems(m_layout_node, YGAlignCenter);
    m_grid4x4 = new Grid{Puzzle::generate_puzzle(4), renderer};
    m_grid6x6 = new Grid{Puzzle::generate_puzzle(6), renderer};
    m_grid10x10 = new Grid{Puzzle::generate_puzzle(10), renderer};

    insert_child(*m_grid4x4, 0);
    // insert_child(*m_grid6x6, 1);
    // insert_child(*m_grid10x10, 2);

    m_grid4x4->show();
    m_grid6x6->hide();
    m_grid10x10->hide();
    m_current_grid = m_grid4x4;
}

void Game::on_update()
{
    m_current_grid->m_enabled = true;
    if (m_current_grid->m_puzzle->is_solved()) {
        m_current_grid->m_enabled = false;
    }
}

void Game::on_render()
{
    SDL_SetRenderDrawColor(m_renderer, 200, 0, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_renderer, &m_bounds);
}
