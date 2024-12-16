#pragma once
#include "Component.h"
class Resistor : public Component {
public:
	Resistor(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size);
	void draw();
	int getRvalue();
	void setRvalue(int new_rvalue);
private:
	int rvalue = 10; //¦¸
};
