#include "ComponentLibrary.h"

void ComponentLibrary::CreateComponentLibrary(wxPanel* panel) {
    wxBoxSizer* librarySizer = new wxBoxSizer(wxVERTICAL);

    vector<pair<wxString, wxBitmap>> components;
    wxImage image;
    image.LoadFile("ComponentPictures/resistor_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("����", wxBitmap(image)));

    image.LoadFile("ComponentPictures/capacitor_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("����", wxBitmap(image)));

    image.LoadFile("ComponentPictures/diode_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("������", wxBitmap(image)));

    image.LoadFile("ComponentPictures/vcc_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("��Դ", wxBitmap(image)));

    image.LoadFile("ComponentPictures/inductor_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("���", wxBitmap(image)));

    image.LoadFile("ComponentPictures/gnd_icon.png", wxBITMAP_TYPE_PNG);
    image.Rescale(85, 70);
    components.push_back(make_pair("GND", wxBitmap(image)));


    for (const auto& comp : components) {
        wxBoxSizer* compSizer = new wxBoxSizer(wxHORIZONTAL);

        // ͼ����ı�
        wxStaticBitmap* icon = new wxStaticBitmap(panel, wxID_ANY, comp.second);
        wxStaticText* name = new wxStaticText(panel, wxID_ANY, comp.first);

        compSizer->Add(name, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        compSizer->Add(icon, 2, wxALL, 5);

        // �����ק֧��
        icon->Bind(wxEVT_LEFT_DOWN, [panel, comp](wxMouseEvent&) { //lambda���ʽ
            wxDropSource dragSource(panel);
            wxTextDataObject data(comp.first);
            dragSource.SetData(data);
            dragSource.DoDragDrop();
        });

        librarySizer->Add(compSizer, 0, wxEXPAND | wxALL, 5);
    }

    panel->SetSizer(librarySizer);
}
