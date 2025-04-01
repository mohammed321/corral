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

    Grid(size_t size, SDL_Renderer* renderer);

    void on_render() override;
    void on_resize() override;
    void on_update() override;

    size_t get_size();
    void new_puzzle();
    void reset_puzzle();

private:

    struct {
        bool mouse_clicked_in_bag = false;
        bool mouse_clicked_out_of_bag = false;
    } m_extra_input_state;

    size_t m_size;
    float m_cell_size;
    float m_grid_size;
    float m_line_width = 3.0;
    float m_bag_border_thickness = 8.0;
    float m_padding = 5.0;

    SDL_Texture* m_grid_texture = nullptr;
    SDL_Texture* m_bag_border_texture = nullptr;
    SDL_Texture* m_text_texture = nullptr;
    
    bool m_should_highlight_square = false;
    CellPosition m_highlight_square;
    SDL_Color m_highlight_square_color;

    inline float calc_cell_size() 
    {
        const float cell_size = (m_bounds.w - (m_line_width * (m_size + 1)) - (2 * m_padding))/m_size;
        return SDL_floorf(cell_size);
    }
    inline float calc_grid_size() 
    {
        const float grid_size = m_cell_size * m_size + (m_line_width * (m_size + 1));;
        return SDL_floorf(grid_size);
    }

    inline SDL_FRect get_frect_for_pos(CellPosition pos, float x_offset = 0, float y_offset = 0)
    {
        const SDL_FRect rect = {
            .x = pos.j * (m_cell_size + m_line_width) + m_line_width + x_offset + m_padding,
            .y = pos.i * (m_cell_size + m_line_width) + m_line_width + y_offset + m_padding,
            .w = m_cell_size,
            .h = m_cell_size
        }; 
        return rect;
    }

    inline SDL_Rect get_rect_for_pos(CellPosition pos, int x_offset = 0, int y_offset = 0)
    {
        const SDL_Rect rect = {
            .x = static_cast<int>(pos.j * (m_cell_size + m_line_width) + m_line_width + x_offset + m_padding),
            .y = static_cast<int>(pos.i * (m_cell_size + m_line_width) + m_line_width + y_offset + m_padding),
            .w = static_cast<int>(m_cell_size),
            .h = static_cast<int>(m_cell_size)
        }; 
        return rect;
    }

    void on_enter(InputState& input_state) override;
    void on_leave(InputState& input_state) override;
    void on_mouse_down(InputState& input_state) override;
    void on_mouse_up(InputState& input_state) override;
    void on_mouse_move(InputState& input_state) override;

    void handle_input(CellPosition pos, InputState& input_state);

    void render_cell_target(CellPosition pos, int32_t target, SDL_Surface* text_surface);
    void fill_cell(CellPosition pos, SDL_Color color);

    void set_grid_texture();
    void set_bag_border_texture();
    void set_text_texture();

    inline void set_textures() 
    {
        set_grid_texture();
        set_bag_border_texture();
        set_text_texture();
    }
    
    plutovg_point_t cell_position_to_point(const CellPosition &pos);
};
