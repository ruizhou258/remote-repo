#include "ComponentLibrary.h"

void ComponentLibrary::CreateComponentLibrary(wxPanel* panel) {
    wxBoxSizer* librarySizer = new wxBoxSizer(wxVERTICAL);

    vector<pair<wxString, wxBitmap>> components;
    wxImage image;
    image.LoadFile("ComponentPictures/resistor_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("电阻", wxBitmap(image)));

    image.LoadFile("ComponentPictures/capacitor_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("电容", wxBitmap(image)));

    image.LoadFile("ComponentPictures/diode_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("二极管", wxBitmap(image)));

    image.LoadFile("ComponentPictures/vcc_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("电源", wxBitmap(image)));

    image.LoadFile("ComponentPictures/inductor_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("电感", wxBitmap(image)));

    image.LoadFile("ComponentPictures/gnd_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("GND", wxBitmap(image)));


    for (const auto& comp : components) {
        wxBoxSizer* compSizer = new wxBoxSizer(wxHORIZONTAL);

        // 图标和文本
        wxStaticBitmap* icon = new wxStaticBitmap(panel, wxID_ANY, comp.second);
        wxStaticText* name = new wxStaticText(panel, wxID_ANY, comp.first);

        compSizer->Add(name, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        compSizer->Add(icon, 2, wxALL, 5);

        // 添加拖拽支持
        icon->Bind(wxEVT_LEFT_DOWN, [panel, comp](wxMouseEvent&) { //lambda表达式
            wxDropSource dragSource(panel);
            wxTextDataObject data(comp.first);
            dragSource.SetData(data);
            dragSource.DoDragDrop();
        });

        librarySizer->Add(compSizer, 0, wxEXPAND | wxALL, 5);
    }

    panel->SetSizer(librarySizer);
}
