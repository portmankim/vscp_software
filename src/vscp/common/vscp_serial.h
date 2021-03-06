// FILE: vscp_serial.h 
//
// The MIT License (MIT)
//
// Copyright (c) 2000-2015 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This file contains defines for the VSCP Serial protocol which 
// is described here http://www.vscp.org/docs/vscpspec/doku.php?id=physical_level_lower_level_protocols#vscp_over_a_serial_channel_rs-232
//

#ifndef _VSCP_SERIAL_H_
#define _VSCP_SERIAL_H_

#include <vscp.h>

// Outgoing VSCP event
// -------------------
// [0]		DLE
// [1]		STX
// [2]		Frame type (1 - VSCP event to/from node.)
// [3]		Channel (always zero)
// [4]		Sequence number 
// [5/6]	Size of payload 
// [7]		VSCP head
// [8]   	VSCP Priority
// [9]  	MSB of VSCP class
// [10]  	LSB of VSCP class
// [11]  	MSB of VSCP type
// [12]  	LSB of VSCP type
// [13-28]	GUID MSB - LSB
// [29-n]  	VSCP data (0-487 bytes/Max 8 byte for Level I) 
// [len-3]	CRC
// [len-2]	STX
// [len-1]	ETX

// CANAL message
// -------------
// [0]      DLE
// [1]      STX
// [2]      Frame type (2 - CANAL message.)
// [3]      Channel (always zero)
// [4]      Sequence number 
// [5/6]    Size of payload ( 12 + sizeData )
// [7]      CANAL flags (MSB)
// [8]      CANAL flags
// [9]      CANAL flags
// [10]     CANAL flags (LSB)
// [11]     CANAL timestamp (MSB)
// [12]     CANAL timestamp
// [13]     CANAL timestamp
// [14]     CANAL timestamp (LSB)
// [15]     CAN id (MSB)
// [26]     CAN id
// [27]     CAN id
// [28]     CAN id (LSB)
// [29-n]   CAN data (0-8 bytes) 
// [len-3]  CRC
// [len-2]  DLE
// [len-1]  ETX


// VSCP Driver special character codes
#define DLE     0x10
#define STX     0x02
#define ETX     0x03

// Serial protocol defines

// VSCP Driver states
#define STATE_VSCP_SERIAL_DRIVER_WAIT_FOR_FRAME_START       0
#define STATE_VSCP_SERIAL_DRIVER_WAIT_FOR_FRAME_END         1
#define STATE_VSCP_SERIAL_DRIVER_FRAME_RECEIVED             2

// VSCP Driver positions in frame
#define VSCP_SERIAL_DRIVER_POS_FRAME_TYPE                   0
#define VSCP_SERIAL_DRIVER_POS_FRAME_CHANNEL                1
#define VSCP_SERIAL_DRIVER_POS_FRAME_SEQUENCY               2
#define VSCP_SERIAL_DRIVER_POS_FRAME_SIZE_PAYLOAD_MSB       3
#define VSCP_SERIAL_DRIVER_POS_FRAME_SIZE_PAYLOAD_LSB       4
#define VSCP_SERIAL_DRIVER_POS_FRAME_PAYLOAD                5

// VSCP Driver operations
#define VSCP_SERIAL_DRIVER_OPERATION_NOOP                   0
#define VSCP_SERIAL_DRIVER_OPERATION_VSCP_EVENT             1
#define VSCP_SERIAL_DRIVER_OPERATION_CANAL                  2
#define VSCP_SERIAL_DRIVER_OPERATION_CONFIGURE              3
#define VSCP_SERIAL_DRIVER_OPERATION_POLL                   4
#define VSCP_SERIAL_DRIVER_OPERATION_NO_EVENT               5
#define VSCP_SERIAL_DRIVER_OPERATION_MULTI_FRAME_CANAL      6
#define VSCP_SERIAL_DRIVER_OPERATION_MULTI_FRAME_VSCP       7
#define VSCP_SERIAL_DRIVER_OPERATION_CAPS_REQUEST           8
#define VSCP_SERIAL_DRIVER_OPERATION_CAPS_RESPONSE          9
#define VSCP_SERIAL_DRIVER_OPERATION_SENT_ACK               249
#define VSCP_SERIAL_DRIVER_OPERATION_SENT_NACK              250
#define VSCP_SERIAL_DRIVER_OPERATION_ACK                    251
#define VSCP_SERIAL_DRIVER_OPERATION_NACK                   252
#define VSCP_SERIAL_DRIVER_OPERATION_ERROR                  253
#define VSCP_SERIAL_DRIVER_OPERATION_COMMAND_REPLY          254
#define VSCP_SERIAL_DRIVER_OPERATION_COMMAND                255

// VSCP Driver errors
#define VSCP_SERIAL_DRIVER_ERROR_CHECKSUM                   1
#define VSCP_SERIAL_DRIVER_ERROR_UNKNOWN_OPERATION          2

// VSCP driver commands
#define VSCP_SERIAL_DRIVER_COMMAND_NOOP                     0
#define VSCP_SERIAL_DRIVER_COMMAND_OPEN                     1
#define VSCP_SERIAL_DRIVER_COMMAND_LISTEN                   2
#define VSCP_SERIAL_DRIVER_COMMAND_LOOPBACK                 3
#define VSCP_SERIAL_DRIVER_COMMAND_CLOSE                    4
#define VSCP_SERIAL_DRIVER_COMMAND_SET_FILTER               5

// Packed size for capabilities structure
#define VSCP_SERIAL_DRIVER_CAPS_SIZE                        4   

// Capabilities structure
typedef struct {
    
    /*!
        Max number of VSCP frames in
        multifram payload
    */
    uint8_t maxVscpFrames; 

    /*!
        Max number of CANAL frames in
        multiframe payload.
    */
    uint8_t maxCanalFrames;

} vscp_serial_caps;

#endif