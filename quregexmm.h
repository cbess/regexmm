
#ifndef _QUREGEXMM_H_
#define _QUREGEXMM_H_

/**
 * @short Application Main Window
 * @author Quantum Quinn <cbess@QuantumQuinn.com>
 * @version 0.1
 */

class QuRegexStorageDia;
class wxSpinCtrl;
class wxSpinEvent;

#if defined(wxPCRE_BUILD)
	class wxPCRE;
#else
	class wxRegEx;
#endif

class QuRegExmmapp : public wxApp
{
	public:
		virtual bool OnInit();
};

class QuRegExmmFrame : public wxFrame
{	
	public:
		QuRegExmmFrame();
		void InitializeFrame();		
		void SetRegex( wxString str );
		
	private:
		void CreateControls();
		
		wxTextCtrl *txtRegex;
		wxTextCtrl *txtSource;
		wxCheckBox *chkMatch;
		wxCheckBox *chkIgnoreCase;
		wxCheckBox *chkMultiline;
		wxSpinCtrl *udSubexpression;
		wxTextCtrl *txtSubexpression;
		QuRegexStorageDia * regexDia;
		
#if defined(wxPCRE_BUILD)
		wxPCRE *mRegex;
#else
		wxRegEx *mRegex;
#endif	
		
		void OnQuit( wxCommandEvent& event );
		void OnAbout( wxCommandEvent& event );
		void OnBtMatchClick( wxCommandEvent & evt );
		void FindMatch();
		void txtRegex_OnTextChange( wxCommandEvent & evt );
 		void OnRegexStorage( wxCommandEvent & evt );	
		void txtRegex_KeyDown( wxKeyEvent & evt );
		void OnSubexpressionChanged( wxSpinEvent& evt );
		void OnLoadSession( wxCommandEvent& evt );
		void OnSaveSession( wxCommandEvent& evt );
		
	private:
		DECLARE_EVENT_TABLE()
};

enum
{
	Menu_File_Quit = 100,
	Menu_File_About,
	Menu_File_AppName,
	Menu_File_RegexStorage,
	Menu_Session_SaveSession,
	Menu_Session_LoadSession
};

#endif // _QUREGEXMM_H_
