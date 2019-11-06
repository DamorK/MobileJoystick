#pragma once

#include <windef.h>

//
// Driver data
//

#define VENDOR_ID	DA90
#define PRODUCT_ID	1234
#define VERSION_ID	0100

#define VENDOR_STR	L"DamorK"
#define PRODUCT_STR L"Mobile Joystick"
#define VERSION_STR L"1.00"

#define CAT(a,b)	a ## b
#define CATSTR(a,b) a ## #b
#define C_HEX(hex)	CAT(0x, hex)
#define W_STR(a)	CATSTR(L, a)

#define VENDOR_ID_C		C_HEX(VENDOR_ID)
#define PRODUCT_ID_C	C_HEX(PRODUCT_ID)
#define VERSION_ID_C	C_HEX(VERSION_ID)

#define DEVICE_STR L"root\\VID_" W_STR(VENDOR_ID) L"&PID_" W_STR(PRODUCT_ID) L"&REV_" W_STR(VERSION_ID)

//
// Reports
//

#define JOY_COUNT			4
#define REPORT_ID_JOY(i)	(i+1)
#define REPORT_ID_CONTROL	0xff

#pragma pack(1)

typedef struct _JOY_STATE
{
	BYTE xAxis;
	BYTE yAxis;
	BYTE buttons; // <-- max 8 buttons
}
JOY_STATE, *PJOY_STATE;

typedef struct _GET_JOY_STATE
{
	BYTE reportId;
	JOY_STATE state;
}
GET_JOY_STATE, *PGET_JOY_STATE;

typedef struct _SET_JOY_STATE
{
	BYTE reportId;
	BYTE deviceNo;
	JOY_STATE state;
}
SET_JOY_STATE, *PSET_JOY_STATE;

#pragma pack()
