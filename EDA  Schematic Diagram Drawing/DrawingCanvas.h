#pragma once
#include "wx_menu.h"
#include <wx/popupwin.h>
#include <set>
#define ID_CHANGE_PROPERTIES 7777

using namespace std;
class Component;
class Wire;

class DrawingCanvas :public wxPanel {
public:
    DrawingCanvas(wxWindow* parent, wxMemoryDC& memDC, wxPoint& delta, wxSize Size);

    void OnPaint(wxPaintEvent& event);

    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);

    void OnInitDrawLines(wxCommandEvent& event);
    void OnEndDrawLines(wxCommandEvent& event);
    void UndoLines(wxMouseEvent& event);

    void Display_Component_info(wxMouseEvent& event);
    void DisplayChangeMenu(wxMouseEvent& event);
    void DisplayChangeDialog(wxCommandEvent& event);
    void OnChangeProperties();

    void OnKeyPress(wxKeyEvent& event);
    void RotateComp();
    void DeleteComp();
    void DeleteWire();

    void CheckE(wxCommandEvent& event);
    void CheckElectricalConnections(const std::vector<Component*>& components, std::vector<Wire*>& wires);
    bool IsConnected(const wxPoint& point1, const wxPoint& point2, int threshold = 5); 
    void HighlightUnconnected();

    void SnapToGrid();
    void AddComponent(const wxString& componentType, wxPoint dropPosition);
    void DrawConnect();
    
    void IsInRangeComp(wxPoint& position);
    void IsInRangePin(wxPoint& position);
    void IsInRangeWire(wxPoint& position);
    void TraceComp();

    void CreateWireToPinConnection();   //导线与引脚
    //void CreateWireToWireConnection();
    //void UpdateWireNetwork(Wire* wire, set<Wire*>& visited);
    //void UpdateConnectedWires(Component* component);

    wxWindow* m_parent;
    wxBitmap circuitBitmap = wxBitmap(800, 700);
    wxMemoryDC& memDC;
    wxPoint m_startPoint;
    wxPoint m_endPoint;
    wxPoint& delta;
    wxSize Size;
    int m_grid_size;
    vector<wxPoint>unconnected_pins;
    wxDialog* propertyDialog;
    wxTextCtrl* textCtrl;

    bool clearBitmap = false;
    bool check_elec = false;
    bool draw_line = false; //是否为绘制导线模式

    //保存所有元器件的数组（多态的体现）
    vector<Component*> cur_comp;
    vector<Wire*> wires;
    Component* selected_component;
    Wire* selected_wire;

    union data {
        int INT_data;
        double DOUBLE_data;
    };
    struct ComponentInfo {
        std::string type;
        wxPoint center_p[100] = {};
        int rotate_degree;
        data d;
    };
    //存储各个元器件信息的数组
    vector<ComponentInfo> EC;
};

