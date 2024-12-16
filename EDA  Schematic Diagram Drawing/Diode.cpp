#include "Diode.h"
Diode::Diode(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size) :
	Component(parent, type, center_point, rotate_degree, base_points_num, memDC, delta, Size) {
	//¶þ¼«¹Ü
	offset.resize(base_points_num);
	offset[0] = wxPoint(0, -50);
	offset[1] = wxPoint(27.169, 0);
	offset[2] = wxPoint(-27.169, 0);
	offset[3] = wxPoint(0, -95);
	offset[4] = wxPoint(0, 25);
	offset[5] = wxPoint(27.169, -50);
	offset[6] = wxPoint(-27.169, -50);
	offset[7] = wxPoint(27.169, -30);

	for (int i = 0; i < getBasePointsNums(); i++) {
		getCoordinate()[i] += center_point + offset[i];
	}
	Pins.push_back(new Pin(getCoordinate()[3], false, this));
	Pins.push_back(new Pin(getCoordinate()[4], false, this));
	setRange();
}

void Diode::draw() {
	wxPoint triangle[3];
	triangle[0] = getCoordinate()[0];
	triangle[1] = getCoordinate()[1];
	triangle[2] = getCoordinate()[2];
	memDC.DrawPolygon(3, triangle);
	memDC.DrawLine(getCoordinate()[3], getCoordinate()[4]);
	memDC.DrawLine(getCoordinate()[5], getCoordinate()[6]);
	memDC.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	memDC.DrawText(wxT("D"), getCoordinate()[7]);
}

int Diode::getElectricCur(){
	return current;
}

void Diode::setElectricCur(int new_cur){
	current = new_cur;
}
