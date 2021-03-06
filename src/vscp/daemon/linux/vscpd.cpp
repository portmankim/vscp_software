// vscpd.cpp : Defines the class behaviours for the application.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
// 
// This file is part of the VSCP (http://www.vscp.org) 
//
// Copyright (C) 2000-2014 
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <syslog.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <syslog.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

#include "vscpd.h"
#include <crc.h>
#include <controlobject.h>
#include <version.h>

#define DEBUG

// Globals for the daemon
int gbStopDaemon;
int gnDebugLevel = 0;
bool gbDontRunAsDaemon = false;

CControlObject *gpobj;

void copyleft(void);
void help(char *szPrgname);

void sighandler(int sig)
{
	gpobj->m_bQuit = true;
	gbStopDaemon = true;
	syslog(LOG_CRIT, "vscpd: signal received, forced to stop.: %m");
	wxLogError(_("vscpd: signal received, forced to stop.: %m"));
}

/////////////////////////////////////////////////////////////////////////////
// VSCPApp


/////////////////////////////////////////////////////////////////////////////
// VSCPApp construction

VSCPApp::VSCPApp()
{
	;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CanalApp object

int main(int argc, char **argv)
{
	int arg = 0;
	wxString strcfgfile;

	wxSocketBase::Initialize();
	crcInit();
	/*	
	  unsigned char ttt[ 50 ];
	  for ( int m=0; m<50; m++ ) {
	  ttt[ m ] = m+8;
	  }	
    
	 *((unsigned short *)(ttt + 48 )) = crcFast( ttt, 48 );
	  ttt[ 48 ] = 0x9f; 
	  ttt[ 49 ] = 0x87;
	  printf( "CRC for data = %X\n", crcFast( ttt, 48 ) );
	  printf( "CRC for all = %X\n", crcFast( ttt, 50 ) );
	 */

	wxInitializer initializer;
	if (!::wxInitialize()) {
		fprintf(stderr, "Failed to initialize the wxWindows library, aborting.");

		return -1;
	}
	
	gpobj = new CControlObject();

	//wxStandardPaths *strpath = wxStandardPaths::Get();
	//strcfgFile =  _("/etc/vscp/vscpd.conf" );     // default config path
	strcfgfile = wxStandardPaths::Get().GetConfigDir() + _("/vscp/vscpd.conf");
	gbStopDaemon = false;


	VSCPApp theApp;
	//IMPLEMENT_APP(theApp)

	while ((arg = getopt(argc, argv, "d:c:hgs")) != EOF) {

		switch (arg) {

		case 's':
			wxPrintf(_("Be hungry, stay foolish my friend!\n"));
			wxPrintf(_("I will ***NOT*** run as daemon! (ctrl+c to terminate)\n"));
			gbDontRunAsDaemon = true;
			break;

		case 'c':
			strcfgfile = wxString(optarg, wxConvUTF8);
			//wxMBConvUTF8 cnv;
			//cnv.MB2WC( strcfgfile, optarg, 10 );
			break;

		case 'd':
			gnDebugLevel = atoi(optarg);
			break;

		case 'g':
			copyleft();
			exit(0);
			break;

		default:
		case 'h':
			help(argv[0]);
			exit(-1);
		}
	}

	wxLogDebug(_("ControlObject: Configfile =") + strcfgfile);
	if ( !theApp.init( strcfgfile ) ) {
		printf("ControlObject: Failed to configure. Terminating.");
		wxLogDebug(_("ControlObject: Failed to configure. Terminating."));
	}
	
	delete gpobj;

}



/////////////////////////////////////////////////////////////////////////////
// initialization

BOOL VSCPApp::init(wxString& strcfgfile)
{

	if (!gbDontRunAsDaemon) {

		pid_t pid, sid;

		// Fork child	
		if (0 > (pid = fork())) {
			// Failure
			printf("Failed to fork.");
			return -1;
		} 
		else if (0 != pid) {
			exit(0); // Parent goes by by.
		}

		sid = setsid(); // Become session leader
		if (sid < 0) {
			// Failure 
			printf("Failed to become session leader.");
			return -1;
		}

		// Write pid to file
		FILE *pFile;
		pFile = fopen("/var/run/vscpd/vscpd.pid", "w");
		if (NULL != pFile) {
			fprintf(pFile, "%d\n", sid);
			fclose(pFile);
		}

		if ( chdir("/") ) { // Change working directory
			syslog( LOG_WARNING, "VSCPD: Failed to change dir to rootdir");
		}

		umask(0); // Clear out file mode creation mask

		// Close out the standard file descriptors 
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		if (open("/", 0)) {
			syslog(LOG_CRIT, "VSCPD: open / not 0: %m");
		}

		dup2(0, 1);
		dup2(0, 2);

		struct sigaction my_action;

		// Ignore SIGPIPE
		my_action.sa_handler = SIG_IGN;
		my_action.sa_flags = SA_RESTART;
		sigaction(SIGPIPE, &my_action, NULL);

		// Redirect SIGQUIT
		my_action.sa_handler = sighandler;
		my_action.sa_flags = SA_RESTART;
		sigaction(SIGQUIT, &my_action, NULL);

		// Redirect SIGABRT
		my_action.sa_handler = sighandler;
		my_action.sa_flags = SA_RESTART;
		sigaction(SIGABRT, &my_action, NULL);

		// Redirect SIGINT
		my_action.sa_handler = sighandler;
		my_action.sa_flags = SA_RESTART;
		sigaction(SIGINT, &my_action, NULL);

		// Redirect SIGTERM
		my_action.sa_handler = sighandler;
		my_action.sa_flags = SA_RESTART;
		sigaction(SIGTERM, &my_action, NULL);

	}

	//wxFile


	wxLogDebug(_("VSCPD: init"));
	if ( !gpobj->init( strcfgfile ) ) {
		printf("Can't initialize daemon. Exiting.");
		syslog(LOG_CRIT, "Can't initialize daemon. Exiting.");
		return FALSE;
	}

	wxLogDebug(_("VSCPD: run"));
	if (!gpobj->run()) {
		printf("Unable to start the VSCPD application. Exiting.");
		syslog(LOG_CRIT, "Unable to start the VSCPD application. Exiting.");
	}

	wxLogDebug(_("VSCPD: cleanup"));
	if (!gpobj->cleanup()) {
		printf("Unable to clean up the VSCPD application.");
		syslog(LOG_CRIT, "Unable to clean up the VSCPD application.");
	}

	// Remove the pid file
	unlink("/var/run/vscp/vscpd/vscpd.pid");
	

	return FALSE;
}




///////////////////////////////////////////////////////////////////////////////
// copyleft

void copyleft(void)
{
	wxPrintf(_("\n\n"));
	wxPrintf(_("vscpd - "));
	wxPrintf(_(VSCPD_DISPLAY_VERSION));
	wxPrintf(_("\n"));
	wxPrintf(_(VSCPD_COPYRIGHT));
	wxPrintf(_("\n"));
	wxPrintf(_("\n"));
	wxPrintf(_("This program is free software; you can redistribute it and/or \n"));
	wxPrintf(_("modify it under the terms of the GNU General Public License as \n"));
	wxPrintf(_("published by the Free Software Foundation; either version 2 of \n"));
	wxPrintf(_("the License, or ( at your option ) any later version.\n"));
	wxPrintf(_("\n"));
	wxPrintf(_("This program is distributed in the hope that it will be useful,\n"));
	wxPrintf(_("but WITHOUT ANY WARRANTY; without even the implied warranty of\n"));
	wxPrintf(_("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"));
	wxPrintf(_("\n"));
	wxPrintf(_("GNU General Public License for more details.\n"));
	wxPrintf(_("\n"));
	wxPrintf(_("You should have received a copy of the GNU General Public License\n"));
	wxPrintf(_("along with this program; if not, write to the Free Software\n"));
	wxPrintf(_("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\n"));
}


///////////////////////////////////////////////////////////////////////////////
// help

void help(char *szPrgname)
{
	fprintf(stderr, "Usage: %s [-ahg] [-c command-file] -dn\n", szPrgname);
	fprintf(stderr, "\t-h\tThis help message.\n");
	fprintf(stderr, "\t-s\tStandalone (don't run as daemon)..\n");
	fprintf(stderr, "\t-c\tSpecify a configuration file \n");
	fprintf(stderr, "\t-d\tDebug level. 0=None, 99=Don't run as daemon. ");
	fprintf(stderr, "that should be used (default: /etc/canalworks.conf).\n");
	fprintf(stderr, "\t-g\tPrint the GNU copyleft info.\n");
}




