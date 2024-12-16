#pragma once
#include "wx_menu.h"
#include <wx/dnd.h>

class ComponentLibrary : public wxPanel {
public:
    ComponentLibrary(wxWindow* parent)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 600)) {
    }
    void CreateComponentLibrary(wxPanel* panel);
};
