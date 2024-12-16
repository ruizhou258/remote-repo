#include "ComponentDragTarget.h"
#include "DrawingCanvas.h"

ComponentDragTarget:: ComponentDragTarget(DrawingCanvas* canvas) : canvas(canvas) {}

bool ComponentDragTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    canvas->AddComponent(data, wxPoint(x, y));
    return true;
}
