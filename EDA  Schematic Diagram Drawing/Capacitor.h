#pragma once
#include "Component.h"
class Capacitor : public Component {
public:
	Capacitor(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size);
	void draw();
	int getCapacity();
	void setCapacity(int new_capacity);
private:
	int capacity = 1000; //uF
};

