#include <format>
#include <algorithm>
#include <cstdlib>
#include "grid.h"
#include "resource_manager.h"
#include "yoga/Yoga.h"

Grid::Grid(size_t size, SDL_Renderer* renderer) : View(ViewStyle{.justify_content = YGJustifyCenter, }, renderer) , m_size(size)
{
    // YGNodeStyleSetHeightPercent(m_layout_node, 100);
    // YGNodeStyleSetDisplay(m_layout_node, YGDisplayContents);
    YGNodeStyleSetWidthPercent(m_layout_node, 100);
    YGNodeStyleSetFlexShrink(m_layout_node, 1.0f);
    // YGNodeStyleSetFlexGrow(m_layout_node, 1.0f);
    // YGNodeStyleSetFlex(m_layout_node, 1.0f);
    YGNodeStyleSetAspectRatio(m_layout_node, 1.0f);

    m_puzzle = Puzzle::generate_puzzle(m_size);

    m_solved_label = new Label({ .align_self = YGAlignCenter }, renderer, "Well Done!", 120, {219, 10, 91, 255});
    insert_child(m_solved_label);
    m_solved_label->hide();
}

void Grid::on_resize()
{
    m_cell_size = calc_cell_size();
    m_grid_size = calc_grid_size();
    set_textures();
    m_solved_label->set_point_size(static_cast<int>(m_bounds.w)/6);
}

void Grid::on_update()
{   
        switch (m_extra_input_state) 
        {
            case flipping_in_bag_cells:
                if (m_puzzle->can_remove_from_bag(m_hovered_cell)) {
                    m_puzzle->remove_from_bag(m_hovered_cell);
                    m_hovered_cell_color = SDL_Color{255, 255, 255, 150};
                    set_bag_border_texture();
                    set_text_texture();
                }
                break;
            case flipping_out_of_bag_cells:
                if (m_puzzle->can_put_back_in_bag(m_hovered_cell)) {
                    m_puzzle->put_back_in_bag(m_hovered_cell);
                    m_hovered_cell_color = SDL_Color{80, 80, 80, 100};
                    set_bag_border_texture();
                    set_text_texture();
                }
                break;
            case idle:
                break;
        }

        switch (m_hover_state)
        {
            case hover_moved:
                {
                    bool in_bag = m_puzzle->is_in_bag(m_hovered_cell);
                    bool can_flip = m_puzzle->can_remove_from_bag(m_hovered_cell) || m_puzzle->can_put_back_in_bag(m_hovered_cell);
                    if (can_flip) 
                    {
                        m_hovered_cell_color = in_bag? SDL_Color{80, 80, 80, 100} : SDL_Color{255, 255, 255, 150};
                    } 
                    else 
                    {
                        m_hovered_cell_color = {200, 0, 0, 50};
                        m_extra_input_state = idle;
                    }
                    m_hover_state = hover_stable;
                }
                break;
            case hover_stable:
                break;
        }

    if (m_puzzle->is_solved()) {
        m_enabled = false;
        m_is_cell_hovered = false;
        m_extra_input_state = idle;
        m_solved_label->show();
    }
}

void Grid::on_enter(InputState* input_state)
{
}

void Grid::on_leave(InputState* input_state)
{
    m_is_cell_hovered = false;
}

bool Grid::point_to_cell_pos(const SDL_FPoint& point, CellPosition* pos) {
    SDL_FRect content_rect = {
        .x = m_bounds.x + m_padding,
        .y = m_bounds.y + m_padding,
        .w = m_bounds.w - 2 * m_padding,
        .h = m_bounds.h - 2 * m_padding
    };
    
    if (SDL_PointInRectFloat(&point, &content_rect)) {
        *pos = {
            static_cast<CellIndexType>(SDL_clamp((point.y - content_rect.y) / (m_cell_size + m_line_width), 0, m_size - 1)),
            static_cast<CellIndexType>(SDL_clamp((point.x - content_rect.x) / (m_cell_size + m_line_width), 0, m_size - 1))
        };
        return true;
    }

    return false;
}

void Grid::on_mouse_down(InputState* input_state)
{
    if (point_to_cell_pos(input_state->pointer_pos, &m_hovered_cell)) {
        if (m_puzzle->is_in_bag(m_hovered_cell)) 
        {
            if (m_puzzle->can_remove_from_bag(m_hovered_cell)) {
                m_extra_input_state = flipping_in_bag_cells;
            }
        }
        else
        {
            if ( m_puzzle->can_put_back_in_bag(m_hovered_cell)) {
                m_extra_input_state = flipping_out_of_bag_cells;
            }
        }
    }
}

void Grid::on_mouse_up(InputState* input_state)
{
    m_extra_input_state = idle;
}

void Grid::on_mouse_move(InputState* input_state)
{
    m_is_cell_hovered = point_to_cell_pos(input_state->pointer_pos, &m_hovered_cell);
    if (m_is_cell_hovered) {
        m_hover_state = hover_moved;
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
    int text_size = static_cast<int>(m_cell_size/2);
    int superscript_text_size = static_cast<int>(m_cell_size/5);
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
        superscript = TTF_RenderText_Blended(font_manager.get_font_for_point_size(superscript_text_size), std::format("+{}", current_score).c_str(), 0, {255, 0, 0, 255});
    }
    else if (current_score < 0)
    {
        superscript = TTF_RenderText_Blended(font_manager.get_font_for_point_size(superscript_text_size), std::format("{}", current_score).c_str(), 0, {255, 0, 0, 255});
    }
    else
    {
        superscript = TTF_RenderGlyph_Blended(font_manager.get_icon_font_for_point_size(superscript_text_size), 0xea10, {0, 255, 0, 255});
    }
    
    dst_rect.x += dst_rect.w + std::min(0, (int)((m_cell_size - text->w)/3 - (superscript->w)));
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

    if (m_is_cell_hovered)
    {
        fill_cell(m_hovered_cell, m_hovered_cell_color);
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
    m_enabled = true;
    m_solved_label->hide();
}

void Grid::reset_puzzle()
{
    m_puzzle->restart();
    set_textures();
    m_enabled = true;
    m_solved_label->hide();
}
