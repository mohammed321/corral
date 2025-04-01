#pragma once

#include <map>
#include <vector>

#include "view.h"
#include "../input.h"

class ViewController 
{
public:
    ViewController(View* root);
    ~ViewController();

    void update();
    void render();
    void resize(const int w, const int h);
    
private:
    View* m_ui_root;
    int m_w;
    int m_h;
    bool m_resized = false;
    bool m_captured = false;
    View* m_capturing_view = nullptr;
    std::map<int, std::vector<View*>> m_render_queue;
    
    InputState m_input_state;
    std::vector<View*> m_current_entered_stack;
    
    void update_input_state();
    View* get_view_under_pointer();
    void update_current_entered_stack(View* current_target);
    void dispatch_events_to_target(View *target);
    void dispatch_events();

    View* get_view_under_pointer(View *root, int z_index);
};