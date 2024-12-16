#pragma once
#include "Component.h"
class GND : public Component {
public:
	GND(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size);
	void draw();
	int getDiam();
	void setDiam(int new_diam);
private:
	int diam = 20; //mm
};