#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>
#include "puzzle.h"
#include "ui/view.h"
#include "SDL3/SDL.h"
#include "plutovg/plutovg.h"

class Grid : public View{
public:
    std::unique_ptr<Puzzle> m_puzzle;
    bool m_enabled = true;

    Grid(std::unique_ptr<Puzzle> puzzle, SDL_Renderer* renderer);

    void on_render() override;
    void on_resize() override;
    void on_update() override;

    size_t get_size();

    struct InputState {
        bool mouse_clicked_in_bag = false;
        bool mouse_clicked_out_of_bag = false;
        bool clicked_this_frame = false;
        bool mouse_down = false;
        bool mouse_released_this_frame = false;
    } m_input_state;

private:
    size_t m_size;
    float m_cell_size;
    float m_line_width = 1.0;
    float m_bag_border_thickness = 5.0;
    float m_padding = 20.0;

    SDL_Texture* m_grid_texture = nullptr;
    SDL_Texture* m_bag_border_texture = nullptr;
    
    bool m_should_highlight_square = false;
    CellPosition m_highlight_square;
    SDL_Color m_highlight_square_color;

    inline float calc_cell_size() 
    {
        return (m_bounds.w - (m_line_width * (m_size + 1)) - (2 * m_padding))/m_size;
    }

    void handle_input(CellPosition pos, SDL_MouseButtonFlags mouse_state);

    void render_cell_target(CellPosition pos, int32_t target);
    void fill_cell(SDL_Renderer *renderer, CellPosition pos, SDL_Color color);

    void set_grid_texture(SDL_Renderer *renderer);
    void set_bag_border_texture(SDL_Renderer *renderer);
    plutovg_point_t cell_position_to_point(const CellPosition &pos);
};
