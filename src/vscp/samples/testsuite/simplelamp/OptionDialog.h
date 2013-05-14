// OptionDialog.h: interface for the COptionDialog class.
//
//////////////////////////////////////////////////////////////////////
// Copyright (C) 2004-2011 Ake Hedman akhe@eurosource.se 
//
// This software is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//

#if !defined(AFX_OPTIONDIALOG_H__2A4A7FC3_C572_4C69_994D_9750357A3527__INCLUDED_)
#define AFX_OPTIONDIALOG_H__2A4A7FC3_C572_4C69_994D_9750357A3527__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class COptionDialog  : public wxDialog
{

public:

	/*!
		Constructor
	*/
	COptionDialog( wxWindow *parent );

	/*!
		Destructor
	*/
	virtual ~COptionDialog();


	/*!
		On change in address LSB
	*/
	void OnAddressChange( wxSpinEvent& event );

	/*!
		Print out the VSCP address
	*/
	void printVSCPAddress( void );

public:
 

	/*!
		Full node address
	*/
	unsigned char m_vscpaddr[ 16 ];


	/*!
		Spin Control for group
	*/
	wxSpinCtrl *m_pSpinCtrlGroup;


	/*!
		Spin Control for LSB Address
	*/
	wxSpinCtrl *m_pSpinCtrlLsbAddress;


  
private:

	wxButton *m_pOKButton;
	wxButton *m_pCancelButton;

	
	// any class wishing to process wxWidgets events must use this macro
   DECLARE_EVENT_TABLE()

};


enum 
{
    BtnSave = 1100,
    BtnCancel,
	EditGroup,
	EditLsbAddress	
};

#endif // !defined(AFX_OPTIONDIALOG_H__2A4A7FC3_C572_4C69_994D_9750357A3527__INCLUDED_)
