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
	// ��ȡ���ĳߴ�
	Size = GetClientSize();

	// ���û�ͼ�豸�Ļ���
	memDC.SetPen(*wxBLACK_PEN);

	selected_component = NULL;
	Refresh();
}

void DrawingCanvas::OnPaint(wxPaintEvent& event) {
	// ��wxBitmap���Ƶ�������
	memDC.SetBackground(*wxWHITE_BRUSH);
	memDC.Clear();
	memDC.SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));
	// ��������
	for (int x = 0; x < Size.x; x += m_grid_size)
		memDC.DrawLine(x, 0, x, Size.y);  // ��ֱ��
	for (int y = 0; y < Size.y; y += m_grid_size)
		memDC.DrawLine(0, y, Size.x, y);  // ˮƽ��

	memDC.SetPen(*wxBLACK_PEN);

	for (auto& comp : cur_comp) {
		comp->draw();
	}

	//�Ѿ���ɻ��Ƶĵ���
	memDC.SetPen(*wxGREEN_PEN);
	// �������е���
	for (const auto& wire : wires) {
		memDC.DrawLine(wire->start_point, wire->end_point);
	}
	//���ڻ��Ƶĵ���
	if (draw_line) {
		memDC.DrawLine(m_startPoint, m_endPoint);
	}
	memDC.SetPen(*wxBLACK_PEN);

	wxPaintDC dc(this);

	//�Ƿ������ͼ
	if (clearBitmap) {
		memDC.Clear();
		dc.Clear();
		memDC.SetPen(*wxBLACK_PEN);
		clearBitmap = false;
		wires.clear();
	}

	dc.Blit(0, 0, Size.GetWidth(), Size.GetHeight(), &memDC, 0, 0, wxCOPY, true);
	
	//����δ���ӵ�����
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
			IsInRangeWire(m_startPoint);    //����ж�û�е�����κ�һ��Ԫ���������ж��Ƿ�������
		}
	}
	else {
		selected_wire = new Wire();
		//���ӻ��߸������ܣ���֤���ߵĶ˵�һ����Ԫ���������غϣ�����û�����
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
		
		//���ӻ��߸������ܣ�����û�����
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
		if (selected_component->m_type == "��Դ") {
			wxString message = wxString::Format("��ѹ��V����%d", dynamic_cast<VCC*>(selected_component)->getVoltage());
			wxMessageBox(message, "��Դ����");
		}
		else if (selected_component->m_type == "������") {
			wxString message = wxString::Format("�����������IF��mA����%d", dynamic_cast<Diode*>(selected_component)->getElectricCur());
			wxMessageBox(message, "�����ܲ���");
		}
		else if (selected_component->m_type == "���") {
			wxString message = wxString::Format("�Ը�ϵ����H����%.2f", dynamic_cast<Inductor*>(selected_component)->getInductance());
			wxMessageBox(message, "��в���");
		}
		else if (selected_component->m_type == "����") {
			wxString message = wxString::Format("��ֵ��������%d", dynamic_cast<Resistor*>(selected_component)->getRvalue());
			wxMessageBox(message, "�������");
		}
		else if (selected_component->m_type == "����") {
			wxString message = wxString::Format("����C����F����%d", dynamic_cast<Capacitor*>(selected_component)->getCapacity());
			wxMessageBox(message, "���ݲ���");
		}
		else if (selected_component->m_type == "GND") {
			wxString message = wxString::Format("���ϣ�Բ��    ֱ����mm����%d", dynamic_cast<GND*>(selected_component)->getDiam());
			wxMessageBox(message, "�ӵ��߲���");
		}
	}
}

void DrawingCanvas::DisplayChangeMenu(wxMouseEvent& event) {
	wxPoint pos = event.GetPosition();
	selected_component = NULL;
	IsInRangeComp(pos);

	if (selected_component != NULL) {
		// �����Ҽ��˵�
		wxMenu menu;
		menu.Append(ID_CHANGE_PROPERTIES, 
			"�޸Ĳ���");
		// �󶨲˵��¼�
		Bind(wxEVT_MENU, &DrawingCanvas::DisplayChangeDialog, this, ID_CHANGE_PROPERTIES);

		// ��ʾ�Ҽ��˵�
		PopupMenu(&menu, pos);
	}
}

void DrawingCanvas::DisplayChangeDialog(wxCommandEvent& event) {
	if (propertyDialog) {
		propertyDialog->Destroy();
	}

	propertyDialog = new wxDialog(this, wxID_ANY, "�޸Ĳ���", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	propertyDialog->Center();
	
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(new wxStaticText(propertyDialog, wxID_ANY,
		"              �޸Ĳ���\n��Դ����ѹ/V��\n�����ܣ������������/mA��\n��У��Ը�ϵ��/H��\n���裺��ֵ/����\n���ݣ�������/��F��\nGND��ֱ��/mm"
	), 0, wxALIGN_CENTER | wxALL, 5);

	stringstream ss;
	string value;
	if (selected_component != NULL) {
		if (selected_component->m_type == "��Դ") {
			ss << dynamic_cast<VCC*>(selected_component)->getVoltage();
		}
		else if (selected_component->m_type == "������") {
			ss << dynamic_cast<Diode*>(selected_component)->getElectricCur();
		}
		else if (selected_component->m_type == "���") {
			ss << dynamic_cast<Inductor*>(selected_component)->getInductance();
		}
		else if (selected_component->m_type == "����") {
			ss << dynamic_cast<Resistor*>(selected_component)->getRvalue();
		}
		else if (selected_component->m_type == "����") {
			ss << dynamic_cast<Capacitor*>(selected_component)->getCapacity();
		}
		else if (selected_component->m_type == "GND") {
			ss << dynamic_cast<GND*>(selected_component)->getDiam();
		}
	}
	ss >> value;

	textCtrl = new wxTextCtrl(propertyDialog, wxID_ANY, wxString(value));

	// ����ı���
	sizer->Add(textCtrl, 1, wxALIGN_CENTER | wxALL, 5);

	// ȷ�ϰ�ť
	wxButton* confirmButton = new wxButton(propertyDialog, wxID_OK, "ȷ��");
	sizer->Add(confirmButton, 0, wxALIGN_CENTER | wxALL, 5);

	// �󶨰�ť�¼�
	confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent&) {
		propertyDialog->EndModal(wxID_OK);  // �رնԻ��򲢷��� wxID_OK
		});

	propertyDialog->SetSizerAndFit(sizer);
	propertyDialog->ShowModal();

	OnChangeProperties();
	propertyDialog->Destroy();
	propertyDialog = NULL;
}

void DrawingCanvas::OnChangeProperties() {
	if (selected_component != NULL) {
		if (selected_component->m_type == "��Դ") {
			dynamic_cast<VCC*>(selected_component)->setVoltage(wxAtoi(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "������") {
			dynamic_cast<Diode*>(selected_component)->setElectricCur(wxAtoi(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "���") {
			dynamic_cast<Inductor*>(selected_component)->setInductance(wxAtof(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "����") {
			dynamic_cast<Resistor*>(selected_component)->setRvalue(wxAtoi(textCtrl->GetValue()));
		}
		else if (selected_component->m_type == "����") {
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
		//��������ҲҪ��ת(ͬʱ������֮�����ӵĵ��ߣ�
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

void DrawingCanvas::DeleteComp() {  //ɾ��Ԫ������ͬʱ������֮�����ĵ���Ҳɾ��
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
	// ��������֮��ľ���
	int dx = point1.x - point2.x;
	int dy = point1.y - point2.y;
	double distance = dx * dx + dy * dy;

	// �жϾ����Ƿ�С����ֵ
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

//����
void DrawingCanvas::DrawConnect() {
	ComponentInfo EC_last(EC.back());
	if (EC_last.type == "����") {
		for (int i = 0; EC_last.center_p[i] != wxPoint(0, 0); i += 2) {
			wires.push_back(new Wire(EC_last.center_p[i], EC_last.center_p[i + 1]));
		}
	}
}

//դ����������
void DrawingCanvas::SnapToGrid() {
	if (selected_component != NULL) {
		// ��������������
		int snapped_x = (selected_component->center_point.x + m_grid_size / 2) / m_grid_size * m_grid_size;
		int snapped_y = (selected_component->center_point.y + m_grid_size / 2) / m_grid_size * m_grid_size;

		// ����Ԫ����λ��
		delta = wxPoint(snapped_x, snapped_y) - selected_component->center_point;
		TraceComp();		
		selected_component->move();
		Refresh();  // ���»���
	}
}

//���Ԫ����
void DrawingCanvas::AddComponent(const wxString& componentType, wxPoint dropPosition) {
	if (componentType == "����") {
		selected_component = new Resistor(this, "����", dropPosition, 0, 13, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "����") {
		selected_component = new Capacitor(this, "����", dropPosition, 0, 9, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "������") {
		selected_component = new Diode(this, "������", dropPosition, 0, 8, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "���") {
		selected_component = new Inductor(this, "���", dropPosition, 0, 10, memDC, delta, Size);
		cur_comp.push_back(selected_component);
		selected_component->setExistingStatement(true);
		selected_component->setMovingStatement(true);
		SnapToGrid();
		selected_component->setMovingStatement(false);
	}
	else if (componentType == "��Դ") {
		selected_component = new VCC(this, "��Դ", dropPosition, 0, 9, memDC, delta, Size);
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
				position = pin->m_position;   //������߶˵���ĳԪ���������Žӽ���һ���ľ��룬���Զ�����������λ��
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

//����ʵ��ʵ���϶�Ԫ����ʱ�ĵ��߸���Ч��
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

//�������������ŵ���ϵ
void DrawingCanvas::CreateWireToPinConnection() {
	//�����ڶ�ȡ��������½������ź͵��ߵ����ӹ�ϵ���Ա�ʵ���϶�Ԫ����ʱ�ĵ��߸���Ч��
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

//���������뵼�ߵ���ϵ
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
////�ݹ���µ�������
//void DrawingCanvas::UpdateWireNetwork(Wire* wire, set<Wire*>& visited) {
//	if (!wire || visited.count(wire)) return; // ��ֹ�ظ�����ͬһ����
//	visited.insert(wire);
//
//	// ���µ�ǰ���ߵ������յ�λ��
//	if (wire->start_pin) {
//		wire->start_point = wire->start_pin->m_position;
//	}
//	if (wire->end_pin) {
//		wire->end_point = wire->end_pin->m_position;
//	}
//
//	// ����������õ�����������������
//	for (Wire* connected_wire : wire->connected_wires) {
//		UpdateWireNetwork(connected_wire, visited);
//	}
//}
//
//void DrawingCanvas::UpdateConnectedWires(Component* component) {
//	if (component != NULL) {
//		std::set<Wire*> visited; // ���ڼ�¼�Ѿ����µĵ��ߣ������ظ�����
//
//		for (auto& pin : component->Pins) {
//			if (pin->m_connected_wire && pin->m_connected_wire) {
//				// �����뵱ǰ���������ĵ�������
//				UpdateWireNetwork(pin->m_connected_wire, visited);
//			}
//		}
//	}
//	//this->Refresh(); // ˢ�»������ػ���º�ĵ��ߺ�Ԫ����
//}

