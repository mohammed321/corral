#pragma once

class Event {

public:

    enum Type {
        None = 0,
        KeyBoardEvent,
        MouseEvent,
    };

    Event(Type type);

    inline Type get_type() {
        return m_type;
    }

private:

    Type m_type;

};
