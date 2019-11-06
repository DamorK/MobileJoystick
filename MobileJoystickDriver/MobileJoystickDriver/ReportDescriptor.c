#include "ReportDescriptor.h"
#include "Public.h"

static const UCHAR JoystickReportDesc[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x00,					   //   REPORT_ID ()  
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x80,                    //     LOGICAL_MINIMUM (-128)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

static const UCHAR ControlReportDesc[] = {
	0x06, 0x00, 0xff,                    // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                          // USAGE (Vendor Usage 1)
    0xa1, 0x01,                          // COLLECTION (Application)
	0x85, REPORT_ID_CONTROL,			 //   REPORT_ID (REPORT_ID_CONTROL)  
    0x15, 0x00,                          //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,                    //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                          //   REPORT_SIZE  (8)   - bits
    0x95, 0x04,                          //   REPORT_COUNT (4)  - Bytes
    0x09, 0x02,                          //   USAGE (Vendor Usage 1)
    0x91, 0x02,                          //   OUTPUT (Data,Var,Abs)
    0xc0                                 // END_COLLECTION
};

USHORT ReportDescriptorSize = JOY_COUNT * sizeof(JoystickReportDesc) + sizeof(ControlReportDesc);
UCHAR ReportDescriptor[JOY_COUNT * sizeof(JoystickReportDesc) + sizeof(ControlReportDesc)];

void initReportDescriptor()
{
	char*	dest = (char*) ReportDescriptor;
	BYTE	i;

	for (i = 0; i < JOY_COUNT; i++)
	{
		RtlCopyMemory(
			dest, 
			JoystickReportDesc, 
			sizeof(JoystickReportDesc));

		dest[7] = REPORT_ID_JOY(i);
		dest += sizeof(JoystickReportDesc);
	}

	RtlCopyMemory(
		dest,
		ControlReportDesc,
		sizeof(ControlReportDesc));
};