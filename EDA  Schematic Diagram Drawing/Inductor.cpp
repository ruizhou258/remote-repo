#include "Inductor.h"
Inductor::Inductor(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size) :
	Component(parent, type, center_point, rotate_degree, base_points_num, memDC, delta, Size) {
	//µç¸Ð
	offset.resize(base_points_num);
	offset[0] = wxPoint(-40, 0);
	offset[1] = wxPoint(-30, 0);
	offset[2] = wxPoint(-20, 0);
	offset[3] = wxPoint(-10, 0);
	offset[4] = wxPoint(0, 0);
	offset[5] = wxPoint(10, 0);
	offset[6] = wxPoint(20, 0);
	offset[7] = wxPoint(30, 0);
	offset[8] = wxPoint(40, 0);
	offset[9] = wxPoint(-5, -20);
	
	for (int i = 0; i < getBasePointsNums(); i++) {
		getCoordinate()[i] += center_point + offset[i];
	}
	center_point = getCoordinate()[4];
	Pins.push_back(new Pin(getCoordinate()[0], false, this));
	Pins.push_back(new Pin(getCoordinate()[8], false, this));
	setRange();
}

void Inductor::draw() {
	for (int n = 0; n < 8; n = n + 2)
	{
		memDC.DrawArc(getCoordinate()[n], getCoordinate()[n + 2], getCoordinate()[n + 1]);
	}
	memDC.SetPen(*wxWHITE_PEN);
	memDC.DrawLine(getCoordinate()[0], getCoordinate()[8]);
	memDC.SetPen(*wxBLACK_PEN);
	memDC.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	memDC.DrawText(wxT("L"), getCoordinate()[9]);
}

double Inductor::getInductance(){
	return inductance;
}

void Inductor::setInductance(double new_inductance){
	inductance = new_inductance;
}
