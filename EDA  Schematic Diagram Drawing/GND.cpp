#include "GND.h"
GND::GND(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size) :
	Component(parent, type, center_point, rotate_degree, base_points_num, memDC, delta, Size) {
	//GND
	offset.resize(base_points_num);
	offset[0] = wxPoint(-17.5, -7);
	offset[1] = wxPoint(17.5, -7);
	offset[2] = wxPoint(-12.5, 0);
	offset[3] = wxPoint(12.5, 0);
	offset[4] = wxPoint(-7.5, 7);
	offset[5] = wxPoint(7.5, 7);
	offset[6] = wxPoint(-2.5, 14);
	offset[7] = wxPoint(2.5, 14);
	offset[8] = wxPoint(0, -7);
	offset[9] = wxPoint(0, -22);
	offset[10] = wxPoint(-10.5, 28);

	for (int i = 0; i < getBasePointsNums(); i++) {
		getCoordinate()[i] += center_point + offset[i];
	}
	Pins.push_back(new Pin(getCoordinate()[9], false, this));
	setRange();
}

void GND::draw() {
	memDC.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	memDC.DrawLine(getCoordinate()[0], getCoordinate()[1]);
	memDC.DrawLine(getCoordinate()[2], getCoordinate()[3]);
	memDC.DrawLine(getCoordinate()[4], getCoordinate()[5]);
	memDC.DrawLine(getCoordinate()[6], getCoordinate()[7]);
	memDC.DrawLine(getCoordinate()[8], getCoordinate()[9]);
	memDC.DrawText("GND", getCoordinate()[10]);
}

int GND::getDiam(){
	return diam;
}

void GND::setDiam(int new_diam){
	diam = new_diam;
}
