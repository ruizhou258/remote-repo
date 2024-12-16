#pragma once
#include "wx_menu.h"
class Wire;

class WireToWireConnection
{
public:
	WireToWireConnection() {
		connected_point = wxPoint(0, 0);
		connected_wire = NULL;
	}
	WireToWireConnection(wxPoint& connected_point, Wire* connected_wire)
		:connected_point(connected_point), connected_wire(connected_wire){}
	wxPoint connected_point;
	Wire* connected_wire;
};

