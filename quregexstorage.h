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
#include <wx/datstrm.h>
#include <wx/wfstream.h>
#include <wx/file.h>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/stdpaths.h>

#define DATA_FILE "quregex.dat"
		
// determine which path separator to use (based on windows or unix)
#if defined(__UNIX__)
#define PATH_SEP "/"
#elif
#define PATH_SEP "\"
#endif

 class QuRegExmmFrame;
 class wxListEvent;
 
 class QuRegexStorageDia : public wxDialog
{
	private:
		wxTextCtrl * txtRegex;
		wxListBox * lsRegex;	
		QuRegExmmFrame * par;
		int prevIdx;
		
	public:
		void InitializeDialog( QuRegExmmFrame * parent )
		{
			par = parent;
			txtRegex = XRCCTRL(*this, "TXT_Regex", wxTextCtrl);
			lsRegex = XRCCTRL(*this, "LS_Regex", wxListBox);
			
			LoadRegexStorage();
		} // end
			
	private:
		
		wxString GetUserDataFilePath()
		{
			wxString filename = wxT(DATA_FILE);
			
			return GetUserFilePath(filename);
		}
				
		// gets the user's file path based on the OS
		wxString GetUserFilePath( wxString filename )
		{			
		   /* - returns one of the following
			* Unix: ~ (the home directory)
			* Windows: C:\Documents and Settings\username\Documents
			* Mac: ~/Documents 
			*/		
			wxString userDir = wxStandardPaths::Get().GetDocumentsDir();
			
			// return the formatted file path
			return wxString::Format(wxT("%s"PATH_SEP"%s"), userDir.c_str(), filename.c_str());
		}
		
		void LoadRegexStorage()
		{
			if ( wxFile::Exists(wxT(DATA_FILE)) )
			{
				wxFileInputStream file(GetUserDataFilePath());
				wxDataInputStream store( file );
				
				int max = store.Read32();
				
				for ( int i = 0 ; i < max ; ++i )
				{					
					lsRegex->Append(store.ReadString());
				} // end FOR
			} // end IF
		} // end
		
		void SaveRegexStorage()
		{			
			wxFileOutputStream file(GetUserDataFilePath());
			wxDataOutputStream store( file );
			
			int max = lsRegex->GetCount();
			
			store.Write32( max );
			
			for ( int i = 0 ; i < max ; ++i )
			{
				store.WriteString( lsRegex->GetString(i) );
			} // end FOR
			
			file.Close();			
		} // end

		inline bool HasText( wxString str )
		{
			return wxPCRE(wxT("[^\\s]")).Matches(str);
		}
				
		inline void GetRegex()
		{
			par->SetRegex( txtRegex->GetValue() );
		}
		
		void OnBtSave_Click( wxCommandEvent & WXUNUSED(evt) )
		{			
			if ( HasText(txtRegex->GetValue()) )
			{
				lsRegex->SetString( prevIdx, txtRegex->GetValue() );			
				
				SaveRegexStorage();				
				
			} // end IF
			else
				wxMessageBox(_("Regex field blank."));				
		} // end
		
		void OnBtNew_Click( wxCommandEvent & WXUNUSED( evt ) )
		{
			wxLogNull lg;
			
			if ( HasText(txtRegex->GetValue()) )
			{									
				lsRegex->Append(txtRegex->GetValue());	
						
				ClearText();
				
				SaveRegexStorage();
				
			} // end IF
			else
				wxMessageBox(_("Regex field blank."));
		} // end
		
		void OnBtDelete_Click( wxCommandEvent & WXUNUSED( evt ) )
		{
			if ( wxMessageBox(_("Delete regex?"), _("Delete?"),
				 wxYES_NO, this ) == wxYES )
			{
				lsRegex->Delete( prevIdx );
				ClearText();
				
				SaveRegexStorage();
			} // end IF
		} // end
		
		void OnBtClose_Click( wxCommandEvent & WXUNUSED(evt) )
		{
			Close();
		} // end
		
		void lsRegex_ItemActivated( wxCommandEvent & WXUNUSED(evt) )
		{ // lsRegex -> double click [enter]
			GetRegex();
		} // end
		
		void lsRegex_ItemSelected( wxCommandEvent & evt )
		{			
			txtRegex->SetValue( evt.GetString() );
			
			prevIdx = evt.GetSelection();
		} // end
		
		inline void ClearText()
		{
			txtRegex->SetValue(wxEmptyString);
		}
		
		void OnDialogClose( wxCloseEvent & evt )
		{
			if ( evt.CanVeto() )
			{
				// hide this dialog
				this->Show(FALSE);
				
				ClearText();
			
				SaveRegexStorage();
				
				// prevent system action
				evt.Skip();
			} // end IF
		}
		
	private:
		DECLARE_EVENT_TABLE()
}; // end CLASS

 BEGIN_EVENT_TABLE( QuRegexStorageDia, wxDialog )
		 EVT_CLOSE(QuRegexStorageDia::OnDialogClose)
		 EVT_BUTTON( XRCID("BT_Save"), QuRegexStorageDia::OnBtSave_Click )
		 EVT_BUTTON( XRCID("BT_New"), QuRegexStorageDia::OnBtNew_Click )
		 EVT_BUTTON( XRCID("BT_Delete"), QuRegexStorageDia::OnBtDelete_Click )
		 EVT_BUTTON( XRCID("BT_Close"), QuRegexStorageDia::OnBtClose_Click )
		 EVT_LISTBOX_DCLICK( XRCID("LS_Regex"), QuRegexStorageDia::lsRegex_ItemActivated )
		 EVT_LISTBOX( XRCID("LS_Regex"), QuRegexStorageDia::lsRegex_ItemSelected )
END_EVENT_TABLE() 
