#pragma once
#include "wx_menu.h"

class Wire;

class Pin
{
public:
    Pin(wxPoint position, bool is_connected, Component* parent_component)
        : m_position(position), m_is_connected(is_connected), m_parent_component(parent_component) {}
    wxPoint m_position;
    bool m_is_connected;
    Component* m_parent_component; // 所属元器件
    Wire* m_connected_wire = NULL; // 记录连接的导线
};

