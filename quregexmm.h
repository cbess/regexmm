/*
	Quantum Quinn (Christopher Bess)
	<cbess@quantumquinn.com>
	
	Copyright (C) 2006, 2007 Christopher Bess of Quantum Quinn
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
 
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
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
		void CreateSourceControl(); // setup the rich text control
		void CreateMenuBar();
		void NextHighlightStyle(); // moves to the next highlighted match color
				
		// UI vars
		wxTextCtrl *txtRegex;
		wxRichTextCtrl *txtSource;
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
		wxTextAttr mDefaultTextAttr;
		wxColour mCurrentHighlightColor;
		int mCurrentHighlightColorIndex;
		
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
	Menu_File_About = wxID_ABOUT,
	Menu_File_Quit = wxID_EXIT,
	Menu_File_AppName = 100,
	Menu_File_RegexStorage,
	Menu_Session_SaveSession,
	Menu_Session_LoadSession,
	Menu_Regex_wxPCRE,
	Menu_Regex_wxRegEx
};

#endif // _QUREGEXMM_H_
