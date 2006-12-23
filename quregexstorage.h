/***************************************************************************
 *   Copyright (C) Tue Mar 7 2006 by Quantum Quinn  *
 *   cbess@QuantumQuinn.com   *
 */
#include <wx/datstrm.h>
#include <wx/wfstream.h>
#include <wx/file.h>
#include <wx/wx.h>
#include <wx/listctrl.h>


#define DATA "./quregex.dat"

 class QuRegExmmFrame;
 class wxListEvent;
 
 class QuRegexStorageDia : public wxDialog
{
	private:
		wxTextCtrl * txtRegex;
		wxListCtrl * lsRegex;	
		QuRegExmmFrame * par;
		int prevIdx;
		
	public:
		void InitializeDialog( QuRegExmmFrame * parent )
		{
			par = parent;
			txtRegex = (wxTextCtrl*)FindWindow( XRCID("TXT_Regex") );
			lsRegex = (wxListCtrl*)FindWindow( XRCID("LS_Regex") );
			
			LoadRegexStorage();
		} // end
			
	private:
		
		void LoadRegexStorage()
		{
			if ( wxFile::Exists( DATA ) )
			{
				wxFileInputStream file( DATA );
				wxDataInputStream store( file );
				
				int max = store.Read32();
				
				for ( int i = 0 ; i < max ; ++i )
				{
					wxListItem li;
					li.SetText( store.ReadString() );
					
					lsRegex->InsertItem( li );
				} // end FOR
			} // end IF
		} // end
		
		void SaveRegexStorage()
		{			
			wxFileOutputStream file( DATA );
			wxDataOutputStream store( file );
			
			int max = lsRegex->GetItemCount();
			
			store.Write32( max );
			
			for ( int i = 0 ; i < max ; ++i )
			{
				store.WriteString( lsRegex->GetItemText(i) );
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
				lsRegex->SetItemText( prevIdx, txtRegex->GetValue() );			
				
				SaveRegexStorage();				
				
			} // end IF
			else
				wxMessageBox( "Regex field blank." );				
		} // end
		
		void OnBtNew_Click( wxCommandEvent & WXUNUSED( evt ) )
		{
			wxLogNull lg;
			
			if ( HasText(txtRegex->GetValue()) )
			{						
				wxListItem li;
				li.SetText( txtRegex->GetValue() );
				
				lsRegex->InsertItem( li );			
						
				ClearText();
				
				SaveRegexStorage();
				
			} // end IF
			else
				wxMessageBox( "Regex field blank." );
		} // end
		
		void OnBtDelete_Click( wxCommandEvent & WXUNUSED( evt ) )
		{
			if ( wxMessageBox("Delete regex?", "Delete?",
				 wxYES_NO, this ) == wxYES )
			{
				lsRegex->DeleteItem( prevIdx );
				ClearText();
				
				SaveRegexStorage();
			} // end IF
		} // end
		
		void OnBtClose_Click( wxCommandEvent & WXUNUSED(evt) )
		{
			this->Hide();
			ClearText();
			
			SaveRegexStorage();
		} // end
		
		void lsRegex_ItemActivated( wxListEvent & evt )
		{ // lsRegex -> double click [enter]
			GetRegex();
		} // end
		
		void lsRegex_ItemSelected( wxListEvent & evt )
		{			
			txtRegex->SetValue( evt.GetText() );
			
			prevIdx = evt.GetIndex();
		} // end
		
		void ClearText()
		{
			txtRegex->SetValue( "" );
		}
		
	private:
		DECLARE_EVENT_TABLE()
}; // end CLASS

 BEGIN_EVENT_TABLE( QuRegexStorageDia, wxDialog )
		 EVT_BUTTON( XRCID("BT_Save"), QuRegexStorageDia::OnBtSave_Click )
		 EVT_BUTTON( XRCID("BT_New"), QuRegexStorageDia::OnBtNew_Click )
		 EVT_BUTTON( XRCID("BT_Delete"), QuRegexStorageDia::OnBtDelete_Click )
		 EVT_BUTTON( XRCID("BT_Close"), QuRegexStorageDia::OnBtClose_Click )
		 EVT_LIST_ITEM_ACTIVATED( XRCID("LS_Regex"), QuRegexStorageDia::lsRegex_ItemActivated )
		 EVT_LIST_ITEM_SELECTED( XRCID("LS_Regex"), QuRegexStorageDia::lsRegex_ItemSelected )
END_EVENT_TABLE() 