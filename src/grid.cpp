#include <format>
#include <cstdlib>
#include "grid.h"
#include "resource_manager.h"
#include "yoga/Yoga.h"

Grid::Grid(size_t size, SDL_Renderer* renderer) : View(ViewStyle{ }, renderer) , m_size(size)
{
    // YGNodeStyleSetHeightPercent(m_layout_node, 100);
    // YGNodeStyleSetDisplay(m_layout_node, YGDisplayContents);
    YGNodeStyleSetWidthPercent(m_layout_node, 100);
    YGNodeStyleSetFlexShrink(m_layout_node, 1.0f);
    // YGNodeStyleSetFlexGrow(m_layout_node, 1.0f);
    // YGNodeStyleSetFlex(m_layout_node, 1.0f);
    YGNodeStyleSetAspectRatio(m_layout_node, 1.0f);

    m_puzzle = Puzzle::generate_puzzle(m_size);
}

void Grid::on_resize()
{
    m_cell_size = calc_cell_size();
    m_grid_size = calc_grid_size();
    set_textures();
}

void Grid::on_update()
{   
    m_should_highlight_square = false;
}

void Grid::on_enter(InputState& input_state)
{
}

void Grid::on_leave(InputState &input_state)
{
}

void Grid::on_mouse_down(InputState &input_state)
{
}

void Grid::on_mouse_up(InputState &input_state)
{
}

void Grid::on_mouse_move(InputState &input_state)
{
    SDL_FRect content_rect = {
        .x = m_bounds.x + m_padding,
        .y = m_bounds.y + m_padding,
        .w = m_bounds.w - 2 * m_padding,
        .h = m_bounds.h - 2 * m_padding
    };
    
    if (SDL_PointInRectFloat(&input_state.pointer_pos, &content_rect)) {
        CellPosition pos = {
            static_cast<CellIndexType>((SDL_clamp(input_state.pointer_pos.y, content_rect.y, content_rect.y + content_rect.h - 1) - content_rect.y) / (m_cell_size + m_line_width)),
            static_cast<CellIndexType>((SDL_clamp(input_state.pointer_pos.x, content_rect.x, content_rect.x + content_rect.w - 1) - content_rect.x) / (m_cell_size + m_line_width))
        };
        handle_input(pos, input_state);
    }
}

void Grid::handle_input(CellPosition pos, InputState& input_state)
{
    if (!input_state.mouse_is_down)
    {
        m_extra_input_state.mouse_clicked_in_bag = false;
        m_extra_input_state.mouse_clicked_out_of_bag = false;
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

        if (input_state.mouse_is_pressed_down_this_frame)
        {
            if (in_bag) 
            {
                m_extra_input_state.mouse_clicked_in_bag = true;
            }
            else
            {
                m_extra_input_state.mouse_clicked_out_of_bag = true;
            }

        }

        if (can_flip) 
        {
            if (in_bag)
            {
                m_highlight_square_color = {80, 80, 80, 100};
                if (input_state.mouse_is_down) {
                    if (m_extra_input_state.mouse_clicked_in_bag)
                    {
                        m_puzzle->remove_from_bag(pos);
                        set_bag_border_texture();
                        set_text_texture();
                    }
                }
            }
            else 
            {
                m_highlight_square_color = {255, 255, 255, 150};
                if (input_state.mouse_is_down) {
                    if (m_extra_input_state.mouse_clicked_out_of_bag)
                    {
                        m_puzzle->put_back_in_bag(pos);
                        set_bag_border_texture();
                        set_text_texture();
                    }
                }
            }
        } 
        else 
        {
            m_highlight_square_color = {200, 0, 0, 50};
            m_extra_input_state.mouse_clicked_in_bag = false;
            m_extra_input_state.mouse_clicked_out_of_bag = false;
        }
    }
}

void Grid::fill_cell(CellPosition pos, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    const SDL_FRect rect = get_frect_for_pos(pos, m_bounds.x, m_bounds.y);
    SDL_RenderFillRect(m_renderer, &rect);
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

void Grid::render_cell_target(CellPosition pos, int32_t target, SDL_Surface* text_surface) 
{
    float text_size = SDL_floorf(m_cell_size/2);
    SDL_Surface* text = TTF_RenderText_Blended(font_manager.get_font_for_point_size(text_size), std::to_string(target).c_str(), 0, {0,0,0,255});
    SDL_Rect dst_rect = get_rect_for_pos(pos);

    dst_rect.x +=  (m_cell_size - text->w) / 2;
    dst_rect.y +=  (m_cell_size - text->h) / 2;
    dst_rect.w = text->w;
    dst_rect.h = text->h;
    SDL_BlitSurface(text, nullptr, text_surface, &dst_rect);

    const int32_t current_score = m_puzzle->get_num_cells_visible_from(pos) - target;
    SDL_Surface* superscript = nullptr;
    if (current_score > 0)
    {
        superscript = TTF_RenderText_Blended(font_manager.get_font_for_point_size(text_size/2), std::format("+{}", current_score).c_str(), 0, {255, 0, 0, 255});
    }
    else if (current_score < 0)
    {
        superscript = TTF_RenderText_Blended(font_manager.get_font_for_point_size(text_size/2), std::format("{}", current_score).c_str(), 0, {255, 0, 0, 255});
    }
    else
    {
        superscript = TTF_RenderGlyph_Blended(font_manager.get_icon_font_for_point_size(text_size/2), 0xea10, {0, 255, 0, 255});
    }

    dst_rect.x += dst_rect.w - 3;
    dst_rect.y -= superscript->h/4;
    dst_rect.w = superscript->w;
    dst_rect.h = superscript->h;
    SDL_BlitSurface(superscript, nullptr, text_surface, &dst_rect);
}

void Grid::set_text_texture() {

    if (m_grid_size <= 0) {
        if (m_text_texture) {
            SDL_DestroyTexture(m_text_texture);
        }
        m_text_texture = nullptr;
        return;
    }

    SDL_Surface* text_surface = SDL_CreateSurface(m_bounds.w + 1, m_bounds.h + 1, SDL_PIXELFORMAT_ARGB8888);

    for (auto &[pos, target] : m_puzzle->get_targets())
    {
        render_cell_target(pos, target, text_surface);
    }

    if (m_text_texture) {
        SDL_DestroyTexture(m_text_texture);
    }
    m_text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
    SDL_DestroySurface(text_surface);
}

void Grid::set_grid_texture() {

    if (m_grid_size <= 0) {
        if (m_grid_texture) {
            SDL_DestroyTexture(m_grid_texture);
        }
        m_grid_texture = nullptr;
        return;
    }

    plutovg_surface_t* surface = plutovg_surface_create(m_bounds.w + 1, m_bounds.h + 1);
    plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

    float dashes[] = {m_line_width, m_cell_size};
    plutovg_canvas_set_dash_array(canvas, dashes, 2);
    plutovg_canvas_set_line_width(canvas, m_grid_size);
    plutovg_canvas_move_to(canvas, 0, m_grid_size/2);
    plutovg_canvas_line_to(canvas, m_grid_size, m_grid_size/2);
    plutovg_canvas_move_to(canvas, m_grid_size/2, 0);
    plutovg_canvas_line_to(canvas, m_grid_size/2, m_grid_size);

    plutovg_canvas_set_rgb(canvas, 0.2049, 0.2827, 0.3809);
    plutovg_canvas_translate(canvas, m_padding, m_padding);
    plutovg_canvas_stroke(canvas);
    // plutovg_surface_write_to_png(surface, "grid.png");

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
    m_grid_texture = SDL_CreateTextureFromSurface(m_renderer, sdl_surface);

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

void Grid::set_bag_border_texture() {

    if (m_grid_size <= 0) {
        if (m_bag_border_texture) {
            SDL_DestroyTexture(m_bag_border_texture);
        }
        m_bag_border_texture = nullptr;
        return;
    }

    std::vector<CellPosition> bag_border_points; // for drawing the loop around cells
    m_puzzle->trace_bag_border_points(bag_border_points);

    plutovg_surface_t* surface = plutovg_surface_create(m_bounds.w + 1, m_bounds.h + 1);
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
    plutovg_canvas_translate(canvas, m_padding + m_line_width/2, m_padding + m_line_width/2);
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
    m_bag_border_texture = SDL_CreateTextureFromSurface(m_renderer, sdl_surface);

    SDL_DestroySurface(sdl_surface);
    plutovg_canvas_destroy(canvas);
    plutovg_surface_destroy(surface);
}

void Grid::on_render()
{
    for (CellIndexType i = 0; i < m_size; i++)
    {
        for (CellIndexType j = 0; j < m_size; j++)
        {
            if (m_puzzle->is_in_bag({i, j}))
            {
                fill_cell({i, j}, {255,255,255,255});
            }
            else
            {
                // fill_cell({i, j}, {130,130,130,255});
                fill_cell({i, j}, {225, 238, 243,255});
            }
        }
    }
    
    if (m_should_highlight_square)
    {
        fill_cell(m_highlight_square, m_highlight_square_color);
    }
    
    if (m_grid_texture) SDL_RenderTexture(m_renderer, m_grid_texture, nullptr, &m_bounds);
    if (m_bag_border_texture) SDL_RenderTexture(m_renderer, m_bag_border_texture, nullptr, &m_bounds);
    if (m_text_texture) SDL_RenderTexture(m_renderer, m_text_texture, nullptr, &m_bounds);
}

size_t Grid::get_size()
{
    return m_size;
}

void Grid::new_puzzle()
{
    m_puzzle = Puzzle::generate_puzzle(m_size);
    set_textures();
}

void Grid::reset_puzzle()
{
    m_puzzle->restart();
    set_textures();
}
