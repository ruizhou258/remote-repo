#include "DrawingCanvas.h"
#include <wx/dnd.h>
#include "Capacitor.h"
#include "Diode.h"
#include "GND.h"
#include "Inductor.h"
#include "Resistor.h"
#include "VCC.h"
#include "Pin.h"
#include "Wire.h"
#include "WireToWireConnection.h"
#include "ComponentDragTarget.h"

DrawingCanvas::DrawingCanvas(wxWindow* parent, wxMemoryDC& memDC, wxPoint& delta, wxSize Size)
	: m_parent(parent), memDC(memDC), delta(delta), Size(Size), wxPanel(parent, wxID_ANY) {
	memDC.SelectObject(circuitBitmap);
	m_grid_size = 30;

	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetDropTarget(new ComponentDragTarget(this));

	Bind(wxEVT_PAINT, &DrawingCanvas::OnPaint, this);
	Bind(wxEVT_LEFT_DOWN, &DrawingCanvas::OnMouseDown, this);
	Bind(wxEVT_LEFT_UP, &DrawingCanvas::OnMouseUp, this);
	Bind(wxEVT_MOTION, &DrawingCanvas::OnMouseMove, this);
	Bind(wxEVT_LEFT_DCLICK, &DrawingCanvas::Display_Component_info, this);
	Bind(wxEVT_RIGHT_DOWN, &DrawingCanvas::DisplayChangeMenu, this);
	// 获取面板的尺寸
	Size = GetClientSize();

	// 设置绘图设备的画笔
	memDC.SetPen(*wxBLACK_PEN);

	selected_component = NULL;
	Refresh();
}

void DrawingCanvas::OnPaint(wxPaintEvent& event) {
	// 将wxBitmap绘制到窗口上
	memDC.SetBackground(*wxWHITE_BRUSH);
	memDC.Clear();
	memDC.SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));
	// 绘制网格
	for (int x = 0; x < Size.x; x += m_grid_size)
		memDC.DrawLine(x, 0, x, Size.y);  // 垂直线
	for (int y = 0; y < Size.y; y += m_grid_size)
		memDC.DrawLine(0, y, Size.x, y);  // 水平线

	memDC.SetPen(*wxBLACK_PEN);

	for (auto& comp : cur_comp) {
		comp->draw();
	}

	//已经完成绘制的导线
	memDC.SetPen(*wxGREEN_PEN);
	// 绘制所有导线
	for (const auto& wire : wires) {
		memDC.DrawLine(wire->start_point, wire->end_point);
	}
	//正在绘制的导线
	if (draw_line) {
		memDC.DrawLine(m_startPoint, m_endPoint);
	}
	memDC.SetPen(*wxBLACK_PEN);

	wxPaintDC dc(this);

	//是否清除绘图
	if (clearBitmap) {
		memDC.Clear();
		dc.Clear();
		memDC.SetPen(*wxBLACK_PEN);
		clearBitmap = false;
		wires.clear();
	}

	dc.Blit(0, 0, Size.GetWidth(), Size.GetHeight(), &memDC, 0, 0, wxCOPY, true);
	
	//高亮未连接的引脚
	if (check_elec) {
		check_elec = false;
		HighlightUnconnected();
		unconnected_pins.clear();
	}
}

void DrawingCanvas::OnMouseDown(wxMouseEvent& event) {
	m_startPoint = event.GetPosition();
	if (!draw_line) {
		Bind(wxEVT_KEY_DOWN, &DrawingCanvas::OnKeyPress, this);
		IsInRangeComp(m_startPoint);
		if (selected_component == NULL) {
			IsInRangeWire(m_startPoint);    //如果判断没有点击到任何一个元器件，则判断是否点击导线
		}
	}
	else {
		selected_wire = new Wire();
		//增加画线辅助功能，保证导线的端点一定和元器件引脚重合，提高用户体验
		IsInRangePin(m_startPoint);
		selected_wire->start_point = m_startPoint;
	}
}

void DrawingCanvas::OnMouseMove(wxMouseEvent& event) {
	if (event.LeftIsDown())
	{
		m_endPoint = event.GetPosition();
		if (!draw_line && selected_component != NULL && selected_component->getMovingStatement()) {
			delta = m_endPoint - m_startPoint;
			TraceComp();
			selected_component->move();
			m_startPoint = m_endPoint;
		}
		Refresh();
	}
}

void DrawingCanvas::OnMouseUp(wxMouseEvent& event) {
	if (draw_line) {
		m_endPoint = event.GetPosition();
		
		//增加画线辅助功能，提高用户体验
		IsInRangePin(m_endPoint);
		selected_wire->end_point = m_endPoint;
		wires.push_back(selected_wire);
		selected_wire = NULL;
	}
	else {
		Unbind(wxEVT_KEY_DOWN, &DrawingCanvas::OnKeyPress, this);
		if (selected_component != NULL && selected_component->getMovingStatement()) {
			SnapToGrid();
			selected_component->setMovingStatement(false);
			selected_component = NULL;
		}
	}
	Refresh();
}

void DrawingCanvas::OnInitDrawLines(wxCommandEvent& event) { 
	if (!draw_line) {
		draw_line = true;
		m_endPoint = m_startPoint;
		Bind(wxEVT_RIGHT_DOWN, &DrawingCanvas::UndoLines, this);
	}
}

void DrawingCanvas::OnEndDrawLines(wxCommandEvent& event) { 
	if (draw_line) {
		draw_line = false;
		Unbind(wxEVT_RIGHT_DOWN, &DrawingCanvas::UndoLines, this);
	}
}

void DrawingCanvas::UndoLines(wxMouseEvent& event) {
	if (!wires.empty()) {
		Wire* tmpwire = wires.back();
		if (tmpwire != NULL) {
			if (tmpwire->start_pin != NULL) {
				tmpwire->start_pin->m_is_connected = false;
				tmpwire->start_pin->m_connected_wire = NULL;
			}
			if (tmpwire->end_pin != NULL) {
				tmpwire->end_pin->m_is_connected = false;
				tmpwire->end_pin->m_connected_wire = NULL;
			}
			delete tmpwire;
		}
		wires.pop_back();
		m_endPoint = m_startPoint;
		Refresh();
	}
}

void DrawingCanvas::Display_Component_info(wxMouseEvent& event) {
	wxPoint pos(event.GetPosition());
	IsInRangeComp(pos);
	if (selected_component != NULL) {
		if (selected_component->m_type == "电源") {
			wxString message = wxString::Format("电压（V）：%d", dynamic_cast<VCC*>(selected_component)->getVoltage());
			wxMessageBox(message, "电源参数");
		}
		else if (selected_component->m_type == "二极管") {
			wxString message = wxString::Format("最大整流电流IF（mA）：%d", dynamic_cast<Diode*>(selected_component)->getElectricCur());
			wxMessageBox(message, "二极管参数");
		}
		else if (selected_component->m_type == "电感") {
			wxString message = wxString::Format("自感系数（H）：%.2f", dynamic_cast<Inductor*>(selected_component)->getInductance());
			wxMessageBox(message, "电感参数");
		}
		else if (selected_component->m_type == "电阻") {
			wxString message = wxString::Format("阻值（Ω）：%d", dynamic_cast<Resistor*>(selected_component)->getRvalue());
			wxMessageBox(message, "电阻参数");
		}
		else if (selected_component->m_type == "电容") {
			wxString message = wxString::Format("电容C（μF）：%d", dynamic_cast<Capacitor*>(selected_component)->getCapacity());
			wxMessageBox(message, "电容参数");
		}
		else if (selected_component->m_type == "GND") {
			wxString message = wxString::Format("材料：圆钢    直径（mm）：%d", dynamic_cast<GND*>(selected_component)->getDiam());
			wxMessageBox(message, "接地线参数");
		}
	}
}

void DrawingCanvas::DisplayChangeMenu(wxMouseEvent& event) {
	wxPoint pos = event.GetPosition();
	selected_component = NULL;
	IsInRangeComp(pos);

	if (selected_component != NULL) {
		// 创建右键菜单
		wxMenu menu;
		menu.Append(ID_CHANGE_PROPERTIES, 
			"修改参数");
		// 绑定菜单事件
		Bind(wxEVT_MENU, &DrawingCanvas::DisplayChangeDialog, this, ID_CHANGE_PROPERTIES);

		// 显示右键菜单
		PopupMenu(&menu, pos);
	}
}

void DrawingCanvas::DisplayChangeDialog(wxCommandEvent& event) {
	if (propertyDialog) {
		propertyDialog->Destroy();
	}

	propertyDialog = new wxDialog(this, wxID_ANY, "修改参数", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	propertyDialog->Center();
	
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(new wxStaticText(propertyDialog, wxID_ANY,
		"              修改参数\n电源：电压/V，\n二极管：最大整流电流/mA，\n电感：自感系数/H，\n电阻：阻值/Ω，\n电容：电容量/μF，\nGND：直径/mm"
	), 0, wxALIGN_CENTER | wxALL, 5);

	stringstream ss;
	string value;
	if (selected_component != NULL) {
		if (selected_component->m_type == "电源") {
			ss << dynamic_cast<VCC*>(selected_component)->getVoltage();
		}
		else if (selected_component->m_type == "二极管") {
			ss << dynamic_cast<Diode*>(selected_component)->getElectricCur();
		}
		else if (selected_component->m_type == "电感") {
			ss << dynamic_cast<Inductor*>(selected_component)->getInductance();
		}
		else if (selected_component->m_type == "电阻") {
			ss << dynamic_cast<Resistor*>(selected_component)->getRvalue();
		}
		else if (selected_component->m_type == "电容") {
			ss << dynamic_cast<Capacitor*>(selected_component)->getCapacity();
		}
		else if (selected_component->m_type == "GND") {
			ss << dynamic_cast<GND*>(selected_component)->getDiam();
		}
	}
	ss >> value;

	textCtrl = new wxTextCtrl(propertyDialog, wxID_ANY, wxString(value));

	// 添加文本框
	sizer->Add(textCtrl, 1, wxALIGN_CENTER | wxALL, 5);

	// 确认按钮
	wxButton* confirmButton = new wxButton(propertyDialog, wxID_OK, "确认");
	sizer->Add(confirmButton, 0, wxALIGN_CENTER | wxALL, 5);

	// 绑定按钮事件
	confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent&) {
		propertyDialog->EndModal(wxID_OK);  // 关闭对话框并返回 wxID_OK
		});

	propertyDialog->SetSizerAndFit(sizer);
	propertyDialog->ShowModal();

	OnChangeProperties();
	propertyDialog->Destroy();
	propertyDialog = NULL;
}

void DrawingCanvas::OnChangeProperties() {
	if (selected_component != NULL) {
		if (selected_component->m_type == "电源") {
			dynamic_cast<VCC*>(selected_component)->setVoltage(wxAtoi(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "二极管") {
			dynamic_cast<Diode*>(selected_component)->setElectricCur(wxAtoi(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "电感") {
			dynamic_cast<Inductor*>(selected_component)->setInductance(wxAtof(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "电阻") {
			dynamic_cast<Resistor*>(selected_component)->setRvalue(wxAtoi(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "电容") {
			dynamic_cast<Capacitor*>(selected_component)->setCapacity(wxAtoi(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "GND") {
			dynamic_cast<GND*>(selected_component)->setDiam(wxAtoi(textCtrl->GetValue()));
		}
	}
}

void DrawingCanvas::OnKeyPress(wxKeyEvent& event) {
	int keyCode = event.GetKeyCode();
	if (keyCode == WXK_SPACE) {
		RotateComp();
	}
	else if (keyCode == WXK_DELETE) {
		if (selected_component != NULL) {
			DeleteComp();
		}
		else if (selected_wire != NULL) {
			DeleteWire();
		}
	}
	Refresh();
}

void DrawingCanvas::RotateComp() {
	if (selected_component != NULL) {
		int rotate_x, rotate_y;
		for (int i = 0; i < selected_component->getBasePointsNums(); i++) {
			rotate_x = selected_component->getCoordinate()[i].x;
			rotate_y = selected_component->getCoordinate()[i].y;
			selected_component->getCoordinate()[i].x = selected_component->center_point.x - selected_component->center_point.y + rotate_y;
			selected_component->getCoordinate()[i].y = selected_component->center_point.x + selected_component->center_point.y - rotate_x;
		}
		//引脚坐标也要旋转(同时更新与之相连接的导线）
		for (auto& pin : selected_component->Pins) {
			wxPoint* tmp = NULL;
			if (pin->m_is_connected) {
				if (IsConnected(pin->m_position, pin->m_connected_wire->start_point)) {
					tmp = &pin->m_connected_wire->start_point;
				}
				else {
					tmp = &pin->m_connected_wire->end_point;
				}
			}
			rotate_x = pin->m_position.x;
			rotate_y = pin->m_position.y;
			pin->m_position.x = selected_component->center_point.x - selected_component->center_point.y + rotate_y;
			pin->m_position.y = selected_component->center_point.x + selected_component->center_point.y - rotate_x;
			if (pin->m_is_connected && tmp != NULL) {
				*tmp = pin->m_position;
			}
		}

		selected_component->draw();
		selected_component->rotate_degree = (selected_component->rotate_degree + 1) % 4;
	}
}

void DrawingCanvas::DeleteComp() {  //删除元器件的同时，把与之相连的导线也删除
	if (selected_component == NULL) {
		return;
	}

	auto it = find(cur_comp.begin(), cur_comp.end(), selected_component);
	if (it != cur_comp.end()) {
		for (auto& pin : (*it)->Pins) {
			if (pin->m_is_connected) {
				selected_wire = pin->m_connected_wire;
				DeleteWire();
			}
		}
		delete* it;
		cur_comp.erase(it);
		selected_component = NULL;
		selected_wire = NULL;
	}
}

void DrawingCanvas::DeleteWire() {
	if (selected_wire == NULL) {
		return;
	}

	auto it = find(wires.begin(), wires.end(), selected_wire);
	if (it != wires.end()) {
		if ((*it)->start_pin != NULL) {
			(*it)->start_pin->m_is_connected = false;
			(*it)->start_pin->m_connected_wire = NULL;
		}
		if ((*it)->end_pin != NULL) {
			(*it)->end_pin->m_is_connected = false;
			(*it)->end_pin->m_connected_wire = NULL;
		}
		delete* it;
		wires.erase(it);
		selected_wire = NULL;
	}
}

void DrawingCanvas::CheckE(wxCommandEvent& event) {
	CheckElectricalConnections(cur_comp, wires);
	check_elec = true;
	Refresh();
}

void DrawingCanvas::CheckElectricalConnections(const std::vector<Component*>& components, std::vector<Wire*>& wires) {
	for (const auto& component : components) {
		for (const auto& pin : component->Pins) {
			if (!pin->m_is_connected) {
				wxLogMessage("Connection missing at component %s, point (%d, %d)",
					component->m_type, pin->m_position.x, pin->m_position.y);
				unconnected_pins.push_back(pin->m_position);
			}
		}
	}
	if (unconnected_pins.empty()) {
		wxLogMessage("No connection missing");
	}
}

bool DrawingCanvas::IsConnected(const wxPoint& point1, const wxPoint& point2, int threshold) {
	// 计算两点之间的距离
	int dx = point1.x - point2.x;
	int dy = point1.y - point2.y;
	double distance = dx * dx + dy * dy;

	// 判断距离是否小于阈值
	return distance <= threshold * threshold;
}

void DrawingCanvas::HighlightUnconnected() {
	wxPaintDC dc(this);
	dc.SetPen(*wxRED_PEN);
	dc.SetBrush(*wxRED_BRUSH);
	for (const auto& point : unconnected_pins) {
		dc.DrawCircle(point, 3);
	}
}

//导线
void DrawingCanvas::DrawConnect() {
	ComponentInfo EC_last(EC.back());
	if (EC_last.type == "导线") {
		for (int i = 0; EC_last.center_p[i] != wxPoint(0, 0); i += 2) {
			wires.push_back(new Wire(EC_last.center_p[i], EC_last.center_p[i + 1]));
		}
	}
}

//栅格吸附功能
void DrawingCanvas::SnapToGrid() {
	if (selected_component != NULL) {
		// 计算最近的网格点
		int snapped_x = (selected_component->center_point.x + m_grid_size / 2) / m_grid_size * m_grid_size;
		int snapped_y = (selected_component->center_point.y + m_grid_size / 2) / m_grid_size * m_grid_size;

		// 调整元器件位置
		delta = wxPoint(snapped_x, snapped_y) - selected_component->center_point;
		TraceComp();		
		selected_component->move();
		Refresh();  // 重新绘制
	}
}

//添加元器件
void DrawingCanvas::AddComponent(const wxString& componentType, wxPoint dropPosition) {
	if (componentType == "电阻") {
		selected_component = new Resistor(this, "电阻", dropPosition, 0, 13, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "电容") {
		selected_component = new Capacitor(this, "电容", dropPosition, 0, 9, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "二极管") {
		selected_component = new Diode(this, "二极管", dropPosition, 0, 8, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "电感") {
		selected_component = new Inductor(this, "电感", dropPosition, 0, 10, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "电源") {
		selected_component = new VCC(this, "电源", dropPosition, 0, 9, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "GND") {
		selected_component = new GND(this, "GND", dropPosition, 0, 11, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	Refresh();
}

void DrawingCanvas::IsInRangeComp(wxPoint& position) {
	for (auto& comp : cur_comp) {
		if (comp->range.Contains(position)) {
			selected_component = comp;
			selected_component->setMovingStatement(true);
			return;
		}
	}
	selected_component = NULL;
}

void DrawingCanvas::IsInRangePin(wxPoint& position) {
	for (auto& comp : cur_comp) {
		for (auto& pin : comp->Pins) {
			if (IsConnected(position, pin->m_position, 10)) {
				position = pin->m_position;   //如果导线端点与某元器件的引脚接近到一定的距离，则自动调整到引脚位置
				pin->m_is_connected = true;
				if (selected_wire != NULL) {
					pin->m_connected_wire = selected_wire;
					if (selected_wire->start_pin == NULL) {
						selected_wire->is_connected_start = true;
						selected_wire->start_pin = pin;
					}
					else {
						selected_wire->is_connected_end = true;
						selected_wire->end_pin = pin;
					}
				}
				return;
			}
		}
	}
}

void DrawingCanvas::IsInRangeWire(wxPoint& position) {
	for (auto& wire : wires) {
		if (wire->IsClose(position)) {
			selected_wire = wire;;
			return;
		}
	}
	selected_wire = NULL;
}

//用于实现实现拖动元器件时的导线跟随效果
void DrawingCanvas::TraceComp() {  
	if (selected_component != NULL) {
		for (auto& pin : selected_component->Pins) {
			if (pin->m_is_connected) {
				if (IsConnected(pin->m_connected_wire->start_point, pin->m_position)) {
					pin->m_connected_wire->start_point += delta;
				}
				if (IsConnected(pin->m_connected_wire->end_point, pin->m_position)) {
					pin->m_connected_wire->end_point += delta;
				}
			}
		}
	}
}

//建立导线与引脚的联系
void DrawingCanvas::CreateWireToPinConnection() {
	//用于在读取网表后，重新建立引脚和导线的连接关系，以便实现拖动元器件时的导线跟随效果
	for (auto& comp : cur_comp) {
		for (auto& pin : comp->Pins) {
			for (auto& wire : wires) {
				if (IsConnected(wire->start_point, pin->m_position)) {
					wire->is_connected_start = true;
					wire->start_pin = pin;
					pin->m_is_connected = true;
					pin->m_connected_wire = wire;
					break;
				}
				if (IsConnected(wire->end_point, pin->m_position)) {
					wire->is_connected_end = true;
					wire->end_pin = pin;
					pin->m_is_connected = true;
					pin->m_connected_wire = wire;
					break;
				}
			}
		}
	}
}

//建立导线与导线的联系
//void DrawingCanvas::CreateWireToWireConnection() {
//	for (auto& wire : wires) {
//		if (IsConnected(wire->start_point, pin->m_position)) {
//			wire->is_connected_start = true;
//			wire->start_pin = pin;
//			pin->m_is_connected = true;
//			pin->m_connected_wire = wire;
//			break;
//		}
//		if (IsConnected(wire->end_point, pin->m_position)) {
//			wire->is_connected_end = true;
//			wire->end_pin = pin;
//			pin->m_is_connected = true;
//			pin->m_connected_wire = wire;
//			break;
//		}
//	}
//
//}
//
////递归更新导线网络
//void DrawingCanvas::UpdateWireNetwork(Wire* wire, set<Wire*>& visited) {
//	if (!wire || visited.count(wire)) return; // 防止重复访问同一导线
//	visited.insert(wire);
//
//	// 更新当前导线的起点和终点位置
//	if (wire->start_pin) {
//		wire->start_point = wire->start_pin->m_position;
//	}
//	if (wire->end_pin) {
//		wire->end_point = wire->end_pin->m_position;
//	}
//
//	// 遍历所有与该导线相连的其他导线
//	for (Wire* connected_wire : wire->connected_wires) {
//		UpdateWireNetwork(connected_wire, visited);
//	}
//}
//
//void DrawingCanvas::UpdateConnectedWires(Component* component) {
//	if (component != NULL) {
//		std::set<Wire*> visited; // 用于记录已经更新的导线，避免重复更新
//
//		for (auto& pin : component->Pins) {
//			if (pin->m_connected_wire && pin->m_connected_wire) {
//				// 更新与当前引脚相连的导线网络
//				UpdateWireNetwork(pin->m_connected_wire, visited);
//			}
//		}
//	}
//	//this->Refresh(); // 刷新画布，重绘更新后的导线和元器件
//}

