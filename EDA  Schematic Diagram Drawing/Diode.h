#pragma once
#include "Component.h"
class Diode : public Component {
public:
	Diode(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size);
	void draw();
	int getElectricCur();
	void setElectricCur(int new_cur);
private:
	int current = 400; //mA
};

