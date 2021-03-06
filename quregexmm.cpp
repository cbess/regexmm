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

#include <wx/wx.h>
#include <wx/intl.h>
#include "resource.h"

#include <pcre.h>
#include "wxpcre.h"
#include <wx/regex.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/splitter.h>
#include "quregexmm.h"
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include "quregexstorage.h"
#include <wx/aboutdlg.h>
#include <wx/spinctrl.h>

#define APP_NAME "QuRegExmm"
#define STAT_TEXT APP_NAME" :: Quantum Quinn"
#ifdef __WXMAC__
#define DEFAULT_FONT_SIZE 13
#else
#define DEFAULT_FONT_SIZE 11
#endif
#define DATA_VERSION 2
#define DATA_VERSION_PREFIX "quregex-data-version="

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
	#include "appicon.xpm"
#endif

BEGIN_EVENT_TABLE( QuRegExmmFrame, wxFrame )
	EVT_MENU( Menu_File_Quit, QuRegExmmFrame::OnQuit )
	EVT_MENU( Menu_File_About, QuRegExmmFrame::OnAbout )
	EVT_BUTTON( XRCID("BT_Match"), QuRegExmmFrame::OnBtMatchClick )
	EVT_TEXT( XRCID("TXT_Regex"), QuRegExmmFrame::txtRegex_OnTextChange )
	EVT_MENU( Menu_File_RegexStorage, QuRegExmmFrame::OnRegexStorage )
	EVT_SPINCTRL(XRCID("UD_Subexpression"), QuRegExmmFrame::OnSubexpressionChanged)
	EVT_MENU(Menu_Session_SaveSession, QuRegExmmFrame::OnSaveSession)
	EVT_MENU(Menu_Session_LoadSession, QuRegExmmFrame::OnLoadSession)
	EVT_MENU(Menu_Regex_wxPCRE, QuRegExmmFrame::OnRegexLibSelect)
	EVT_MENU(Menu_Regex_wxRegEx, QuRegExmmFrame::OnRegexLibSelect)
END_EVENT_TABLE()

IMPLEMENT_APP(QuRegExmmapp)

class wxXmlResource;

bool QuRegExmmapp::OnInit()
{	
	wxXmlResource::Get()->InitAllHandlers();
	InitXmlResource();

	QuRegExmmFrame *frame = new QuRegExmmFrame;
	
	wxXmlResource::Get()->LoadFrame(frame, NULL, wxT("FRM_Main"));
	
	frame->InitializeFrame();
	frame->SetTitle(wxT(APP_NAME" (PCRE)")); // default title
	frame->Show(TRUE);

	SetTopWindow(frame);
	
	return TRUE;
} 

QuRegExmmFrame::QuRegExmmFrame()
	: wxFrame()
{
} // end

void QuRegExmmFrame::InitializeFrame()
{	
    // set the frame icon
	SetIcon(wxICON(appicon));

	mUsePCRELib = true;
	
	CreateControls();
	
	// create initial size
	wxSize initialSize = wxSize(450, 400);
	
	// hard code frame size
	SetSize(initialSize);
	
	// set the min size to the current size
	SetMinSize(initialSize);	
	
	// set up the key down events
	txtRegex->Connect( txtRegex->GetId(),
					   wxEVT_KEY_DOWN, wxKeyEventHandler(QuRegExmmFrame::txtRegex_KeyDown), NULL, this );
	
	// create reusable regex objects for FindMatch(...)
	mPCRE = new wxPCRE;
	mRegEx = new wxRegEx;
	
	// set the default color	
	mYellowColour = wxColour(wxT("YELLOW"));
	mBrownColour = wxColour(wxT("BROWN"));
	mGoldColour = wxColour(wxT("GOLD")); // used by mac
	mCurrentHighlightColorIndex = 1;

	SetStatusText(wxT(STAT_TEXT), 1);
} // end

void QuRegExmmFrame::CreateControls()
{		
	regexDia = new QuRegexStorageDia;
	wxXmlResource::Get()->LoadDialog( regexDia, this, wxT("DIA_Regex") );
	regexDia->InitializeDialog( this );
	
	CreateMenuBar();
	
	txtRegex = XRCCTRL(*this, "TXT_Regex", wxTextCtrl);
	txtSource = XRCCTRL(*this, "TXT_Source", wxTextCtrl);
	chkMatch = XRCCTRL(*this, "CHK_Match", wxCheckBox);
	txtSubexpression = XRCCTRL(*this, "TXT_Subexpression", wxTextCtrl);
	udSubexpression = XRCCTRL(*this, "UD_Subexpression", wxSpinCtrl);
	chkIgnoreCase = XRCCTRL(*this, "CHK_IgnoreCase", wxCheckBox);
	chkMultiline = XRCCTRL(*this, "CHK_Multiline", wxCheckBox);
	splMain = XRCCTRL(*this, "SPL_Main", wxSplitterWindow);
	
	// set the default text attr
	mDefaultTextAttr = wxTextAttr(*wxBLACK, *wxWHITE);
	mDefaultTextAttr.SetFlags(wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_BACKGROUND_COLOUR|wxTEXT_ATTR_FONT_WEIGHT);
	mDefaultTextAttr.SetFont(wxFont(DEFAULT_FONT_SIZE, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	
	// setup the highlighting text attr
	mTextAttr.SetFlags(wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_BACKGROUND_COLOUR|wxTEXT_ATTR_FONT_WEIGHT);
	mTextAttr.SetTextColour(*wxBLACK);
	mTextAttr.SetFont(wxFont(DEFAULT_FONT_SIZE, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));	
			
#ifndef __WXMAC__
	splMain->SetSashPosition(95);
#endif

	// create two status bar fields
	CreateStatusBar(2);	
	
	// adjust the status bar sizes
	int widths[2];
	widths[0] = -1; // (variable width) the left most status area
	widths[1] = 230; // (fixed width) the right most status area (app title)
	SetStatusWidths(2, widths);
	
	// set focus to source
	txtSource->SetFocus();
	txtSource->SetDefaultStyle(mDefaultTextAttr);
}

void QuRegExmmFrame::CreateMenuBar()
{
	wxMenu *muFile = new wxMenu;
	
#ifndef __WXMAC__
	muFile->Append( Menu_File_AppName, _(STAT_TEXT) );
	muFile->AppendSeparator(); // add items below		
#endif
	muFile->Append( Menu_File_RegexStorage, wxT("&RegEx Storage"), _("Manage stored regular expressions.") );
	muFile->AppendSeparator();
	muFile->Append( Menu_File_About, _("&About..."), _("View "APP_NAME" credits.") );
	muFile->AppendSeparator();
	muFile->Append( Menu_File_Quit, _("E&xit") );
	
	wxMenu *muRegexLib = new wxMenu;
	muRegexLib->AppendRadioItem(Menu_Regex_wxPCRE, wxT("PCRE"), _("Use the PCRE library."));
	muRegexLib->AppendRadioItem(Menu_Regex_wxRegEx, wxT("wxRegEx"), _("Use the wxRegEx library."));
	
	wxMenu *muSession = new wxMenu;
	muSession->Append(Menu_Session_SaveSession, _("Save Session"), _("Save the current session."));
	muSession->AppendSeparator();
	muSession->Append(Menu_Session_LoadSession, _("Load Session"), _("Load a previously saved session."));
	muSession->AppendSeparator();
	muSession->AppendSubMenu(muRegexLib, _("RegEx Library"), _("Select the regular expression library."));
	
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(muFile, _("&File"));
	menuBar->Append(muSession, _("&Session"));
	
	SetMenuBar( menuBar );
	
	menuBar->Enable( Menu_File_AppName, false );
}

void QuRegExmmFrame::txtRegex_KeyDown( wxKeyEvent &evt )
{
	switch ( evt.GetKeyCode() )
	{
		case WXK_RETURN:
			FindMatch();
			return;
	} // end SWITCH
	
	// tell app to handle the key event
	evt.Skip();
}

void QuRegExmmFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{ // menu->Quit
	
	if ( mRegEx )
		delete mRegEx;
	
	if ( mPCRE )
		delete mPCRE;
	
	Close(TRUE);
}

void QuRegExmmFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{	
	// create about dialog box info
	wxAboutDialogInfo info;
	info.SetName(wxT(APP_NAME));
	info.SetVersion(_("0.7.1b"));
	info.SetDescription(_("Open source multi-platform regular expression matching application. Supports both PCRE and wxRegEx."));
	info.SetCopyright(wxT("(C) 2007 Quantum Quinn"));
	info.SetWebSite(wxT("http://quregexmm.sf.net"), wxT("QuRegExmm Homepage"));
	info.AddDeveloper(wxT("Christopher Bess of Quantum Quinn"));
			
	// show the about info
	wxAboutBox(info);
}

void QuRegExmmFrame::OnBtMatchClick( wxCommandEvent & WXUNUSED(evt) )
{
	FindMatch();
	txtSource->SetFocus();
} // end

void QuRegExmmFrame::FindMatch()
{
	bool useDefaultLib = UsePCRELib();
		
	// get interactive values
	wxString pattern( txtRegex->GetValue() );
	wxString source( txtSource->GetValue() );
	wxString statusText;
	int regexStyle = 0;
	bool isValid = false;
	size_t count = 0;
	int nSubCount = 0;
	
	// set the default style for the text ctrl
	txtSource->SetStyle(0, source.Length(), mDefaultTextAttr);
	
	// reset the selection
	txtSource->SetSelection(0, 0);
	
	// reset the subexpression ctrls
	txtSubexpression->Clear();
	udSubexpression->SetValue(0);		
	
	wxLogNull * logNull = new wxLogNull; // suppress msgs

	// if the required fields are blank
	if ( pattern.IsEmpty() )
	{
		statusText = _("No pattern.");
		goto end;
	}

	// set the style var
	if ( useDefaultLib )
		regexStyle = PCRE_DEFAULT;
	else
		regexStyle = wxRE_DEFAULT | wxRE_ADVANCED;

	// user wants to ignore case
	if ( chkIgnoreCase->IsChecked() )
	{
		if ( useDefaultLib )
			regexStyle |= PCRE_CASELESS;
		else
			regexStyle |= wxRE_ICASE;
	} // end IF	
	
	// use wants multi-line pattern
	if ( chkMultiline->IsChecked() )
	{
		if ( useDefaultLib )
			regexStyle |= PCRE_MULTILINE;
		else
			regexStyle |= wxRE_NEWLINE;
	} // end IF
		
	// perform the regex operation (highlight matches)
	if ( useDefaultLib )
	{
		isValid = wxPCRE_Match(pattern, source, regexStyle, &count, &nSubCount);
	}
	else
	{
		isValid = wxRegEx_Match(pattern, source, regexStyle, &count, &nSubCount);
	}
	
	// if the pattern was valid
	if ( isValid )
	{				
		if ( count > 0 )
		{			
			// set the status text
			statusText = wxString::Format( 
					wxT("%i %s for `%s` (%i)"), 
					 (int)count, 
					 (count == 1 ? wxT("match") : wxT("matches")),  
					 pattern.c_str(),
					nSubCount);
			
			// set the max value for the spin ctrl
			udSubexpression->SetRange(0, nSubCount);
						
			if ( nSubCount >= 0 )
			{
				// restore wxPCRE subexpr
				mPCRE->Matches(source, regexStyle);
				
				wxString subexpr;
				
				// set the default subexpr value
				if ( useDefaultLib )
					subexpr = mPCRE->GetMatch(source);
				else
					subexpr = mRegEx->GetMatch(source);
				
				// set the subexpr text value
				txtSubexpression->SetValue(subexpr);
			} // end IF (nSubCount)
		} // end IF
		else
		{
			// set status text
			statusText = wxString::Format(
					wxT("No match for `%s` (%i)"),
			pattern.c_str(), 
			nSubCount);
		} // end ELSE
		
	} // end IF isValid
	else
	{
		// inform user of the error
		if (useDefaultLib)
			statusText = mPCRE->LastError();
		else
			statusText = _("Check expression.");
	} // end ELSE
	
end:

	delete logNull; // restore msg notification

	// set the status text
	SetStatusText(statusText);
	
#ifdef __WXMAC__
	/*
	 [Built on wxMac-2.8.0, OS X 10.4]
	 - does not update style via SetStyle,
	 unless the wxTextCtrl receives focus
	 */
	txtSource->SetFocus();
	txtRegex->SetFocus();
#endif
} // end

bool QuRegExmmFrame::wxPCRE_Match
( const wxString& pattern, wxString& source, int flags, size_t * matchCount, int * subCount )
{
	if ( !mPCRE->Compile(pattern, flags) )
		return false;
	
	size_t count = 0;
	size_t tmpStrLen = source.length(); // length won't change
	int nOffset = 0;
	
	// iterate over all possible matches
	while ( mPCRE->RegExMatches(source, 0, nOffset) )
	{			
		size_t start = 0, len = 0;	
		
		// get the match coords			
		start = mPCRE->GetMatchStart();
		len = mPCRE->GetMatchLength();
					
		// if using a look around (look ahead/behind assertion)
		if ( len == 0 )
			break;
				
		// set next mark past this match
		nOffset = start+len;	
		
		// highlight the match
		NextHighlightStyle();
		txtSource->SetStyle(start, nOffset, mTextAttr);
		
		// store the sub match count
		if ( count == 0 )
			*subCount = mPCRE->GetMatchCount()-1;			
		
		// increment match count
		++count;
		
		/* - to prevent infinite loops its safe to assume that
		* there will never be more matches than actual chars available
		*/
		if ( count == tmpStrLen )
			break;						
		
	} // end WHILE
	
	// set the matches count
	*matchCount = count;
	
	return true;
}

bool QuRegExmmFrame::wxRegEx_Match
( const wxString& pattern, wxString& source, int flags, size_t * matchCount, int * subCount )
{	
	if ( !mRegEx->Compile(pattern, flags) )
		return false;
	
	size_t count = 0;
	size_t tmpStrLen = source.length(); // length won't change
	
	// iterate over all possible matches
	while ( mRegEx->Matches(source) )
	{			
		size_t start = 0, len = 0;	
				
		// get the match coords			
		mRegEx->GetMatch(&start, &len);
					
		// if using a look around (look ahead/behind assertion)
		if ( len == 0 )
			break;
		
		long to = start+len;
		
		// highlight the match
		NextHighlightStyle();
		txtSource->SetStyle(start, to, mTextAttr);
		
		// this prevents it from matching the same thing
		for ( size_t index = start, num = 1 ; ; ++index, ++num )
		{
			// replace char at index position
			source.SetChar(index, ' ');
			
			// if it has replaced the len of chars from the start index
			if ( num == len ) 
				break;
		} // end FOR
		
		// store the valid subexpr count
		if ( count == 0 )			
			*subCount = mRegEx->GetMatchCount()-1;
		
		// increment match count
		++count;
		
		/* - to prevent infinite loops its safe to assume that
		* there will never be more matches than actual chars available
		*/
		if ( count == tmpStrLen )
			break;		
		
	} // end WHILE
	
	// transfer count value
	*matchCount = count;
	
	return true;
}

void QuRegExmmFrame::txtRegex_OnTextChange( wxCommandEvent& WXUNUSED(evt) )
{
	if ( chkMatch->IsChecked() )
	{
		FindMatch();
	} // end IF
} // end

void QuRegExmmFrame::OnRegexStorage( wxCommandEvent & WXUNUSED(evt) )
{ // menu->regex storage -> click
	if ( ! regexDia->IsShown() )
		regexDia->Show(TRUE);
} // end

void QuRegExmmFrame::SetRegex( wxString str )
{
	txtRegex->SetValue(str);
} // end

void QuRegExmmFrame::OnSubexpressionChanged( wxSpinEvent & evt )
{
	// determine the lib
	bool useDefaultLib = UsePCRELib();
	
	// determine if the match has subexpressions
	int nSubs = (useDefaultLib ? mPCRE->GetMatchCount() : mRegEx->GetMatchCount())-1;
	int nSelSub = evt.GetSelection();
	bool isValid = (useDefaultLib ? mPCRE->IsValid() : mRegEx->IsValid());
	
	if ( (nSubs <= 0) || (!isValid) )
		return;
	
	// store source text
	wxString source = txtSource->GetValue();
	
	// prevent out of range
	if ( nSelSub > nSubs )
		nSelSub = nSubs;
	
	// get the subexpression
	wxString subexpression;
	
	if ( useDefaultLib )
		subexpression = mPCRE->GetMatch(source, nSelSub);
	else
		subexpression = mRegEx->GetMatch(source, nSelSub);
	
	// set the text box value to the value of the corresponding subexpression
	txtSubexpression->SetValue(subexpression);
	
	// set focus
	txtSubexpression->SetFocus();
	udSubexpression->SetFocus();
}

void QuRegExmmFrame::OnSaveSession( wxCommandEvent & WXUNUSED(evt) )
{
	wxString filePath = wxFileSelector(_("Save session as..."),
									   wxEmptyString, wxEmptyString,
									   wxEmptyString,
									   wxT("QuRegExmm Session files (*.quregex)|*.quregex"),
									   wxSAVE, this);
	if ( filePath.empty() )
		return;

	if ( !filePath.EndsWith(wxT(".quregex")) )
		filePath += wxT(".quregex");
	
	wxFileOutputStream file(filePath);
	wxDataOutputStream store( file );
	wxString verString; // the data version
	verString << DATA_VERSION_PREFIX << DATA_VERSION;

	/** 9/9/2007
	 * - I'm using a string instead of a numerical store, because
	 * I forgot to add versioning earlier (backwards compatibility workaround)
	 */
	// write the data version
	store.WriteString(verString);

	// store the regex
	store.WriteString(txtRegex->GetValue());
	
	// store the source text
	store.WriteString(txtSource->GetValue());
	
	file.Close();	
}

void QuRegExmmFrame::OnLoadSession( wxCommandEvent & WXUNUSED(evt) )
{	
	wxString filePath = wxFileSelector(_("Load session from..."),
									   wxEmptyString, wxEmptyString,
									   wxEmptyString,
									   wxT("QuRegExmm Session files (*.quregex)|*.quregex"),
									   wxOPEN, this);	
	if ( filePath.empty() )
		return;
	
	if ( wxFile::Exists(filePath) )
	{
		wxFileInputStream file(filePath);
		wxDataInputStream store( file );
		wxString firstString;
		wxString verString;
		long ver = 1;

		// read the data version
		firstString = store.ReadString();

		if (firstString.StartsWith(wxT(DATA_VERSION_PREFIX), &verString))
		{
			verString.ToLong(&ver);
		}

		// load specific data version
		if (ver == 1)
		{
			// restore the previous values
			txtRegex->SetValue(firstString);
			txtSource->SetValue(store.ReadString());
		}
		
		if (ver >= 2)
		{
			txtRegex->SetValue(store.ReadString());
			txtSource->SetValue(store.ReadString());
		}

		if (ver >= 3)
		{
			// future use
		}

		// execute the match making
		this->FindMatch();
	} // end IF
}

void QuRegExmmFrame::OnRegexLibSelect( wxCommandEvent& evt )
{
	switch ( evt.GetId() )
	{
		case Menu_Regex_wxPCRE:
			this->SetTitle(wxT(APP_NAME" (PCRE)"));
			mUsePCRELib = true;
			break;
			
		case Menu_Regex_wxRegEx:
			this->SetTitle(wxT(APP_NAME" (wxRegEx)"));
			mUsePCRELib = false;
			break;
	} // end SWITCH
	
	// execute the match operation
	FindMatch();
}

#if defined(__WXMAC__)

void QuRegExmmFrame::NextHighlightStyle()
{		
	wxColour color;
	
again: // used to start the color choice again
		
	switch ( mCurrentHighlightColorIndex )
	{
		case 0:
			color = *wxBLUE;
			break;

		case 1:			
			color = *wxRED;
			break;
			
		case 2:
			color = mGoldColour;
			break;
			
		default:
			mCurrentHighlightColorIndex = 0;
			goto again;
	} // end SWITCH
	
	// set the next color value
	mTextAttr.SetTextColour(color);
	
	// increment to next color index
	++mCurrentHighlightColorIndex;
}

// else - is other platforms (Win32, Linux)
#else

void QuRegExmmFrame::NextHighlightStyle()
{	
	wxColour color;
	
again: // used to start the color choice again
		
	switch ( mCurrentHighlightColorIndex )
	{
		case 0:
			color = *wxBLUE;
			break;

		case 1:			
			color = *wxRED;
			break;
			
		case 2:
			color = mBrownColour;
			break;
			
		default:
			mCurrentHighlightColorIndex = 0;
			goto again;
	} // end SWITCH
	
	// set the next color value
	mTextAttr.SetBackgroundColour(mYellowColour);
	mTextAttr.SetTextColour(color);
	
	// increment to next color index
	++mCurrentHighlightColorIndex;
}

#endif
