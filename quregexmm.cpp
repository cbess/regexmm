#include <wx/wx.h>
#include <wx/intl.h>
#include "resource.h"
#include "quregexmm.h"
#include <wx/xrc/xmlres.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include "quregexstorage.h"
#include <wx/aboutdlg.h>
#include <wx/spinctrl.h>

#define APP_NAME "QuRegExmm"
#define STAT_TEXT APP_NAME" :: Quantum Quinn"

BEGIN_EVENT_TABLE( QuRegExmmFrame, wxFrame )
	EVT_MENU( Menu_File_Quit, QuRegExmmFrame::OnQuit )
	EVT_MENU( Menu_File_About, QuRegExmmFrame::OnAbout )
	EVT_BUTTON( XRCID("BT_Match"), QuRegExmmFrame::OnBtMatchClick )
	EVT_TEXT( XRCID("TXT_Regex"), QuRegExmmFrame::txtRegex_OnTextChange )
	EVT_MENU( Menu_File_RegexStorage, QuRegExmmFrame::OnRegexStorage )
	EVT_SPINCTRL(XRCID("UD_Subexpression"), QuRegExmmFrame::OnSubexpressionChanged)
	EVT_MENU(Menu_Session_SaveSession, QuRegExmmFrame::OnSaveSession)
	EVT_MENU(Menu_Session_LoadSession, QuRegExmmFrame::OnLoadSession)
END_EVENT_TABLE()

IMPLEMENT_APP(QuRegExmmapp)

class wxXmlResource;

bool QuRegExmmapp::OnInit()
{	
	wxXmlResource::Get()->InitAllHandlers();
	InitXmlResource();
	
	// wxXmlResource::Get()->Load("QuRegExmm.xrc");

	QuRegExmmFrame *frame = new QuRegExmmFrame;
	
	wxXmlResource::Get()->LoadFrame( frame, NULL, wxT("FRM_Main") );
	
	frame->InitializeFrame();

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
	CreateControls();
	
	SetStatusText( wxT( STAT_TEXT ), 1 );
} // end

void QuRegExmmFrame::CreateControls()
{		
	regexDia = new QuRegexStorageDia;
	wxXmlResource::Get()->LoadDialog( regexDia, this, wxT("DIA_Regex") );
	regexDia->InitializeDialog( this );
	
	wxMenu *menuFile = new wxMenu;
	
	menuFile->Append( Menu_File_AppName, _(STAT_TEXT) );
	menuFile->AppendSeparator(); // add items below		
	menuFile->Append( Menu_File_RegexStorage, wxT("&RegExmm Storage") );
	menuFile->AppendSeparator();
	menuFile->Append( Menu_File_About, _("&About...") );
	menuFile->AppendSeparator();
	menuFile->Append( Menu_File_Quit, _("E&xit") );
	
	wxMenu *sessionMenu = new wxMenu;
	sessionMenu->Append(Menu_Session_SaveSession, _("Save Session"));
	sessionMenu->AppendSeparator();
	sessionMenu->Append(Menu_Session_LoadSession, _("Load Session"));
	
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _("&File") );
	menuBar->Append(sessionMenu, _("&Session"));
	
	SetMenuBar( menuBar );
	
	menuBar->Enable( Menu_File_AppName, false );
	
	txtRegex = XRCCTRL(*this, "TXT_Regex", wxTextCtrl);
	txtSource = XRCCTRL(*this, "TXT_Source", wxTextCtrl);
	chkMatch = XRCCTRL(*this, "CHK_Match", wxCheckBox);
	txtSubexpression = XRCCTRL(*this, "TXT_Subexpression", wxTextCtrl);
	udSubexpression = XRCCTRL(*this, "UD_Subexpression", wxSpinCtrl);
	chkIgnoreCase = XRCCTRL(*this, "CHK_IgnoreCase", wxCheckBox);
	chkMultiline = XRCCTRL(*this, "CHK_Multiline", wxCheckBox);
	
	// set the default style for subexpression text
	txtSubexpression->SetDefaultStyle(wxTextAttr(wxColour(wxT("YELLOW")), *wxBLACK));
	
	// create initial size
	wxSize initialSize = wxSize(500, 400);
	
	// hard code frame size
	SetSize(initialSize);
	
	// set the min size to the current size
	SetMinSize(initialSize);	
	
	// set up the key down events
	txtRegex->Connect( wxID_ANY,
					   wxEVT_KEY_DOWN, wxKeyEventHandler(QuRegExmmFrame::txtRegex_KeyDown), NULL, this );
	
	// create reusable regex object for FindMatch(...)
	mRegex = new wxRegEx;	

	// create two status bar fields
	CreateStatusBar(2);	
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

void QuRegExmmFrame::OnQuit( wxCommandEvent& WXUNUSED( event ) )
{ // menu->Quit
	Close(TRUE);
}

void QuRegExmmFrame::OnAbout( wxCommandEvent& WXUNUSED( event ) )
{	
	// create about dialog box info
	wxAboutDialogInfo info;
	info.SetName(_(APP_NAME));
	info.SetVersion(_("0.2 Beta"));
	info.SetDescription(_("Free multi-platform regular expression matching application."));
	info.SetCopyright(_T("(C) 2007 Quantum Quinn"));
	info.SetWebSite(wxT("QuRegExmm Website"), wxT("http://QuantumQuinn.com"));
	info.AddDeveloper(wxT("C. Bess of Quantum Quinn"));
			
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
	// get interactive values
	wxString pattern( txtRegex->GetValue() );
	wxString source( txtSource->GetValue() );
	int regexStyle;
	
	// set the default style for the text ctrl
	txtSource->SetStyle(0, source.Length(), wxTextAttr(*wxBLACK,*wxWHITE));
	wxTextAttr ta;
	ta.SetFlags(wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_BACKGROUND_COLOUR);
	ta.SetTextColour(*wxBLUE);
	ta.SetBackgroundColour(wxColour(wxT("YELLOW")));
	
	// reset the selection
	txtSource->SetSelection(0,0);
	
	// reset the subexpression ctrls
	txtSubexpression->Clear();
	udSubexpression->SetValue(0);
	
	size_t start = 0, len = 0;	
	
	// if the required fields are blank
	if ( pattern.IsEmpty() || source.IsEmpty() )
	{
		SetStatusText( _("EMPTY") );
		return;
	} // end
	
	wxLogNull * logNull = new wxLogNull; // suppress wxRegEx msgs

	// set the style var
	regexStyle = wxRE_DEFAULT | wxRE_ADVANCED;

	if ( chkIgnoreCase->IsChecked() )
		regexStyle |= wxRE_ICASE;
	
	if ( chkMultiline->IsChecked() )
		regexStyle |= wxRE_NEWLINE;
	
	// compile the regex object	
	if ( mRegex->Compile(pattern, regexStyle) )
	{
		size_t count = 0;
		size_t tmpStrLen = source.length(); // length won't change
		
		while ( mRegex->Matches(source, regexStyle, tmpStrLen) )
		{								
			mRegex->GetMatch(&start, &len, 0);
						
			// if using a look around (look ahead/behind assertion)
			if ( len == 0 )
				break;
			
			long to = start+len;
			
			txtSource->SetStyle(start, to, ta);
			
			// this prevents it from matching the same thing
			for ( size_t index = start, num = 1 ; ; ++index, ++num )
			{
				// replace char at index position
				source.SetChar(index, ' ');
				
				// if it has replaced the len of chars from the start index
				if ( num == len ) break;
			} // end FOR
			
			// increment match count
			++count;
			
			/* - to prevent infinite loops its safe to assume that
			* there will never be more matches than actual chars available
			*/
			if ( count == tmpStrLen )
				break;						
		} // end WHILE
		
		// subexpression count
		int nSubCount = mRegex->GetMatchCount()-1;
		
		if ( count > 0 )
		{			
			// set the status text
			SetStatusText( wxString::Format( 
					_("%i %s found for `%s` (%i)"), 
					 count, 
					 (count == 1 ? _("match") : _("matches")),  
					 pattern.c_str(),
					nSubCount) );
			
			// set the max value for the spin ctrl
			udSubexpression->SetRange(0, nSubCount);
		} // end IF
		else
		{
			// set status text
			SetStatusText(wxString::Format(
					_("No match found for `%s` (%i)"),
			pattern.c_str(), 
			nSubCount));
		} // end ELSE
		
		delete logNull; // restore msg notification
	} // end IF IsValid
	else
	{
		// inform user of the error
		SetStatusText(_("Check expression."));
	} // end ELSE
	
#if defined(__WXMAC__)
	// draw the highlights
	/*
	 [Built on wxMac-2.8.0, OS X 10.4]
	 - does not update style via SetStyle,
	 unless the text ctrl receives focus
	 */
	txtSource->SetFocus();
	txtRegex->SetFocus();
#endif
} // end

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
	txtRegex->SetValue( str );
} // end

void QuRegExmmFrame::OnSubexpressionChanged( wxSpinEvent & evt )
{
	// determine if the match has subexpressions
	int nSubs = mRegex->GetMatchCount()-1;
	int nSelSub = evt.GetSelection();
	
	if ( (nSubs <= 0) || (!mRegex->IsValid()) )
		return;
	
	// prevent out of range
	if ( nSelSub > nSubs )
		nSelSub = nSubs;
	
	// get the subexpression
	wxString subexpression = mRegex->GetMatch(txtSource->GetValue(), nSelSub);
	
	// clear the previous
	txtSubexpression->Clear();
	
	// set the text box value to the value of the corresponding subexpression
	txtSubexpression->AppendText(subexpression);
	
	// set focus
	txtSubexpression->SetFocus();
	udSubexpression->SetFocus();
}

void QuRegExmmFrame::OnSaveSession( wxCommandEvent & WXUNUSED(evt) )
{
	wxString filePath = wxFileSelector(_("Save session as..."),
									   wxT(""), wxT(""),
									   wxT(""),
									   wxT("QuRegExmm Session files (*.quregex)|*.quregex"),
									   wxSAVE, this);
	if ( filePath.empty() )
		return;
	
	wxFileOutputStream file(filePath);
	wxDataOutputStream store( file );
			
	// store the regex
	store.WriteString(txtRegex->GetValue());
	
	// store the source text
	store.WriteString(txtSource->GetValue());
	
	file.Close();	
}

void QuRegExmmFrame::OnLoadSession( wxCommandEvent & WXUNUSED(evt) )
{
	wxString filePath = wxFileSelector(_("Load session from..."));
	
	if ( filePath.empty() )
		return;
	
	if ( wxFile::Exists(filePath) )
	{
		wxFileInputStream file(filePath);
		wxDataInputStream store( file );
				
		// restore the previous values
		txtRegex->SetValue(store.ReadString());
		txtSource->SetValue(store.ReadString());
	} // end IF
}
