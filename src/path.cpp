#include "path.h"
#include "plutovg/plutovg.h"



SDL_Texture* render_path(SDL_Renderer *renderer)
{
    const int width = 1000;
    const int height = 1000;

    const float center_x = width / 2.f;
    const float center_y = height / 2.f;
    const float face_radius = 70;
    const float mouth_radius = 50;
    const float eye_radius = 10;
    const float eye_offset_x = 25;
    const float eye_offset_y = 20;
    const float eye_x = center_x - eye_offset_x;
    const float eye_y = center_y - eye_offset_y;

    plutovg_surface_t* surface = plutovg_surface_create(width, height);
    plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

    plutovg_canvas_save(canvas);
    plutovg_canvas_arc(canvas, center_x, center_y, face_radius, 0, PLUTOVG_TWO_PI, 0);
    plutovg_canvas_set_rgb(canvas, 1, 1, 0);
    plutovg_canvas_fill_preserve(canvas);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_set_line_width(canvas, 5);
    plutovg_canvas_stroke(canvas);
    plutovg_canvas_restore(canvas);

    plutovg_canvas_save(canvas);
    plutovg_canvas_arc(canvas, eye_x, eye_y, eye_radius, 0, PLUTOVG_TWO_PI, 0);
    plutovg_canvas_arc(canvas, center_x + eye_offset_x, eye_y, eye_radius, 0, PLUTOVG_TWO_PI, 0);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_fill(canvas);
    plutovg_canvas_restore(canvas);

    plutovg_canvas_save(canvas);
    plutovg_canvas_arc(canvas, center_x, center_y, mouth_radius, 0, PLUTOVG_PI, 0);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_set_line_width(canvas, 5);
    plutovg_canvas_stroke(canvas);
    plutovg_canvas_restore(canvas);

    SDL_Surface* sdl_surface = SDL_CreateSurfaceFrom(
        plutovg_surface_get_width(surface), 
        plutovg_surface_get_height(surface),
        SDL_PIXELFORMAT_ARGB8888,
        plutovg_surface_get_data(surface),
        plutovg_surface_get_stride(surface)
    );

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, sdl_surface);

    SDL_DestroySurface(sdl_surface);
    plutovg_canvas_destroy(canvas);
    plutovg_surface_destroy(surface);

    return tex;
}