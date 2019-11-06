
#include "Driver.h"
#include "HID.h"
#include "ReportDescriptor.h"
#include "Trace.h"

#include <hidport.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, HIDGetDeviceAttributes)
#pragma alloc_text(PAGE, HIDGetDeviceDescriptor)
#pragma alloc_text(PAGE, HIDGetReportDescriptor)
#pragma alloc_text(PAGE, EvtPNPQueryID)
#endif

#ifdef USE_WPP
#include "hid.tmh"
#endif

// ==================================================
// EvtHIDInternalDeviceControl
// ==================================================

VOID
EvtHIDInternalDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
{
	NTSTATUS status;
	WDFDEVICE device;
	DEVICE_CONTEXT* deviceContext;
	UNREFERENCED_PARAMETER(InputBufferLength);
	UNREFERENCED_PARAMETER(OutputBufferLength);
	
	switch (IoControlCode)
	{
	case IOCTL_HID_READ_REPORT:
	case IOCTL_HID_GET_INPUT_REPORT:
	case IOCTL_HID_WRITE_REPORT:
	case IOCTL_HID_SET_OUTPUT_REPORT:
		break;
	default:
		TRACE_INFO(
			"EvtHIDInternalDeviceControl Entry [Queue_0x%p Request_0x%p "
			"OutputBufferLength_%d InputBufferLength_%d IoControlCode_%s", 
			Queue, Request, (int) OutputBufferLength, (int) InputBufferLength, IOCTLGetDesc(IoControlCode));
	}
	
	switch (IoControlCode)
	{
	case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
		status = HIDGetDeviceAttributes(Request);
		break;
	case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
		status = HIDGetDeviceDescriptor(Request);
		break;
	case IOCTL_HID_GET_REPORT_DESCRIPTOR:
		status = HIDGetReportDescriptor(Request);
		break;
	case IOCTL_HID_GET_STRING:
		status = HIDGetString(Request);
		break;
	case IOCTL_HID_READ_REPORT:
	case IOCTL_HID_GET_INPUT_REPORT:
		device = WdfIoQueueGetDevice(Queue);
		deviceContext = WdfObjectGet_DEVICE_CONTEXT(device);
		status = HIDDeferReadReportRequest(Request, deviceContext->readReportQueue);
		break;
	case IOCTL_HID_WRITE_REPORT:
	case IOCTL_HID_SET_OUTPUT_REPORT:
		device = WdfIoQueueGetDevice(Queue);
		deviceContext = WdfObjectGet_DEVICE_CONTEXT(device);
		status = HIDWriteReport(Request, deviceContext);
		break;
	default:
		status = STATUS_NOT_SUPPORTED;
		break;
	}
	
	if (!NT_SUCCESS(status))
		WdfRequestComplete(Request, status);
}

// ==================================================
// HIDGetDeviceAttributes
// ==================================================

NTSTATUS HIDGetDeviceAttributes(WDFREQUEST req)
{
	NTSTATUS status;
	WDFMEMORY outMemory;
	HID_DEVICE_ATTRIBUTES devAttr;

	status = WdfRequestRetrieveOutputMemory(req, &outMemory);
	if (!NT_SUCCESS(status))
	{	
		TRACE_ERROR("WdfRequestRetrieveOutputMemory failed [status_%08X]", status);
		return status;
	}	

	devAttr.Size = sizeof(HID_DEVICE_ATTRIBUTES);
	devAttr.VendorID = VENDOR_ID_C;
	devAttr.ProductID = PRODUCT_ID_C;
	devAttr.VersionNumber = VERSION_ID_C;
	
	status = WdfMemoryCopyFromBuffer(outMemory, 0, (PVOID) &devAttr, sizeof(HID_DEVICE_ATTRIBUTES));
	if (!NT_SUCCESS(status))
	{
		TRACE_ERROR("WdfMemoryCopyFromBuffer failed [status_%08X]", status);
		return status;
	}

	WdfRequestCompleteWithInformation(req, STATUS_SUCCESS, sizeof(HID_DEVICE_ATTRIBUTES));
	return STATUS_SUCCESS;
}

// ==================================================
// HIDGetDeviceDescriptor
// ==================================================

NTSTATUS HIDGetDeviceDescriptor(WDFREQUEST req)
{
	NTSTATUS status;
	WDFMEMORY outMemory;
	HID_DESCRIPTOR desc;

	status = WdfRequestRetrieveOutputMemory(req, &outMemory);
	if (!NT_SUCCESS(status))
	{	
		TRACE_ERROR("WdfRequestRetrieveOutputMemory failed [status_%08X]", status);
		return status;
	}		

	desc.bLength = sizeof(HID_DESCRIPTOR);
	desc.bDescriptorType = HID_HID_DESCRIPTOR_TYPE;
	desc.bcdHID = 0x100;
	desc.bCountry = 0;
	desc.bNumDescriptors = 1;
	desc.DescriptorList[0].bReportType = HID_REPORT_DESCRIPTOR_TYPE;
	desc.DescriptorList[0].wReportLength = ReportDescriptorSize;

	status = WdfMemoryCopyFromBuffer(outMemory, 0, (PVOID) &desc, sizeof(HID_DESCRIPTOR));
	if (!NT_SUCCESS(status))
	{
		TRACE_ERROR("WdfMemoryCopyFromBuffer failed [status_%08X]", status);
		return status;
	}

	WdfRequestCompleteWithInformation(req, STATUS_SUCCESS, sizeof(HID_DESCRIPTOR));
	return STATUS_SUCCESS;
}

// ==================================================
// HIDGetReportDescriptor
// ==================================================

NTSTATUS HIDGetReportDescriptor(WDFREQUEST req)
{
	NTSTATUS status;
	WDFMEMORY outMemory;

	initReportDescriptor();

	status = WdfRequestRetrieveOutputMemory(req, &outMemory);
	if (!NT_SUCCESS(status))
	{	
		TRACE_ERROR("WdfRequestRetrieveOutputMemory failed [status_%08X]", status);
		return status;
	}	

	status = WdfMemoryCopyFromBuffer(outMemory, 0, (PVOID) ReportDescriptor, ReportDescriptorSize);
	if (!NT_SUCCESS(status))
	{
		TRACE_ERROR("WdfMemoryCopyFromBuffer failed [status_%08X]", status);
		return status;
	}

	WdfRequestCompleteWithInformation(req, STATUS_SUCCESS, ReportDescriptorSize);
	return STATUS_SUCCESS;
}

// ==================================================
// HIDGetString
// ==================================================

NTSTATUS HIDGetString(WDFREQUEST req)
{
	NTSTATUS				status;
	WDF_REQUEST_PARAMETERS	params;
	PUSHORT					IOCTLParams;
	PWCHAR					string = NULL;
	size_t					size;
	WDFMEMORY				outMemory;

	WDF_REQUEST_PARAMETERS_INIT(&params);
	WdfRequestGetParameters(req, &params);

	IOCTLParams = (PUSHORT) params.Parameters.DeviceIoControl.Type3InputBuffer;
	switch (IOCTLParams[0])
	{
	case HID_STRING_ID_IMANUFACTURER:
		string = VENDOR_STR;
		break;
	case HID_STRING_ID_IPRODUCT:
		string = PRODUCT_STR;
		break;
	case HID_STRING_ID_ISERIALNUMBER:
		string = VERSION_STR;
		break;
	default:
		return STATUS_INVALID_PARAMETER;
	}

	status = WdfRequestRetrieveOutputMemory(req, &outMemory);
	if (!NT_SUCCESS(status))
	{	
		TRACE_ERROR("WdfRequestRetrieveOutputMemory failed [status_%08X]", status);
		return status;
	}	
	
	size = (wcslen(string) + 1) * sizeof(WCHAR);
	status = WdfMemoryCopyFromBuffer(outMemory, 0, string, size);
	if (!NT_SUCCESS(status))
	{
		TRACE_ERROR("WdfMemoryCopyFromBuffer failed [status_%08X]", status);
		return status;
	}

	WdfRequestCompleteWithInformation(req, STATUS_SUCCESS, size);
	return STATUS_SUCCESS;
}

// ==================================================
// HIDDeferReadReportRequest
// ==================================================

NTSTATUS HIDDeferReadReportRequest(WDFREQUEST req, WDFQUEUE readReportQueue)
{
	NTSTATUS status;

	status = WdfRequestForwardToIoQueue(req, readReportQueue);

	if (!NT_SUCCESS(status))
	{
		TRACE_ERROR("HIDDeferReadReportRequest:WdfRequestForwartToIoQueue failed [status_%08X]", status);
		return status;
	}

	return STATUS_SUCCESS;
}

// ==================================================
// HIDDeferReadReportRequest
// ==================================================

NTSTATUS HIDCompleteReadReportRequest(WDFQUEUE readReportQueue, BYTE deviceIndex)
{
	NTSTATUS		status = STATUS_SUCCESS;
	WDFDEVICE		device;
	DEVICE_CONTEXT* deviceContext;
	WDFREQUEST		req;
	WDFMEMORY		mem;

	if (deviceIndex >= JOY_COUNT)
	{
		TRACE_ERROR("HIDCompleteReadReportRequest:Invalid device index [%d]", deviceIndex);
		return STATUS_INVALID_PARAMETER;
	}

	device = WdfIoQueueGetDevice(readReportQueue);
	deviceContext = WdfObjectGet_DEVICE_CONTEXT(device);

	if (NT_SUCCESS(status = WdfIoQueueRetrieveNextRequest(readReportQueue, &req)))
	{
		status = WdfRequestRetrieveOutputMemory(req, &mem);
		if (!NT_SUCCESS(status))
		{
			TRACE_ERROR("HIDCompleteReadReportRequest:WdfRequestRetrieveOutputMemory failed [status_%08X]", status);
			WdfRequestComplete(req, status);
			return status;
		}
		
		TRACE_INFO("Device %d state: %d", deviceIndex, deviceContext->input[deviceIndex].state.buttons);

		status = WdfMemoryCopyFromBuffer(mem, 0, &deviceContext->input[deviceIndex], sizeof(GET_JOY_STATE));
		if (!NT_SUCCESS(status))
		{
			TRACE_ERROR("HIDCompleteReadReportRequest:WdfMemoryCopyFromBuffer failed [status_%08X]", status);
			WdfRequestComplete(req, status);
			return status;
		}

		WdfRequestCompleteWithInformation(req, status, sizeof(GET_JOY_STATE));
	}
	return status;
}

// ==================================================
// HIDWriteReport
// ==================================================

NTSTATUS HIDWriteReport(WDFREQUEST req, PDEVICE_CONTEXT deviceContext)
{
	NTSTATUS			status;
    PHID_XFER_PACKET    transferPacket = NULL;
	PSET_JOY_STATE		message;
	BYTE				deviceIndex;

	transferPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(req)->UserBuffer;

	if (transferPacket->reportBufferLen < sizeof(SET_JOY_STATE))
	{
		TRACE_ERROR("HIDWriteReport:User buffer is too small");
		return STATUS_BUFFER_TOO_SMALL;
	}

	message = (PSET_JOY_STATE) transferPacket->reportBuffer;
	deviceIndex = message->deviceNo;

	RtlCopyMemory(
		&deviceContext->input[deviceIndex].state,
		&message->state,
		sizeof(JOY_STATE));

	status = HIDCompleteReadReportRequest(deviceContext->readReportQueue, deviceIndex);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	WdfRequestCompleteWithInformation(req, STATUS_SUCCESS, sizeof(SET_JOY_STATE));
	return STATUS_SUCCESS;
}

// ==================================================
// EvtPNPQueryID
// ==================================================

NTSTATUS
EvtPNPQueryID(
    _In_ WDFDEVICE Device,
    _Inout_ PIRP Irp
    )
{
	NTSTATUS			status;
    PIO_STACK_LOCATION  currentStack;
	PIO_STACK_LOCATION	previousStack;
    PDEVICE_OBJECT      deviceObject;
    PWCHAR              buffer;
	PWCHAR				id;
	size_t				length;
	size_t				size;

    TRACE_INFO("EvtPNPQueryID Entry\n");
	
	currentStack = IoGetCurrentIrpStackLocation (Irp);
	previousStack = currentStack + 1;
    deviceObject = WdfDeviceWdmGetDeviceObject(Device); 

    // 
    // This check is required to filter out QUERY_IDs forwarded
    // by the HIDCLASS for the parent FDO. These IDs are sent
    // by PNP manager for the parent FDO if you root-enumerate this driver.
    //
	if (previousStack->DeviceObject == deviceObject)
	{
		return Irp->IoStatus.Status;
	}

	switch (currentStack->Parameters.QueryId.IdType)
    {
	case BusQueryInstanceID:
		id = L"0000";
		break;
    case BusQueryDeviceID:
    case BusQueryHardwareIDs:
		id = DEVICE_STR;
		break;
	default:
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return Irp->IoStatus.Status;
	}

	length = wcslen(id);
	size = (length + 2) * sizeof(WCHAR);
    buffer = (PWCHAR) ExAllocatePool(NonPagedPool, size);

    if (buffer) 
    {
		buffer[length] = buffer[length + 1] = 0;
        RtlCopyMemory(
			buffer, 
			id, 
			length * sizeof(wchar_t));
        Irp->IoStatus.Information = (ULONG_PTR) buffer;
        Irp->IoStatus.Status = STATUS_SUCCESS;
    }
    else 
    {
        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
    }

    IoCompleteRequest (Irp, IO_NO_INCREMENT);
	status = Irp->IoStatus.Status;
    TRACE_INFO("EvtPNPQueryID Exit [status_%08X]\n", status);
    return status;
}

// ==================================================
// IOCTLGetDesc
// ==================================================

#define MAP_CASE(X) case X: return #X;
PCHAR IOCTLGetDesc(ULONG controlCode)
{
	switch (controlCode)
	{
		MAP_CASE(IOCTL_GET_PHYSICAL_DESCRIPTOR)
		MAP_CASE(IOCTL_HID_ACTIVATE_DEVICE)
		MAP_CASE(IOCTL_HID_DEACTIVATE_DEVICE)
		MAP_CASE(IOCTL_HID_GET_DEVICE_ATTRIBUTES)
		MAP_CASE(IOCTL_HID_GET_DEVICE_DESCRIPTOR)
		MAP_CASE(IOCTL_HID_GET_FEATURE)
		MAP_CASE(IOCTL_HID_GET_INDEXED_STRING)
		MAP_CASE(IOCTL_HID_GET_INPUT_REPORT)
		MAP_CASE(IOCTL_HID_GET_REPORT_DESCRIPTOR)
		MAP_CASE(IOCTL_HID_GET_STRING)
		MAP_CASE(IOCTL_HID_READ_REPORT)
		MAP_CASE(IOCTL_HID_SET_FEATURE)
		MAP_CASE(IOCTL_HID_SET_OUTPUT_REPORT)
		MAP_CASE(IOCTL_HID_WRITE_REPORT)
		//...
	default:
		return "OTHER IOCTL";
	};
}
#undef MAP_CASE