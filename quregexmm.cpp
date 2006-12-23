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
	
	menuFile->Append( Menu_File_AppName, wxT(STAT_TEXT) );
	menuFile->AppendSeparator(); // add items below
	
	menuFile->Append( Menu_File_RegexStorage, wxT("&RegExmm Storage") );
	menuFile->AppendSeparator();
	menuFile->Append( Menu_File_About, wxT( "&About..." ) );
	menuFile->AppendSeparator();
	menuFile->Append( Menu_File_Quit, wxT( "E&xit" ) );
	
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, wxT( "&File" ) );
	
	SetMenuBar( menuBar );
	
	menuBar->Enable( Menu_File_AppName, false );
	
	txtRegex = ((wxTextCtrl*)this->FindWindow(XRCID("TXT_Regex")));
	txtSource = ((wxTextCtrl*)this->FindWindow(XRCID("TXT_Source")));
	chkMatch = ((wxCheckBox*)this->FindWindow(XRCID("CHK_Match")));
	
	SetSize( this->GetSize().GetWidth(), 350 );
	
	//this->GetSize().SetHeight( 350 );
	
	SetMinSize( this->GetSize() );	

	CreateStatusBar(2);	
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
	txtSource->SetStyle( 0, txtSource->GetValue().Length(),
						  wxTextAttr(wxColour("BLACK"),wxColour("WHITE")) );	
	txtSource->SetSelection(0,0);
	
	SetStatusText( wxT("") );
	
	size_t start = 0, len = 0;
	
	wxString pattern( txtRegex->GetValue() );
	wxString source( txtSource->GetValue() );
	
	if ( pattern.IsEmpty() || source.IsEmpty() )
	{
		SetStatusText( wxT("EMPTY") );
		return;
	} // end
	
	wxLogNull * logNull = new wxLogNull; // suppress wxRegEx msgs

	int _style = wxRE_DEFAULT | wxRE_NEWLINE;

	wxRegEx regex( pattern, _style );		

	if ( regex.IsValid() )
	{
		wxString tmpStr( source );
		
		size_t max = regex.GetMatchCount();
		size_t count = 0;
		
		while ( regex.Matches(tmpStr, _style) )
		{								
			regex.GetMatch( &start, &len, 0 );
			
			SetStatusText( wxString::Format( "%i Match(es) found for `%s`", max, pattern.c_str() ) );
			
			long to = start+len;

			wxTextAttr ta( wxColour("BLACK"), wxColour("GREEN") );
			
			txtSource->SetStyle( start, to, ta );
			
			txtSource->SetSelection( 0, 0 );
	
			for ( int i = start ; i < len ; ++i )
				tmpStr.SetChar( i, ' ' );
			// end FOR
			
			if ( ++count == max ) break;
		} // end WHILE
				
	delete logNull; // restore msg notification

	} // end IF IsValid
	else
	{
		SetStatusText( wxT("Check expression.") );
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