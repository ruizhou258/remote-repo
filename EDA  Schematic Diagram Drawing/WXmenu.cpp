#include "wx_menu.h"
#include "Capacitor.h"
#include "Diode.h"
#include "GND.h"
#include "Inductor.h"
#include "Resistor.h"
#include "VCC.h"
#include "Pin.h"
#include "Wire.h"
#include "DrawingCanvas.h"
#include "ComponentLibrary.h"

using namespace std;

wxBEGIN_EVENT_TABLE(MyFrame1, wxFrame)
//EVT_PAINT(MyFrame1::OnPaint)
EVT_MENU(wxID_EXIT, MyFrame1::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame1::OnInitDrawLines)
EVT_MENU(wxID_END, MyFrame1::OnEndDrawLines)
EVT_MENU(wxID_SAVE, MyFrame1::OnSaveSchematicDialog)
EVT_MENU(wxID_CLEAR, MyFrame1::OnClear)
EVT_MENU(wxID_REPAINT, MyFrame1::OnOpenSchematicDialog)
EVT_MENU(wxID_check, MyFrame1::CheckE)

wxEND_EVENT_TABLE()

MyFrame1::MyFrame1(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	m_menubar1 = new wxMenuBar(0);
	m_menu2 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem(m_menu2, wxID_SAVE, wxString(wxT("保存网表")), wxEmptyString, wxITEM_NORMAL);
	m_menu2->Append(m_menuItem1);

	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem(m_menu2, wxID_REPAINT, wxString(wxT("读取网表")), wxEmptyString, wxITEM_NORMAL);
	m_menu2->Append(m_menuItem2);

	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem(m_menu2, wxID_CLEAR, wxString(wxT("清除绘图")), wxEmptyString, wxITEM_NORMAL);
	m_menu2->Append(m_menuItem3);

	m_menubar1->Append(m_menu2, wxT("文件"));

	m_menu3 = new wxMenu();
	m_menu1 = new wxMenu();
	wxMenuItem* m_menu1Item = new wxMenuItem(m_menu3, wxID_ANY, wxT("导线绘制"), wxEmptyString, wxITEM_NORMAL, m_menu1);
	wxMenuItem* m_menuItem4;
	wxMenuItem* m_menuItem5;
	m_menuItem4 = new wxMenuItem(m_menu1, wxID_ABOUT, wxString(wxT("开始")), wxEmptyString, wxITEM_NORMAL);
	m_menuItem5 = new wxMenuItem(m_menu1, wxID_END, wxString(wxT("结束")), wxEmptyString, wxITEM_NORMAL);
	wxMenuItem* m_menu1Item6 = new wxMenuItem(m_menu3, wxID_check, wxT("电气检查"), wxEmptyString, wxITEM_NORMAL);

	m_menu1->Append(m_menuItem4);
	m_menu1->Append(m_menuItem5);
	m_menu3->Append(m_menu1Item);
	m_menu3->Append(m_menu1Item6);

	m_menubar1->Append(m_menu3, wxT("扩展"));
	this->SetMenuBar(m_menubar1);
	this->Centre(wxBOTH);

	// 获取面板的尺寸
	Size = GetClientSize();

	// 设置绘图设备的画笔
	memDC.SetPen(*wxBLACK_PEN);

	splitter = new wxSplitterWindow(this);
	canvas = new DrawingCanvas(splitter, memDC, delta, Size);
	ComponentLibrary* componentLibraryPanel = new ComponentLibrary(splitter);
	componentLibraryPanel->CreateComponentLibrary(componentLibraryPanel);
	splitter->SplitVertically(componentLibraryPanel, canvas, 200);
	splitter->SetMinimumPaneSize(150);
}

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;
	wxInitAllImageHandlers();
	MyFrame1* dlg = new MyFrame1(NULL);
	dlg->Show();
	return true;
}

void MyFrame1::OnExit(wxCommandEvent& event) {
	Close();
}

//开始连线
void MyFrame1::OnInitDrawLines(wxCommandEvent& event) {
	canvas->OnInitDrawLines(event);
}

//结束连线
void MyFrame1::OnEndDrawLines(wxCommandEvent& event) {
	canvas->OnEndDrawLines(event);
}

//清除绘图
void MyFrame1::OnClear(wxCommandEvent& event) {
	canvas->clearBitmap = true;
	if (canvas->draw_line) {
		canvas->OnEndDrawLines(event);
	}
	if (!canvas->cur_comp.empty()) {
		for (auto& comp : canvas->cur_comp) {
			if (comp != NULL) {
				delete comp;
			}
		}
		canvas->selected_component = NULL;
		canvas->cur_comp.clear();
	}

	if (!canvas->wires.empty()) {
		for (auto& wire : canvas->wires) {
			if (wire != NULL) {
				delete wire;
			}
		}
		canvas->wires.clear();
	}
	canvas->Refresh();
}

void MyFrame1::OnSaveSchematicDialog(wxCommandEvent& event) {
	// 创建文件保存对话框
	wxFileDialog saveFileDialog(
		this,                          // 父窗口
		"保存文件",                    // 对话框标题
		wxEmptyString,                 // 初始路径
		wxEmptyString,                 // 初始文件名
		"原理图文件 (*.txt)|*.txt",    // 文件过滤器
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT // 保存标志
	);

	// 显示对话框并处理用户选择
	if (saveFileDialog.ShowModal() == wxID_CANCEL) {
		return; // 用户取消保存
	}

	// 获取用户选择的路径和文件名
	wxString savePath = saveFileDialog.GetPath();

	// 调用保存函数
	OnSave(savePath);
}

void MyFrame1::OnOpenSchematicDialog(wxCommandEvent& event) {
	// 创建文件选择对话框
	wxFileDialog openFileDialog(
		this,
		"打开文件", 
		wxEmptyString, 
		wxEmptyString,
		"原理图文件 (*.txt)|*.txt",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST
	);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;  // 用户取消选择

	// 获取选中的文件路径
	wxString filePath = openFileDialog.GetPath();

	// 调用读取文件内容并重绘的函数
	OnRePaint(filePath);
}

void MyFrame1::OnSave(wxString& savePath) {
	ofstream fout(savePath.ToStdString());
	if (!fout.is_open()) {
		cerr << "Failed to open the file." << std::endl;
	}

	for (auto& comp : canvas->cur_comp) {
		comp->save(fout);
		if (comp->m_type == "电源") {
			fout << dynamic_cast<VCC*>(comp)->getVoltage() << endl;
		}
		else if (comp->m_type == "二极管") {
			fout << dynamic_cast<Diode*>(comp)->getElectricCur() << endl;
		}
		else if (comp->m_type == "电感") {
			fout << dynamic_cast<Inductor*>(comp)->getInductance() << endl;
		}
		else if (comp->m_type == "电阻") {
			fout << dynamic_cast<Resistor*>(comp)->getRvalue() << endl;
		}
		else if (comp->m_type == "电容") {
			fout << dynamic_cast<Capacitor*>(comp)->getCapacity() << endl;
		}
		else if (comp->m_type == "GND") {
			fout << dynamic_cast<GND*>(comp)->getDiam() << endl;
		}
	}

	//导线
	if(!canvas->wires.empty())
	fout << "导线 ";
	for (auto& wire : canvas->wires) {
		wire->save(fout);
	}
	fout << endl;
}

void MyFrame1::OnRePaint(wxString& filePath) {
	//重绘之前，先把目前的绘图清除掉（元器件和导线都要删除）
	if (!canvas->cur_comp.empty()) {
		for (auto& comp : canvas->cur_comp) {
			if (comp != NULL) {
				delete comp;
			}
		}
		canvas->selected_component = NULL;
		canvas->cur_comp.clear();
	}

	if (!canvas->wires.empty()) {
		for (auto& wire : canvas->wires) {
			if (wire != NULL) {
				delete wire;
			}
		}
		canvas->wires.clear();
	}

	ifstream fin(filePath.ToStdString());
	if (!fin.is_open()) {
		cerr << "Failed to open the file." << std::endl;
	}
	char line[1024] = { 0 };
	//按行读取，并且提取其中的名称、坐标信息
	while (fin.getline(line, sizeof(line))) {
		stringstream word(line);
		DrawingCanvas::ComponentInfo C;
		word >> C.type;
		int j = 0;
		char comma;
		wxPoint point;
		while (word >> point.x >> comma >> point.y) {
			C.center_p[j].x = point.x;
			C.center_p[j].y = point.y;
			if (C.type != "导线") {
				word >> C.rotate_degree;
				string str;
				word >> str;
				if (str.find(".") != std::string::npos) {
					C.d.DOUBLE_data = stod(str);
				}
				else {
					C.d.INT_data = stoi(str);
				}
			}
			j++;
		}
		canvas->EC.push_back(C);
	}

	for (auto ec : canvas->EC) {
		if (ec.type == "二极管") {
			canvas->selected_component = new Diode(canvas, "二极管", ec.center_p[0], ec.rotate_degree, 8, canvas->memDC, canvas->delta, canvas->Size);
			Diode* diode = dynamic_cast<Diode*>(canvas->selected_component);
			diode->setElectricCur(ec.d.INT_data);
			canvas->selected_component->rotate_degree = 0;
			for (int i = 0; i < ec.rotate_degree; i++) {
				canvas->RotateComp();
			}
			canvas->cur_comp.push_back(canvas->selected_component);
			canvas->SnapToGrid();
		}
		else if (ec.type == "电感") {
			canvas->selected_component = new Inductor(canvas, "电感", ec.center_p[0], ec.rotate_degree, 10, canvas->memDC, canvas->delta, canvas->Size);
			Inductor* inductor = dynamic_cast<Inductor*>(canvas->selected_component);
			inductor->setInductance(ec.d.DOUBLE_data);
			canvas->selected_component->rotate_degree = 0;
			for (int i = 0; i < ec.rotate_degree; i++) {
				canvas->RotateComp();
			}
			canvas->cur_comp.push_back(canvas->selected_component);
			canvas->SnapToGrid();
		}
		else if (ec.type == "电容") {
			canvas->selected_component = new Capacitor(canvas, "电容", ec.center_p[0], ec.rotate_degree, 9, canvas->memDC, canvas->delta, canvas->Size);
			Capacitor* capacitor = dynamic_cast<Capacitor*>(canvas->selected_component);
			capacitor->setCapacity(ec.d.INT_data);
			canvas->selected_component->rotate_degree = 0;
			for (int i = 0; i < ec.rotate_degree; i++) {
				canvas->RotateComp();
			}
			canvas->cur_comp.push_back(canvas->selected_component);
			canvas->SnapToGrid();
		}
		else if (ec.type == "GND") {
			canvas->selected_component = new GND(canvas, "GND", ec.center_p[0], ec.rotate_degree, 11, canvas->memDC, canvas->delta, canvas->Size);
			GND* gnd = dynamic_cast<GND*>(canvas->selected_component);
			gnd->setDiam(ec.d.INT_data);
			canvas->selected_component->rotate_degree = 0;
			for (int i = 0; i < ec.rotate_degree; i++) {
				canvas->RotateComp();
			}
			canvas->cur_comp.push_back(canvas->selected_component);
			canvas->SnapToGrid();
		}
		else if (ec.type == "电阻") {
			canvas->selected_component = new Resistor(canvas, "电阻", ec.center_p[0], ec.rotate_degree, 13, canvas->memDC, canvas->delta, canvas->Size);
			Resistor* resistor = dynamic_cast<Resistor*>(canvas->selected_component);
			resistor->setRvalue(ec.d.INT_data);
			canvas->selected_component->rotate_degree = 0;
			for (int i = 0; i < ec.rotate_degree; i++) {
				canvas->RotateComp();
			}
			canvas->cur_comp.push_back(canvas->selected_component);
			canvas->SnapToGrid();
		}
		else if (ec.type == "电源") {
			canvas->selected_component = new VCC(canvas, "电源", ec.center_p[0], ec.rotate_degree, 9, canvas->memDC, canvas->delta, canvas->Size);
			VCC* vcc = dynamic_cast<VCC*>(canvas->selected_component);
			vcc->setVoltage(ec.d.INT_data);
			canvas->selected_component->rotate_degree = 0;
			for (int i = 0; i < ec.rotate_degree; i++) {
				canvas->RotateComp();
			}
			canvas->cur_comp.push_back(canvas->selected_component);
			canvas->SnapToGrid();
		}
		else if (ec.type == "导线") {
			canvas->DrawConnect();
		}
	}

	//重新建立引脚和导线的连接关系，以便实现拖动元器件的跟随
	canvas->CreateWireToPinConnection();
	canvas->EC.clear();
	canvas->Refresh();
}

void MyFrame1::CheckE(wxCommandEvent& event) {
	canvas->CheckE(event);
}
