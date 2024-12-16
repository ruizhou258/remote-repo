#pragma once
#include "wx_menu.h"
#include "DrawingCanvas.h"
#include "Pin.h"

class Component : public wxFrame{
public:
	wxWindow* m_parent;
	Component(wxWindow* parent, string type, wxPoint center_point, int rotate_degree, int base_points_num, wxMemoryDC& memDC, wxPoint& delta, wxSize Size)
		:m_type(type), center_point(center_point), rotate_degree(rotate_degree), m_base_points_num(base_points_num),
		memDC(memDC), delta(delta), Size(Size), wxFrame(parent, wxID_ANY, "C"), m_parent(parent) {
		m_if_exist = true;
		m_moving = false;
		coordinate.resize(base_points_num);
		offset.resize(base_points_num);
	};

	//实现拖动重绘的函数
	void move() {
		if (getMovingStatement()) {
			for (int i = 0; i < getBasePointsNums(); i++) {
				getPoint(i) += delta;
			}
			for (auto& pin : Pins) {
				pin->m_position += delta;
			}
			center_point += delta;
			setRange();
		}
	}

	//设置拖动时鼠标的判定范围
	void setRange() {
		range.SetPosition(center_point - wxPoint(40, 40));
		range.SetSize(wxSize(80, 80));
	}

	virtual void draw() = 0;    //封装绘制操作的函数
	void save(ofstream& fout) { //把信息保存到文件中
		if (getExistingStatement()) {
			fout << m_type <<" ";
			fout << center_point.x << "," << center_point.y << " ";
			fout << rotate_degree << " ";
		}
	}

	void setExistingStatement(bool if_exist) {
		m_if_exist = if_exist;
	}
	bool getExistingStatement() {
		return m_if_exist;
	}

	void setMovingStatement(bool moving) {
		m_moving = moving;
	}
	bool getMovingStatement() {
		return m_moving;
	}

	int getBasePointsNums() {
		return m_base_points_num;
	}
	wxPoint& getPoint(int index) {
		if (index < m_base_points_num)
			return coordinate[index];
		else
			return coordinate[0];
	}
	vector<wxPoint>& getCoordinate() {
		return coordinate;
	}

	~Component() {
		if (!Pins.empty()) {
			for (auto& pin : Pins) {
				if (pin != NULL) {
					delete pin;
				}
			}
			Pins.clear();
		}
	}

	wxMemoryDC& memDC;
	wxPoint& delta;
	wxSize Size;
	wxRect range;
	wxPoint center_point;
	string m_type;
	vector<wxPoint> offset;
	vector<Pin*> Pins;
	int rotate_degree;

private:
	bool m_if_exist;
	bool m_moving;
	int m_base_points_num;
	vector<wxPoint> coordinate;

};