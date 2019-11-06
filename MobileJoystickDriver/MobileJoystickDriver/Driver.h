#pragma once

#include "WdfHeaders.h"
#include "Public.h"

// 
// Declare Device Context
//

typedef struct _DEVICE_CONTEXT
{
	WDFQUEUE readReportQueue;
	GET_JOY_STATE input[JOY_COUNT];
}
DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE(DEVICE_CONTEXT);

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE				DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD		EvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP	EvtCleanup;

NTSTATUS MJCreateDevice(_Inout_ PWDFDEVICE_INIT DeviceInit);
NTSTATUS MJCreateDefaultQueue(_In_ WDFDEVICE Device);
NTSTATUS MJCreateReadReportQueue(_In_ WDFDEVICE Device);
