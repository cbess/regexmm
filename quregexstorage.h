/***************************************************************************
 *   Copyright (C) Tue Mar 7 2006 by Quantum Quinn  *
 *   cbess@QuantumQuinn.com   *
 */
#include <wx/datstrm.h>
#include <wx/wfstream.h>
#include <wx/file.h>
#include <wx/wx.h>
#include <wx/listctrl.h>

#define DATA_FILE "quregex.dat"

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
		
		void LoadRegexStorage()
		{
			if ( wxFile::Exists(wxT(DATA_FILE)) )
			{
				wxFileInputStream file(wxT(DATA_FILE));
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
			wxFileOutputStream file(wxT(DATA_FILE));
			wxDataOutputStream store( file );
			
			int max = lsRegex->GetCount();
			
			store.Write32( max );
			
			for ( int i = 0 ; i < max ; ++i )
			{
				store.WriteString( lsRegex->GetString(i) );
			} // end FOR
			
			file.Close();			
		} // end
		
		bool HasText( wxString str )
		{
			wxLogNull lg;
			return wxRegEx( wxT("[^\\s]") ).Matches( str );
		}
				
		void GetRegex()
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
		
		void ClearText()
		{
			txtRegex->SetValue(wxT(""));
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
