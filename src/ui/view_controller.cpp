#include "view_controller.h"
#include "../events/event.h"

const int min_int = -1000;

void dfs_insert(std::map<int, std::vector<View *>> &zmap, View *root, int z_index)
{
    if (root->get_z_index() > z_index) {
        z_index = root->get_z_index();
        std::vector<View *> &render_queue = zmap[z_index];
        render_queue.push_back(root);
    }

    size_t child_count = root->get_child_count();
    for (size_t i = 0; i < child_count; ++i)
    {
        dfs_insert(zmap, root->get_child(i), z_index);
    }
}

ViewController::ViewController(SDL_Renderer* renderer, View *root) : m_renderer(renderer), m_ui_root(root)
{
    dfs_insert(m_render_queue, root, min_int);
}

ViewController::~ViewController()
{
    delete m_ui_root;
}

void ViewController::update()
{
    update_input_state();
    dispatch_events();
    if (m_ui_root->is_dirty() || m_resized)
    {
        m_ui_root->calc_layout(m_w, m_h);
        m_resized = false;
    }
    m_ui_root->update();
}

void render_dfs(View* root, int z_index) {
    if (root->is_showing()) {
        root->render();
        size_t child_count = root->get_child_count();
        for (size_t i = 0; i < child_count; ++i)
        {
            View* child = root->get_child(i);
            if (child->get_z_index() <= z_index) {
                render_dfs(child, z_index);
            }
        }
    }
}

void ViewController::render()
{
    for (const auto &[z_index, render_queue] : m_render_queue)
    {
        for (auto &view : render_queue)
        {
            render_dfs(view, z_index);
        }
    }
}

void ViewController::resize(const int w, const int h)
{
    m_w = w;
    m_h = h;
    m_resized = true;
}

View* ViewController::get_view_under_pointer(View* root, int z_index) {

    size_t child_count = root->get_child_count();
    for (size_t i = 0; i < child_count; ++i)
    {
        View* child = root->get_child(i);
        if (child->get_z_index() <= z_index) {
            if (child->is_showing() && child->is_enabled() && child->contains_point(m_input_state.pointer_pos))
            {
                return get_view_under_pointer(child, z_index);
            }
        }
    }

    return root;
}

View* ViewController::get_view_under_pointer() {
    for (auto z_iter = m_render_queue.rbegin(); z_iter != m_render_queue.rend(); ++z_iter)
    {
        auto &[z_index, render_queue] = *z_iter;
        for (auto view_iter = render_queue.rbegin(); view_iter != render_queue.rend(); ++view_iter)
        {
            View* view = *view_iter;
            if (view->is_showing() && view->is_enabled() && view->contains_point(m_input_state.pointer_pos))
            {
                return get_view_under_pointer(view, z_index);
            }
        }
    }

    return nullptr;
}

void ViewController::update_current_entered_stack(View* current_target) {
    while (!m_current_entered_stack.empty())
    {   
        View* top_view = m_current_entered_stack.back();
        if (!top_view->contains_point(m_input_state.pointer_pos))
        {
            Event event(Event::leave, &m_input_state);
            top_view->handle_event(event);
            m_current_entered_stack.pop_back();
        }
        else {
            break;
        }
    }

    if (current_target) {
        while (!m_current_entered_stack.empty())
        {   
            View* top_view = m_current_entered_stack.back();
            if (!top_view->is_ancestor_of(current_target))
            {
                Event event(Event::leave, &m_input_state);
                top_view->handle_event(event);
                m_current_entered_stack.pop_back();
            }
            else {
                break;
            }
        }

        while (m_current_entered_stack.empty() || m_current_entered_stack.back() != current_target) {
            Event event(Event::enter, &m_input_state);
            if (current_target->handle_event(event)) {
                m_current_entered_stack.push_back(current_target);
                break;
            }
            current_target = current_target->parent_view();
            if (!current_target) {
                break;
            }
        }
    }
}

#define POINTS_EQUAL(p1,p2) ((p1.x == p2.x) && (p1.y == p2.y))

void ViewController::dispatch_events_to_target(View* target) {

    if (m_input_state.mouse_is_pressed_down_this_frame) {
        View* mouse_down_target = target;
        Event event(Event::mouse_down, &m_input_state);
        while (!mouse_down_target->handle_event(event)) {
            mouse_down_target = mouse_down_target->parent_view();
            if (!mouse_down_target) {
                break;
            }
        }
        if (mouse_down_target) {
            m_captured = true;
            m_capturing_view = mouse_down_target;
        }
    }

    if (m_input_state.mouse_is_released_up_this_frame) {
        View* mouse_up_target = target;
        Event event(Event::mouse_up, &m_input_state);
        while(!mouse_up_target->handle_event(event)) {
            mouse_up_target = mouse_up_target->parent_view();
            if (!mouse_up_target) {
                break;
            }
        }
        m_captured = false;
        m_capturing_view = nullptr;
    }

    if (!POINTS_EQUAL(m_input_state.pointer_pos, m_input_state.prev_pointer_pos)) {
        View* move_target = target;
        Event event(Event::mouse_move, &m_input_state);
        while(!move_target->handle_event(event)) {
            move_target = move_target->parent_view();
            if (!move_target) {
                break;
            }
        }
    }
}

void ViewController::dispatch_events()
{
    View* target = nullptr;
    if (m_captured && m_capturing_view->is_enabled()) {
        target = m_capturing_view;
    }
    else {
        target = get_view_under_pointer();
        update_current_entered_stack(target);
    }

    if (target) {
        dispatch_events_to_target(target);
    }
}

void ViewController::update_input_state()
{
    const InputState prev_input_state = m_input_state;
    InputState& current_input_state = m_input_state;
    SDL_FPoint pointer_window_pos;
    SDL_MouseButtonFlags mouse_state = SDL_GetMouseState(&pointer_window_pos.x, &pointer_window_pos.y);
    // convert from window cords to pixle cords
    SDL_RenderCoordinatesFromWindow(m_renderer, pointer_window_pos.x, pointer_window_pos.y, &current_input_state.pointer_pos.x, &current_input_state.pointer_pos.y);
    current_input_state.prev_pointer_pos = prev_input_state.pointer_pos;
    current_input_state.mouse_is_down = mouse_state & SDL_BUTTON_LMASK;
    current_input_state.mouse_is_pressed_down_this_frame = !prev_input_state.mouse_is_down && current_input_state.mouse_is_down;
    current_input_state.mouse_is_released_up_this_frame = prev_input_state.mouse_is_down && !current_input_state.mouse_is_down;
}