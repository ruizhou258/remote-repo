#include "Capacitor.h"
Capacitor::Capacitor(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size) :
	Component(parent, type, center_point, rotate_degree, base_points_num, memDC, delta, Size) {
	offset.resize(base_points_num);
	offset[0] = wxPoint(-20, 7.5);
	offset[1] = wxPoint(20, 7.5);
	offset[2] = wxPoint(-20, -7.5);
	offset[3] = wxPoint(20, -7.5);
	offset[4] = wxPoint(0, 7.5);
	offset[5] = wxPoint(0, 27.5);
	offset[6] = wxPoint(0, -7.5);
	offset[7] = wxPoint(0, -27.5);
	offset[8] = wxPoint(15, -27.5);
	//µÁ»›
	for (int i = 0; i < getBasePointsNums(); i++) {
		getCoordinate()[i] += center_point + offset[i];
	}
	Pins.push_back(new Pin(getCoordinate()[5], false, this));
	Pins.push_back(new Pin(getCoordinate()[7], false, this));
	setRange();
}

void Capacitor::draw() {
	memDC.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	memDC.DrawLine(getCoordinate()[0], getCoordinate()[1]);
	memDC.DrawLine(getCoordinate()[2], getCoordinate()[3]);
	memDC.DrawLine(getCoordinate()[4], getCoordinate()[5]);
	memDC.DrawLine(getCoordinate()[6], getCoordinate()[7]);
	memDC.DrawText("C", getCoordinate()[8]);
}

int Capacitor::getCapacity() {
	return capacity;
}

void Capacitor::setCapacity(int new_capacity) {
	capacity = new_capacity;
}