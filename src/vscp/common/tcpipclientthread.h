// tcpipclientthread.h
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
// 
// This file is part of the VSCP (http://www.vscp.org) 
//
// Copyright (C) 2000-2015 
// Ake Hedman, Grodans Paradis AB, <akhe@grodansparadis.com>
// 
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this file see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//


#if !defined(TCPIPCLIENTTHREAD_H__7D80016B_5EFD_40D5_94E3_6FD9C324CC7B__INCLUDED_)
#define TCPIPCLIENTTHREAD_H__7D80016B_5EFD_40D5_94E3_6FD9C324CC7B__INCLUDED_


#include "wx/thread.h"
//#include "wx/socket.h"

#include "userlist.h"
#include "controlobject.h"

#define VSCP_TCP_MAX_CLIENTS                1024
#define MSG_COPYRIGHT                       "+OK Copyright (c) 2000-2015, Grodans Paradis AB, http://www.grodansparadis.com\r\n"
#define MSG_WELCOME                         "+OK Welcome to the VSCP daemon.\r\n"
#define MSG_OK                              "+OK - Success.\r\n"
#define MSG_GOODBY                          "+OK - Connection closed by client.\r\n"
#define MSG_GOODBY2                         "+OK - Connection closed.\r\n"
#define MSG_USENAME_OK                      "+OK - User name accepted, password please\r\n"
#define MSG_PASSWORD_OK                     "+OK - Ready to work.\r\n"
#define MSG_QUEUE_CLEARED                   "+OK - All events cleared.\r\n"
#define MSG_RECEIVE_LOOP                    "+OK - Receive loop entered. QUITLOOP to terminate.\r\n"
#define MSG_QUIT_LOOP						"+OK - Quit receive loop.\r\n"
#define MSG_CAN_MODE                        "+OK - CAN mode set.\r\n"

#define MSG_ERROR                           "-OK - Error\r\n"
#define MSG_UNKNOWN_COMMAND                 "-OK - Unknown command\r\n"
#define MSG_PARAMETER_ERROR                 "-OK - Invalid parameter or format\r\n"
#define MSG_BUFFER_FULL                     "-OK - Buffer Full\r\n"
#define MSG_NO_MSG                          "-OK - No event(s) available\r\n"

#define MSG_PASSWORD_ERROR                  "-OK - Invalid username or password.\r\n"
#define MSG_NOT_ACCREDITED                  "-OK - Need to log in to perform this command.\r\n"
#define MSG_INVALID_USER                    "-OK - Invalid user.\r\n"
#define MSG_NEED_USERNAME                   "-OK - Need a Username before a password can be entered.\r\n"

#define MSG_MAX_NUMBER_OF_CLIENTS           "-OK - Max number of clients connected.\r\n"
#define MSG_INTERNAL_ERROR                  "-OK - Server Internal error.\r\n"
#define MSG_INTERNAL_MEMORY_ERROR           "-OK - Internal Memory error.\r\n"
#define MSG_INVALID_REMOTE_ERROR            "-OK - Invalid or unknown peer.\r\n"

#define MSG_LOW_PRIVILEGE_ERROR             "-OK - User need higher privilege level to perform this operation.\r\n"
#define MSG_INTERFACE_NOT_FOUND             "-OK - Interface not found.\r\n"

#define MSG_VARIABLE_NOT_DEFINED            "-OK - Variable is not defined.\r\n"
#define MSG_VARIABLE_MUST_BE_EVENT_TYPE     "-OK - Variable must be of event type.\r\n"


//class CControlObject;
//class CClientItem;

typedef const char * ( * COMMAND_METHOD) (  void );

/*!
	Class that defines one command
*/
typedef struct {
	wxString m_strCmd;                  // Command name
	uint8_t m_securityLevel;            // Security level for command (0-15)
	wxProcess *m_pfnCommand;            // Function to execute
} structCommand;



/*!
	This class implement the listen thread for
	the vscpd connections on the TCP interface
*/

class VSCPClientThread : public wxThread
{

public:
	
	/// Constructor
	VSCPClientThread();

	/// Destructor
	~VSCPClientThread();

	/*!
		Thread code entry point
	*/
	virtual void *Entry();

	/*!
		TCP/IP handler
	*/
	static void ev_handler(struct ns_connection *conn, enum ns_event ev, void *p);

	/*!
		When a command is received on the TCP/IP interface the command handler is called.
	*/
	void CommandHandler( struct ns_connection *conn, 
							CControlObject *pCtrlObject, 
							wxString& strCommand );

	/*! 
		called when the thread exits - whether it terminates normally or is
		stopped with Delete() (but not when it is Kill()ed!)
	*/
	virtual void OnExit();

	
		/*!
		Check if a user has been verified
		@return true if verified, else false.
	*/
	bool isVerified( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Check if a user has enough privilege
		@return true if yes, else false.
	*/
	bool checkPrivilege( struct ns_connection *conn, CControlObject *pCtrlObject, unsigned char reqiredPrivilege );


	/*!
		Client send event
	*/	
	void handleClientSend( struct ns_connection *conn,  CControlObject *pCtrlObject  );

	/*!
		Client receive
	*/	
	void handleClientReceive( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		sendOneEventFromQueue
		@param 	bStatusMsg True if response messages (+OK/-OK) should be sent. Default.
		@return True on success/false on failue.
	*/
	bool sendOneEventFromQueue( struct ns_connection *conn, CControlObject *pCtrlObject, bool bStatusMsg = true );

	/*!
		Client DataAvailable
	*/	
	void handleClientDataAvailable ( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client Clear Input queue
	*/
	void handleClientClearInputQueue( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client Get statistics
	*/
	void handleClientGetStatistics( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client get status
	*/
	void handleClientGetStatus ( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client get channel GUID
	*/
	void handleClientGetChannelGUID ( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client set channel ID
	*/
	void handleClientSetChannelGUID ( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client get version
	*/
	void handleClientGetVersion( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client set filter
	*/
	void handleClientSetFilter( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client set mask
	*/
	void handleClientSetMask( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client issue user
	*/
	void handleClientUser( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
	Client issue password
	*/
	bool handleClientPassword ( struct ns_connection *conn, CControlObject *pCtrlObject  );

	/*!
		Client Get channel ID
	*/
	void handleClientGetChannelID ( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Handle RcvLoop
	*/
	void handleClientRcvLoop( struct ns_connection *conn, CControlObject *pCtrlObject );

  /*!
		Client Help
	*/
	void handleClientHelp ( struct ns_connection *conn, CControlObject *pCtrlObject );

  /*!
		Client Test
	*/
	void handleClientTest ( struct ns_connection *conn, CControlObject *pCtrlObject );

  /*!
		Client Restart
	*/
	void handleClientRestart ( struct ns_connection *conn, CControlObject *pCtrlObject );

  /*!
		Client Shutdown
	*/
	void handleClientShutdown ( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client REMOTE command
	*/
	void handleClientRemote( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client CLIENT/INTERFACE command
	*/
	void handleClientInterface( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client INTERFACE LIST command
	*/
	void handleClientInterface_List( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client INTERFACE UNIQUE command
	*/
	void handleClientInterface_Unique( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client INTERFACE NORMAL command
	*/
	void handleClientInterface_Normal( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client INTERFACE CLOSE command
	*/
	void handleClientInterface_Close( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client UDP command
	*/
	void handleClientUdp( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client FILE command
	*/
	void handleClientFile( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client VARIABLE command
	*/
	void handleClientVariable( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable List
	*/
	void handleVariable_List( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable Write
	*/
	void handleVariable_Write( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable Read
        @param bOKResponse OK response will be given if true (default).
	*/
	void handleVariable_Read( struct ns_connection *conn, CControlObject *pCtrlObject, bool bOKResponse = true );

	/*!
		Variable ReadReset
	*/
	void handleVariable_ReadReset( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable Write
	*/
	void handleVariable_Reset( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable Remove
	*/
	void handleVariable_Remove( struct ns_connection *conn, CControlObject *pCtrlObject );

    /*!
        Variable Read/remove
    */
    void handleVariable_ReadRemove( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable Length
	*/
	void handleVariable_Length( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable Load
	*/
	void handleVariable_Load( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Variable Save
	*/
	void handleVariable_Save( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client DM command
	*/
	void handleClientDm( struct ns_connection *conn, CControlObject *pCtrlObject  );

	/*!
		Enable DM row
	*/
	void handleDM_Enable( struct ns_connection *conn, CControlObject *pCtrlObject  );

	/*!
		Disable DM row
	*/
	void handleDM_Disable( struct ns_connection *conn, CControlObject *pCtrlObject  );

	/*!
		List DM 
	*/
	void handleDM_List( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Add DM row
	*/
	void handleDM_Add( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Delete DM row
	*/
	void handleDM_Delete( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Do action for row
	*/
	void handleDM_Trigger( struct ns_connection *conn, CControlObject *pCtrlObject );
	
	/*!
		Delete DM row trigger counter
	*/
	void handleDM_ClearTriggerCount( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Delete DM row error counter
	*/
	void handleDM_ClearErrorCount( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Reset DM
	*/
	void handleDM_Reset( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client LIST command
	*/
	void handleClientList( struct ns_connection *conn, CControlObject *pCtrlObject );

	/*!
		Client DRIVER command
	*/
	void handleClientDriver( struct ns_connection *conn, CControlObject *pCtrlObject );

    /*!
        Client WhatCanYouDo command
    */
    void handleClientCapabilityRquest( struct ns_connection *conn, CControlObject *pCtrlObject );

// --- Member variables ---

	/*!
		Termination control
	*/
	bool m_bQuit;
	

	/*!
		Pointer to ower owner
	*/
	CControlObject *m_pCtrlObject;

};


#endif




