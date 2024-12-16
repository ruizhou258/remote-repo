#include "Resistor.h"
Resistor::Resistor(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size) :
	Component(parent, type, center_point, rotate_degree, base_points_num, memDC, delta, Size) {
	
	offset.resize(base_points_num);
	//³õÊ¼»¯Æ«ÒÆÁ¿
	offset[0] = wxPoint(-14.0, -26.5);
	offset[1] = wxPoint(14.0, -26.5);
	offset[2] = wxPoint(-14.0, 26.5);
	offset[3] = wxPoint(14.0, 26.5);
	offset[4] = wxPoint(-14.0, -26.5);
	offset[5] = wxPoint(-14.0, 26.5);
	offset[6] = wxPoint(14.0, -26.5);
	offset[7] = wxPoint(14.0, 26.5);
	offset[8] = wxPoint(0, -26.5);
	offset[9] = wxPoint(0, -51.5);
	offset[10] = wxPoint(0, 26.5);
	offset[11] = wxPoint(0, 51.5);
	offset[12] = wxPoint(24.0, -6.5);
	
	for (int i = 0; i < getBasePointsNums(); i++) {
		getCoordinate()[i] += center_point + offset[i];
	}

	Pins.push_back(new Pin(getCoordinate()[9], false, this));
	Pins.push_back(new Pin(getCoordinate()[11], false, this));
	setRange();
}

void Resistor::draw() {
	memDC.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	memDC.DrawLine(getCoordinate()[0], getCoordinate()[1]);
	memDC.DrawLine(getCoordinate()[2], getCoordinate()[3]);
	memDC.DrawLine(getCoordinate()[4], getCoordinate()[5]);
	memDC.DrawLine(getCoordinate()[6], getCoordinate()[7]);
	memDC.DrawLine(getCoordinate()[8], getCoordinate()[9]);
	memDC.DrawLine(getCoordinate()[10], getCoordinate()[11]);
	memDC.DrawText("R", getCoordinate()[12]);
}

int Resistor::getRvalue(){
	return rvalue;
}

void Resistor::setRvalue(int new_rvalue){
	rvalue = new_rvalue;
}
