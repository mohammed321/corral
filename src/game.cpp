#include <memory>
#include <format>

#include "game.h"
#include "resource_manager.h"


Game::Game(SDL_Renderer* renderer) : View(ViewStyle{ .flexDirection = YGFlexDirectionColumn, .alignItems = YGAlignCenter, .flexShrink = 1.0f, .padding = 10.0f }, renderer)
{
    m_grid4x4 = new Grid{4, renderer};
    m_grid6x6 = new Grid{6, renderer};
    m_grid10x10 = new Grid{10, renderer};

    insert_child(m_grid4x4);
    insert_child(m_grid6x6);
    insert_child(m_grid10x10);

    m_grid4x4->show();
    m_grid6x6->hide();
    m_grid10x10->hide();
    m_current_grid = m_grid4x4;
}

Game::~Game()
{
    delete m_grid4x4;
    delete m_grid6x6;
    delete m_grid10x10;
}

void Game::on_update()
{
}

void Game::new_puzzle()
{
    m_current_grid->new_puzzle();
}

void Game::reset_puzzle()
{
    m_current_grid->reset_puzzle();
}

void Game::set_current_grid(Game *game, size_t index)
{
    game->m_current_grid->hide();
    Grid* grids[] = {game->m_grid4x4, game->m_grid6x6, game->m_grid10x10};
    game->m_current_grid = grids[index];
    game->m_current_grid->show();
}

void Game::on_render()
{
}
