#pragma once
#include "Component.h"
class VCC : public Component {
public:
	VCC(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size);
	void draw();
	int getVoltage();
	void setVoltage(int new_voltage);
private:
	int voltage = 15; //V
};