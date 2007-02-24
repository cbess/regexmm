
#ifndef _QUREGEXMM_H_
#define _QUREGEXMM_H_

/**
 * @short Application Main Window
 * @author Quantum Quinn <cbess@QuantumQuinn.com>
 * @version 0.1
 */

enum
{ // these are only wxRegEx surrogates
    // use extended regex syntax
	wxPCRE_EXTENDED = 0, // not implemented
    
    // use advanced RE syntax (built-in regex only)
	wxPCRE_ADVANCED = 0, // not implemented

    // use basic RE syntax
	wxPCRE_BASIC = 0, // not implemented

    // only check match, don't set back references
	wxPCRE_NOSUB = 0, // not implemented
	
    // default flags
	wxPCRE_DEFAULT = wxRE_EXTENDED,
	
    // ignore case in match
	wxPCRE_ICASE = PCRE_CASELESS, // compile time only

    // if not set, treat '\n' as an ordinary character, otherwise it is
    // special: it is not matched by '.' and '^' and '$' always match
    // after/before it regardless of the setting of wxRE_NOT[BE]OL
	wxPCRE_NEWLINE = PCRE_MULTILINE, // compile time only
	
    // '^' doesn't match at the start of line
	wxPCRE_NOTBOL = PCRE_NOTBOL, // match time only

    // '$' doesn't match at the end of line
	wxPCRE_NOTEOL = PCRE_NOTEOL // match time only
};

class QuRegexStorageDia;
class wxSpinCtrl;
class wxSpinEvent;
class wxPCRE;
class wxRegEx;

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
		
		// events
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
