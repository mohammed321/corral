#include <format>
#include <cstdlib>
#include "grid.h"
#include "resource_manager.h"
#include "yoga/Yoga.h"

Grid::Grid(std::unique_ptr<Puzzle> puzzle, SDL_Renderer* renderer) : View(ViewStyle{ }, renderer) ,m_puzzle(std::move(puzzle)), m_size(m_puzzle->get_size())
{
    // YGNodeStyleSetHeightPercent(m_layout_node, 100);
    // YGNodeStyleSetDisplay(m_layout_node, YGDisplayContents);
    YGNodeStyleSetWidthPercent(m_layout_node, 100);
    YGNodeStyleSetFlexShrink(m_layout_node, 1.0f);
    // YGNodeStyleSetFlexGrow(m_layout_node, 1.0f);
    // YGNodeStyleSetFlex(m_layout_node, 1.0f);
    YGNodeStyleSetAspectRatio(m_layout_node, 1.0f);
}

void Grid::on_resize()
{
    m_cell_size = calc_cell_size();
    set_grid_texture(m_renderer);
    set_bag_border_texture(m_renderer);
}

void Grid::on_update()
{   
    m_should_highlight_square = false;

    SDL_FPoint pointer_pos;
    SDL_MouseButtonFlags mouse_state = SDL_GetMouseState(&pointer_pos.x, &pointer_pos.y);
    m_input_state.mouse_released_this_frame = m_input_state.mouse_down && !(mouse_state & SDL_BUTTON_LMASK);
    m_input_state.clicked_this_frame = !m_input_state.mouse_down && (mouse_state & SDL_BUTTON_LMASK);
    m_input_state.mouse_down = mouse_state & SDL_BUTTON_LMASK;

    SDL_FRect content_rect = {
        .x = m_bounds.x + m_padding,
        .y = m_bounds.y + m_padding,
        .w = m_bounds.w - 2 * m_padding,
        .h = m_bounds.h - 2 * m_padding
    };
    
    if (SDL_PointInRectFloat(&pointer_pos, &content_rect)) {
        CellPosition pos = {
            static_cast<CellIndexType>((SDL_clamp(pointer_pos.y, content_rect.y, content_rect.y + content_rect.h - 1) - content_rect.y) / (m_cell_size + m_line_width)),
            static_cast<CellIndexType>((SDL_clamp(pointer_pos.x, content_rect.x, content_rect.x + content_rect.w - 1) - content_rect.x) / (m_cell_size + m_line_width))
        };
        handle_input(pos, mouse_state);
    }

}

void Grid::handle_input(CellPosition pos, SDL_MouseButtonFlags mouse_state)
{
    if (!m_input_state.mouse_down)
    {
        m_input_state.mouse_clicked_in_bag = false;
        m_input_state.mouse_clicked_out_of_bag = false;
    }

    if (m_enabled)
    {
        m_should_highlight_square = true;
        m_highlight_square = pos;

        bool in_bag = m_puzzle->is_in_bag(pos);
        bool can_flip = false;
        if (m_puzzle->can_remove_from_bag(pos) || m_puzzle->can_put_back_in_bag(pos)) 
        {
            can_flip = true;
        } 

        if (m_input_state.clicked_this_frame)
        {
            if (in_bag) 
            {
                m_input_state.mouse_clicked_in_bag = true;
            }
            else
            {
                m_input_state.mouse_clicked_out_of_bag = true;
            }

        }

        if (can_flip) 
        {
            if (in_bag)
            {
                m_highlight_square_color = {80, 80, 80, 100};
                if (m_input_state.mouse_down) {
                    if (m_input_state.mouse_clicked_in_bag)
                    {
                        m_puzzle->remove_from_bag(pos);
                        set_bag_border_texture(m_renderer);
                    }
                }
            }
            else 
            {
                m_highlight_square_color = {255, 255, 255, 150};
                if (m_input_state.mouse_down) {
                    if (m_input_state.mouse_clicked_out_of_bag)
                    {
                        m_puzzle->put_back_in_bag(pos);
                        set_bag_border_texture(m_renderer);
                    }
                }
            }

        } 
        else 
        {
            m_highlight_square_color = {200, 0, 0, 50};
            m_input_state.mouse_clicked_in_bag = false;
            m_input_state.mouse_clicked_out_of_bag = false;
        }
    }
}

void Grid::fill_cell(SDL_Renderer *renderer, CellPosition pos, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    const SDL_FRect rect = {
        .x = pos.j * (m_cell_size + m_line_width) + m_line_width + m_bounds.x + m_padding,
        .y = pos.i * (m_cell_size + m_line_width) + m_line_width + m_bounds.y + m_padding,
        .w = m_cell_size,
        .h = m_cell_size
    }; 

    SDL_RenderFillRect(renderer, &rect);
}

inline uint32_t absDifference(uint32_t a, uint32_t b) {
    if (a > b) {
        return a - b;
    } else {
        return b - a;
    }
}

uint32_t closest(const std::vector<uint16_t>& values, const uint32_t& value) {
    size_t min_idx = 0;
    uint32_t min_diff = absDifference(values[0], value);

    for (size_t i = 1; i < values.size(); ++i)
    {
        uint32_t diff = absDifference(values[i], value);
        if (diff < min_diff) {
            min_diff = diff;
            min_idx = i;
        }
    }

    return min_idx;
}

void Grid::render_cell_target(CellPosition pos, int32_t target) 
{
        uint32_t font_idx = closest(font_sizes, m_cell_size/2);
        uint32_t font_size = font_sizes[font_idx];

        Vector2 text_size = MeasureTextEx(fonts[font_idx], std::to_string(target).c_str(), font_size, 0);
        Vector2 text_pos = {
            m_bounds.x + pos.j * m_cell_size + (m_cell_size - text_size.x) / 2 ,
            m_bounds.y + pos.i * m_cell_size + (m_cell_size - text_size.y) / 2};
        DrawTextEx(fonts[font_idx], std::to_string(target).c_str(), text_pos, font_size, 0, BLACK);

        const int32_t current_score = m_puzzle->get_num_cells_visible_from(pos) - target;
        Color current_score_color;
        std::string current_score_str;
        if (current_score > 0)
        {
            current_score_str = std::format("+{}", current_score);
            current_score_color = RED;
        }
        else if (current_score < 0)
        {
            current_score_str = std::format("{}", current_score);
            current_score_color = RED;
        }
        else
        {
            current_score_str = "0";
            current_score_color = GREEN;
        }
        uint32_t second_font_idx = (font_idx < 3) ? 0 : font_idx - 3;
        uint32_t second_font_size = font_sizes[second_font_idx];
        // std::cout << ">>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        // std::cout << font_idx << " - " << font_size << std::endl;
        // std::cout << second_font_idx << " - " << second_font_size << std::endl;
        // std::cout << "<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
        DrawTextEx(
            fonts[second_font_idx],
            current_score_str.c_str(),
            {
                text_pos.x + text_size.x - 2,
                text_pos.y - text_size.y/5
            },
            second_font_size,
            0,
            current_score_color);
}

void Grid::set_grid_texture(SDL_Renderer *renderer) {

    const float grid_size = m_bounds.w - 2 * m_padding; 
    if (grid_size <= 0) {
        if (m_grid_texture) {
            SDL_DestroyTexture(m_grid_texture);
        }
        m_grid_texture = nullptr;
        return;
    }

    plutovg_surface_t* surface = plutovg_surface_create(m_bounds.w, m_bounds.w);
    plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

    float dashes[] = {m_line_width, m_cell_size};
    plutovg_canvas_set_dash_array(canvas, dashes, 2);
    plutovg_canvas_set_line_width(canvas, grid_size);
    plutovg_canvas_move_to(canvas, 0, grid_size/2);
    plutovg_canvas_line_to(canvas, grid_size, grid_size/2);
    plutovg_canvas_move_to(canvas, grid_size/2, 0);
    plutovg_canvas_line_to(canvas, grid_size/2, grid_size);

    plutovg_canvas_set_rgb(canvas, 0.8, 0.8, 0.8);
    plutovg_canvas_translate(canvas, m_padding, m_padding);
    plutovg_canvas_stroke(canvas);

    SDL_Surface* sdl_surface = SDL_CreateSurfaceFrom(
        plutovg_surface_get_width(surface), 
        plutovg_surface_get_height(surface),
        SDL_PIXELFORMAT_ARGB8888,
        plutovg_surface_get_data(surface),
        plutovg_surface_get_stride(surface)
    );

    if (m_grid_texture) {
        SDL_DestroyTexture(m_grid_texture);
    }
    m_grid_texture = SDL_CreateTextureFromSurface(renderer, sdl_surface);

    SDL_DestroySurface(sdl_surface);
    plutovg_canvas_destroy(canvas);
    plutovg_surface_destroy(surface);
}

plutovg_point_t Grid::cell_position_to_point(const CellPosition &pos)
{   
    return {
        pos.j * (m_cell_size + m_line_width),
        pos.i * (m_cell_size + m_line_width)
    };
}

void Grid::set_bag_border_texture(SDL_Renderer *renderer) {

    const float grid_size = m_bounds.w - 2 * m_padding; 
    if (grid_size <= 0) {
        if (m_bag_border_texture) {
            SDL_DestroyTexture(m_bag_border_texture);
        }
        m_bag_border_texture = nullptr;
        return;
    }

    std::vector<CellPosition> bag_border_points; // for drawing the loop around cells
    m_puzzle->trace_bag_border_points(bag_border_points);

    plutovg_surface_t* surface = plutovg_surface_create(m_bounds.w, m_bounds.w);
    plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

    const plutovg_point_t starting_point = cell_position_to_point(bag_border_points[0]);
    plutovg_canvas_move_to(canvas, starting_point.x, starting_point.y);
    for (auto &pos : bag_border_points) {
        const plutovg_point_t point = cell_position_to_point(pos);
        plutovg_canvas_line_to(canvas, point.x, point.y);
    }
    plutovg_canvas_close_path(canvas);
        
    plutovg_canvas_set_line_width(canvas, m_bag_border_thickness);
    plutovg_canvas_set_line_join(canvas, PLUTOVG_LINE_JOIN_ROUND);
    plutovg_canvas_set_rgb(canvas, 0.0, 0.0, 0.0);
    plutovg_canvas_translate(canvas, m_padding, m_padding);
    plutovg_canvas_stroke(canvas);

    SDL_Surface* sdl_surface = SDL_CreateSurfaceFrom(
        plutovg_surface_get_width(surface), 
        plutovg_surface_get_height(surface),
        SDL_PIXELFORMAT_ARGB8888,
        plutovg_surface_get_data(surface),
        plutovg_surface_get_stride(surface)
    );

    if (m_bag_border_texture) {
        SDL_DestroyTexture(m_bag_border_texture);
    }
    m_bag_border_texture = SDL_CreateTextureFromSurface(renderer, sdl_surface);

    SDL_DestroySurface(sdl_surface);
    plutovg_canvas_destroy(canvas);
    plutovg_surface_destroy(surface);
}

void Grid::on_render()
{
    SDL_SetRenderDrawColor(m_renderer, 130, 130, 130, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_renderer, &m_bounds);

    for (CellIndexType i = 0; i < m_size; i++)
    {
        for (CellIndexType j = 0; j < m_size; j++)
        {
            if (m_puzzle->is_in_bag({i, j}))
            {
                fill_cell(m_renderer, {i, j}, {255,255,255,255});
            }
            else
            {
                fill_cell(m_renderer, {i, j}, {130,130,130,255});
            }
        }
    }

    for (auto &[pos, target] : m_puzzle->get_targets())
    {
        render_cell_target(pos, target);
    }
    
    if (m_should_highlight_square)
    {
        fill_cell(m_renderer, m_highlight_square, m_highlight_square_color);
    }
    
    if (m_grid_texture) SDL_RenderTexture(m_renderer, m_grid_texture, nullptr, &m_bounds);
    if (m_bag_border_texture) SDL_RenderTexture(m_renderer, m_bag_border_texture, nullptr, &m_bounds);
}

size_t Grid::get_size()
{
    return m_size;
}

