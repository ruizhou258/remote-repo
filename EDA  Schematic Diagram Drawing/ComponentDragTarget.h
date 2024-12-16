#pragma once
#include <wx/dnd.h>

class DrawingCanvas;
class ComponentDragTarget : public wxTextDropTarget {
public:
    explicit ComponentDragTarget(DrawingCanvas* canvas);

    bool OnDropText(wxCoord x, wxCoord y, const wxString& data) override;

private:
    DrawingCanvas* canvas;
};
