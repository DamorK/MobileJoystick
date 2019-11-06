#pragma once

#include "WdfHeaders.h"

//
// Internal Device Control
//
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL EvtHIDInternalDeviceControl;

NTSTATUS HIDGetDeviceAttributes(WDFREQUEST req);
NTSTATUS HIDGetDeviceDescriptor(WDFREQUEST req);
NTSTATUS HIDGetReportDescriptor(WDFREQUEST req);
NTSTATUS HIDGetString(WDFREQUEST req);
NTSTATUS HIDDeferReadReportRequest(WDFREQUEST req, WDFQUEUE readReportQueue);
NTSTATUS HIDCompleteReadReportRequest(WDFQUEUE readReportQueue, BYTE deviceIndex);
NTSTATUS HIDWriteReport(WDFREQUEST req, PDEVICE_CONTEXT deviceContext);

//
// PNP
//
EVT_WDFDEVICE_WDM_IRP_PREPROCESS EvtPNPQueryID;

//
// Others
//
PCHAR IOCTLGetDesc(ULONG controlCode);