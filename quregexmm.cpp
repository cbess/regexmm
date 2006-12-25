#include <wx/wx.h>
#include <wx/intl.h>
#include "quregexmm.h"
#include <wx/xrc/xmlres.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include "quregexstorage.h"

#define APP_NAME "QuRegExmm"
#define STAT_TEXT APP_NAME" :: Quantum Quinn"

BEGIN_EVENT_TABLE( QuRegExmmFrame, wxFrame )
	EVT_MENU( Menu_File_Quit, QuRegExmmFrame::OnQuit )
	EVT_MENU( Menu_File_About, QuRegExmmFrame::OnAbout )
	EVT_BUTTON( XRCID("BT_Match"), QuRegExmmFrame::OnBtMatchClick )
	EVT_TEXT( XRCID("TXT_Regex"), QuRegExmmFrame::txtRegex_OnTextChange )
	EVT_BUTTON( XRCID("BT_Quit"), QuRegExmmFrame::OnQuit )
	EVT_MENU( Menu_File_RegexStorage, QuRegExmmFrame::OnRegexStorage )
END_EVENT_TABLE()

IMPLEMENT_APP(QuRegExmmapp)

class wxXmlResource;

bool 
QuRegExmmapp::OnInit()
{	
	wxXmlResource::Get()->InitAllHandlers();

	wxXmlResource::Get()->Load("QuRegExmmFrame.xrc");

	QuRegExmmFrame *frame = new QuRegExmmFrame;
	
	wxXmlResource::Get()->LoadFrame( frame, NULL, "FRM_Main" );
	
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
	wxXmlResource::Get()->LoadDialog( regexDia, this, "DIA_Regex" );
	regexDia->InitializeDialog( this );
	
	wxMenu *menuFile = new wxMenu;
	
	menuFile->Append( Menu_File_AppName, _(STAT_TEXT) );
	menuFile->AppendSeparator(); // add items below		
	menuFile->Append( Menu_File_RegexStorage, wxT("&RegExmm Storage") );
	menuFile->AppendSeparator();
	menuFile->Append( Menu_File_About, _("&About...") );
	menuFile->AppendSeparator();
	menuFile->Append( Menu_File_Quit, _("E&xit") );
	
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _("&File") );
	
	SetMenuBar( menuBar );
	
	menuBar->Enable( Menu_File_AppName, false );
	
	txtRegex = ((wxTextCtrl*)this->FindWindow(XRCID("TXT_Regex")));
	txtSource = ((wxTextCtrl*)this->FindWindow(XRCID("TXT_Source")));
	chkMatch = ((wxCheckBox*)this->FindWindow(XRCID("CHK_Match")));
	
	SetSize( this->GetSize().GetWidth(), 350 );
	
	// set up the key down events
	txtRegex->Connect( wxID_ANY,
					   wxEVT_KEY_DOWN, wxKeyEventHandler(QuRegExmmFrame::txtRegex_KeyDown), NULL, this );
	
	// create reusable regex object
	mRegex = new wxRegEx;
	
	SetMinSize( this->GetSize() );	

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

void 
QuRegExmmFrame::OnQuit( wxCommandEvent& WXUNUSED( event ) )
{
	Close(TRUE);
}

void 
QuRegExmmFrame::OnAbout( wxCommandEvent& WXUNUSED( event ) )
{
	wxMessageBox( wxT( "QuRegEx Match Maker (Quantum Quinn)" ),
			wxT( "About "APP_NAME ), wxOK | wxICON_INFORMATION, this );
}

void QuRegExmmFrame::OnBtMatchClick( wxCommandEvent & WXUNUSED(evt) )
{
	FindMatch();
} // end

void QuRegExmmFrame::FindMatch()
{
	// get interactive values
	wxString pattern( txtRegex->GetValue() );
	wxString source( txtSource->GetValue() );
	
	// set the default style for the text ctrl
	txtSource->SetStyle(0, source.Length(), wxTextAttr(*wxBLACK,*wxWHITE));
	wxTextAttr ta;
	ta.SetFlags(wxTEXT_ATTR_TEXT_COLOUR);
	ta.SetTextColour(*wxBLUE);
	txtSource->SetDefaultStyle(ta);
	
	// reset the selection
	txtSource->SetSelection(0,0);
	
	size_t start = 0, len = 0;	
	
	// if the required fields are blank
	if ( pattern.IsEmpty() || source.IsEmpty() )
	{
		SetStatusText( _("EMPTY") );
		return;
	} // end
	
	wxLogNull * logNull = new wxLogNull; // suppress wxRegEx msgs

	// set the style var
	int regexStyle = wxRE_DEFAULT;

	// compile the regex object	
	if ( mRegex->Compile(pattern, regexStyle) )
	{
		size_t count = 0;
		size_t tmpStrLen = source.length(); // length won't change
		
		while ( mRegex->Matches(source, regexStyle, tmpStrLen) )
		{								
			mRegex->GetMatch(&start, &len, 0);
						
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
			
			++count;			
		} // end WHILE
				
		if ( count > 0 )
		{
			// set the status text
			SetStatusText( wxString::Format( 
					_("%i Match%s found for `%s`"), 
					 count, 
					 (count == 1 ? "" : "es"),  
					 pattern.c_str()) );
		} // end IF
		else
		{
			// set status text
			SetStatusText(wxString::Format(_("No match found for `%s`"), pattern.c_str()));
		} // end ELSE
		
		delete logNull; // restore msg notification
	} // end IF IsValid
	else
	{
		// inform user of the error
		SetStatusText( _("Check expression.") );
	} // end ELSE
		
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