#pragma once
#include "wx_menu.h"
#include "WireToWireConnection.h"
#include<unordered_map>
class Pin;

class Wire {
public:
    Wire()
        :start_point(wxPoint(0, 0)), end_point(wxPoint(0, 0)) {
        is_connected_start = false;
        is_connected_end = false;
        start_pin = NULL;
        end_pin = NULL;
    }
    Wire(const wxPoint& start, const wxPoint& end)
        : start_point(start), end_point(end) {
        is_connected_start = false;
        is_connected_end = false;
        start_pin = NULL;
        end_pin = NULL;
    }

    // �ж��Ƿ�ѡ�е���
    bool IsClose(const wxPoint& point, int threshold = 5) const {
        // �����Ƿ����߶θ���
        double distance = PointToLineDistance(point, start_point, end_point);
        return distance <= threshold;
    }

    // ����㵽ֱ�ߵľ���
    double PointToLineDistance(const wxPoint& p, const wxPoint& a, const wxPoint& b) const {
        double x = p.x, y = p.y;
        double x1 = a.x, y1 = a.y, x2 = b.x, y2 = b.y;

        // ����ֱ�ߵ�ϵ�� A, B, C ʹ�÷���Ϊ Ax + By + C = 0
        double A = y1 - y2;
        double B = x2 - x1;
        double C = x1 * y2 - x2 * y1;

        // ʹ�õ㵽ֱ�ߵľ��빫ʽ����
        double distance = fabs(A * x + B * y + C) / sqrt(A * A + B * B);

        return distance;
    }

    void save(ofstream& fout) {
        fout << start_point.x << "," << start_point.y << " ";
        fout << end_point.x << "," <<end_point.y << " ";
    }

    wxPoint start_point; // �������
    wxPoint end_point;   // �����յ�
    bool is_connected_start; // ����Ƿ����ӵ�Ԫ��������
    bool is_connected_end;   // �յ��Ƿ����ӵ�Ԫ��������
    //vector<Pin*> connected_pins;
    vector<Wire*> connected_wires;
    Pin* start_pin;
    Pin* end_pin;
};
