#pragma once
#include "Component.h"
class Inductor : public Component {
public:
	Inductor(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size);
	void draw();
	double getInductance();
	void setInductance(double new_inductance);
private:
	double inductance = 0.22; //H
};

