#pragma once

#include "../input.h"

class Event {

public:

    InputState* m_input_state = nullptr;

    enum Type {
        None = 0,
        enter,
        leave,
        mouse_down,
        mouse_up,
        mouse_move,
    };

    Event(Type type, InputState* input_state);

    inline Type get_type() const {
        return m_type;
    }

private:

    Type m_type;

};
