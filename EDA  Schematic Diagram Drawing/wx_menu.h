
#ifndef __NONAME_H__
#define __NONAME_H__
#include <wx/wx.h>

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/frame.h>
#include"wx/dcbuffer.h"
#include <wx/event.h>
#include <wx/splitter.h>
#include <vector>
#include<fstream> 
#include<sstream>

#define wxID_REPAINT 6000
#define wxID_CLEAR_LINE 6001
#define wxID_END 6002
#define wxID_D 6003
#define wxID_L 6004
#define wxID_VCC 6005
#define wxID_R 6006
#define wxID_C 6007
#define wxID_GND 6008
#define wxID_check 6009

using namespace std;
class Component;
class Wire;
class DrawingCanvas;
class ComponentLibrary;

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame1 : public wxFrame
{

protected:
	wxMenuBar* m_menubar1;
	wxMenu* m_menu2;
	wxMenu* m_menu3;
	wxMenu* m_menu1;
	wxMenu* m_menu4;
	wxMemoryDC memDC;
	wxPoint delta;
	wxSize Size;
	vector<wxPoint>unconnected_pins;

public:
	MyFrame1(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "‘≠¿ÌÕº", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1000, 700), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	void OnExit(wxCommandEvent& event);
	void OnInitDrawLines(wxCommandEvent& event);
	void OnEndDrawLines(wxCommandEvent& event);
	void OnSaveSchematicDialog(wxCommandEvent& event);
	void OnOpenSchematicDialog(wxCommandEvent& event);
	void OnSave(wxString& savePath);
	void OnClear(wxCommandEvent& event);
	void OnRePaint(wxString& filePath);
	void CheckE(wxCommandEvent& event);
	
	~MyFrame1() {}
	wxSplitterWindow* splitter;
	DrawingCanvas* canvas;
	wxPanel* componentLibraryPanel;

private:
	wxDECLARE_EVENT_TABLE();
};


#endif //__NONAME_H__
