#include "NewSimulationDialog.h"


enum
{
    BUTTON_OPEN_CONFIG_FILE = wxID_HIGHEST + 1,
};


wxBEGIN_EVENT_TABLE(NewSimulationDialog, wxDialog)

	EVT_BUTTON(BUTTON_OPEN_CONFIG_FILE, OnOpenButtonPressed)

wxEND_EVENT_TABLE()


NewSimulationDialog::NewSimulationDialog(wxWindow* parent,
			wxWindowID id, const wxString& title,
			const wxPoint& position, const wxSize& size, long style) :

	wxDialog(parent, id, title, position, size, style | wxRESIZE_BORDER)
{
	m_textLoadMessage = new wxTextCtrl(this, -1, _("Please specify a configuration file"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);
	m_buttonOpenConfigFile = new wxButton(this, BUTTON_OPEN_CONFIG_FILE, _("Browse"));
	m_textConfigFilePath = new wxTextCtrl(this, -1, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	m_buttonOkay = new wxButton(this, wxID_OK, _("OK"));
	m_buttonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
	
	int b = 2;

	// Open file text box and button
	wxBoxSizer* sizerOpenFile = new wxBoxSizer(wxHORIZONTAL);
	sizerOpenFile->Add(m_textConfigFilePath, 1, wxEXPAND, 0);
	sizerOpenFile->Add(m_buttonOpenConfigFile, 0, 0, 0);
	wxStaticBoxSizer* sizerConfigBox = new wxStaticBoxSizer(wxVERTICAL, this, "Configuration File:");
	sizerConfigBox->Add(sizerOpenFile, 0, wxEXPAND | wxALL, b);
	sizerConfigBox->Add(m_textLoadMessage, 1, wxEXPAND | wxALL, b);
	m_textLoadMessage->SetTransparent(true);

	// Okay and cancel buttons
	wxBoxSizer* sizerBottomButtons = new wxBoxSizer(wxHORIZONTAL);
	sizerBottomButtons->Add(m_buttonOkay, 1, 0, 0);
	sizerBottomButtons->Add(m_buttonCancel, 0, 0, 0);
	
	// Vertical layout
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(sizerConfigBox, 1, wxEXPAND | wxALL, b);
	sizer->Add(sizerBottomButtons, 0, wxALIGN_RIGHT | wxRIGHT | wxALL, b);
	SetSizer(sizer);

	m_buttonCancel->SetFocus();
	m_buttonOkay->Enable(false);
}


void NewSimulationDialog::OnOpenButtonPressed(wxCommandEvent& e)
{
	wxFileDialog* openDialog = new wxFileDialog(this,
		"Open Configuration File", wxEmptyString, wxEmptyString, 
		"Simulation Config files (*.txt)|*.txt",
		wxFD_OPEN, wxDefaultPosition);
 
	if (openDialog->ShowModal() == wxID_OK)
	{
		std::string path = (std::string) openDialog->GetPath();
		
		// Update the path text box.
		m_textConfigFilePath->SetValue(path);

		// Load the config file.
		std::string errorMessage;
		bool success = m_configFileLoader.LoadConfigFile(
			path, m_config, errorMessage);

		// Check if the file was loaded successfully.
		if (success)
		{
			m_textLoadMessage->SetValue("Configuration file loaded successfully!");
			m_buttonOkay->Enable(true);
		}
		else
		{
			m_textLoadMessage->SetValue(errorMessage);
			m_buttonOkay->Enable(false);
		}
	}
 
	openDialog->Destroy();
}

