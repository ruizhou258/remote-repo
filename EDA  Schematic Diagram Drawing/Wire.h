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

    // 判断是否选中导线
    bool IsClose(const wxPoint& point, int threshold = 5) const {
        // 检查点是否在线段附近
        double distance = PointToLineDistance(point, start_point, end_point);
        return distance <= threshold;
    }

    // 计算点到直线的距离
    double PointToLineDistance(const wxPoint& p, const wxPoint& a, const wxPoint& b) const {
        double x = p.x, y = p.y;
        double x1 = a.x, y1 = a.y, x2 = b.x, y2 = b.y;

        // 计算直线的系数 A, B, C 使得方程为 Ax + By + C = 0
        double A = y1 - y2;
        double B = x2 - x1;
        double C = x1 * y2 - x2 * y1;

        // 使用点到直线的距离公式计算
        double distance = fabs(A * x + B * y + C) / sqrt(A * A + B * B);

        return distance;
    }

    void save(ofstream& fout) {
        fout << start_point.x << "," << start_point.y << " ";
        fout << end_point.x << "," <<end_point.y << " ";
    }

    wxPoint start_point; // 导线起点
    wxPoint end_point;   // 导线终点
    bool is_connected_start; // 起点是否连接到元器件引脚
    bool is_connected_end;   // 终点是否连接到元器件引脚
    //vector<Pin*> connected_pins;
    vector<Wire*> connected_wires;
    Pin* start_pin;
    Pin* end_pin;
};
