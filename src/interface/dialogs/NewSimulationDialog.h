#ifndef _NEW_SIMULATION_DIALOG_H_
#define _NEW_SIMULATION_DIALOG_H_

#include <wx/wx.h>
#include <application/ConfigFileLoader.h>


class NewSimulationDialog : public wxDialog
{
public:
	NewSimulationDialog(wxWindow* parent, wxWindowID id, const wxString& title,
	    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE);

	const SimulationConfig& GetSimulationConfig() const { return m_config; }


private:
	DECLARE_EVENT_TABLE();

	void OnOpenButtonPressed(wxCommandEvent& e);


private:
	SimulationConfig	m_config;

	//wxTextCtrl* dialogText;
	wxButton*		m_buttonOkay;
	wxButton*		m_buttonCancel;

	wxTextCtrl*		m_textLoadMessage;
	wxTextCtrl*		m_textConfigFilePath;
	wxButton*		m_buttonOpenConfigFile;
	ConfigFileLoader	m_configFileLoader;
};


#endif // _NEW_SIMULATION_DIALOG_H_