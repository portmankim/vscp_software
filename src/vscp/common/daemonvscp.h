// daemon_VSCP.h
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


#if !defined(DAEMONVSCP_H__7D80016B_5EFD_40D5_94E3_6FD9C324CC7B__INCLUDED_)
#define DAEMONVSCP_H__7D80016B_5EFD_40D5_94E3_6FD9C324CC7B__INCLUDED_


#include "wx/thread.h"
#include "wx/socket.h"
#include <wx/datetime.h>

#include "guid.h"


class CControlObject;
class CClientItem;


// This thread is used for node discovery


class discoveryVSCPThread : public wxThread
{

public:
	
    /// Constructor
    discoveryVSCPThread();

    /// Destructor
    ~discoveryVSCPThread();

    /*!
        Thread code entry point
	*/
    virtual void *Entry();

    /*! 
        called when the thread exits - whether it terminates normally or is
        stopped with Delete() (but not when it is Kill()ed!)
    */
    virtual void OnExit();

    /*!
        Send event to client with specific id
    */
    bool sendEvent( vscpEvent *pEvent, uint32_t obid );

    /*!
        Read a level I register
    */
    bool readLevel1Register( uint8_t nodeid, 
								uint8_t reg, 
								uint8_t *pcontent,
                                uint32_t clientID,
                                uint32_t timeout = 1000 );

    /*!
        Termination control
    */
    bool m_bQuit;

    // Node id for the node to investigate
    uint8_t m_nodeid;

    // Client id for interface the node is located at
    uint32_t m_clientID;

    // Clientitem for this trhread
    CClientItem *m_pClientItem;

    // Pointer to the control object
    CControlObject *m_pCtrlObject;
};


struct discoveredNodeInfo {
    int bStatus;        // 0 = working, -1=failed, 777=succes.
    discoveryVSCPThread *pThread;  // Discover thread
    uint8_t nodeid;     // nodeid for the node to investigate
    uint32_t clientId;  // Clientid for node to investigate
    cguid guid;         // GUID for node.
    wxString mdfPath;   // MDF path for node.
};

WX_DECLARE_LIST ( discoveredNodeInfo, DISCOVERYLIST );


class cnodeInformation 
{

public:

    cnodeInformation();
    ~cnodeInformation();

private:

    /*!
        Client originating this 
        Event
    */
    CClientItem *m_pClientItem;

    // Full GUID for node
    cguid m_guid;

    // MDF path for node
    wxString m_mdfPath;

    /// Last haertbeat from this node
    wxDateTime m_lastHeartBeat;

};


WX_DECLARE_LIST( cnodeInformation, VSCP_KNOWN_NODES_LIST );

/*!
    This class implement a one of thread that look
    for specific events and react on them appropriatly.

*/

class daemonVSCPThread : public wxThread
{

public:
	
    /// Constructor
    daemonVSCPThread();

    /// Destructor
    ~daemonVSCPThread();

    /*!
        Thread code entry point
	*/
    virtual void *Entry();


    /*! 
        called when the thread exits - whether it terminates normally or is
        stopped with Delete() (but not when it is Kill()ed!)
    */
    virtual void OnExit();

    /*!
        Find node
    */
    cnodeInformation *findKnownNode( cguid& guid );

    /*!
        Save the list of known nodes
    */
    void saveKnownNodes( void );


    /*!
        Load the list of known nodes
    */
    void loadKnownNodes( void );


    /*!
        Termination control
    */
    bool m_bQuit;

    CControlObject *m_pCtrlObject;

    // This array holds guid's for nodes that
    // we have received a heartbeat from. The GUID
    // is not aleays the nodes actual GUID but oftebn 
    // the interface GUID plus nickname.
    wxArrayString m_knownGUIDs;

    // This list holds information about each discovered
    // node in the system and the content in the list
    // is preserved in the filesystem over time
    VSCP_KNOWN_NODES_LIST m_knownNodes;

    // This list contains items (nodes) that are under discovery
    DISCOVERYLIST m_discoverList;

};




#endif



