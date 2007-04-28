
#ifndef _QUREGEXMM_H_
#define _QUREGEXMM_H_

/**
 * @short Application Main Window
 * @author Quantum Quinn <cbess@QuantumQuinn.com>
 * @version 0.7
 */

class QuRegexStorageDia;
class wxSpinCtrl;
class wxSpinEvent;
class wxPCRE;
class wxRegEx;

const int PCRE_DEFAULT = 0;

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
		
		inline bool UsePCRELib() const
		{ return mUsePCRELib; }
		
	private:
		void CreateControls();
		void CreateMenuBar();
		
		// UI vars
		wxTextCtrl *txtRegex;
		wxTextCtrl *txtSource;
		wxCheckBox *chkMatch;
		wxCheckBox *chkIgnoreCase;
		wxCheckBox *chkMultiline;
		wxSpinCtrl *udSubexpression;
		wxTextCtrl *txtSubexpression;
		QuRegexStorageDia * regexDia;
		
		// regular expression lib vars
		wxPCRE *mPCRE;
		wxRegEx *mRegEx;
		
		// vars
		bool mUsePCRELib;
		wxTextAttr mTextAttr;
		
		// match methods
		void FindMatch();
		bool wxPCRE_Match( const wxString& pattern, wxString& source, int flags, size_t * count, int * subCount );
		bool wxRegEx_Match( const wxString& pattern, wxString& source, int flags, size_t * count, int * subCount );
		
		// event handlers
		void OnQuit( wxCommandEvent& event );
		void OnAbout( wxCommandEvent& event );
		void OnBtMatchClick( wxCommandEvent & evt );		
		void txtRegex_OnTextChange( wxCommandEvent & evt );
 		void OnRegexStorage( wxCommandEvent & evt );	
		void txtRegex_KeyDown( wxKeyEvent & evt );
		void OnSubexpressionChanged( wxSpinEvent& evt );
		void OnLoadSession( wxCommandEvent& evt );
		void OnSaveSession( wxCommandEvent& evt );
		void OnRegexLibSelect( wxCommandEvent& evt );
		
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
	Menu_Session_LoadSession,
	Menu_Regex_wxPCRE,
	Menu_Regex_wxRegEx
};

#endif // _QUREGEXMM_H_
