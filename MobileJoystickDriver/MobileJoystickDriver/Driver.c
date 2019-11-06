#include "Driver.h"
#include "Trace.h"
#include "HID.h"

#ifdef USE_WPP
#include "driver.tmh"
#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, EvtDeviceAdd)
#pragma alloc_text (PAGE, EvtCleanup)
#pragma alloc_text (PAGE, MJCreateDevice)
#pragma alloc_text (PAGE, MJCreateDefaultQueue)
#pragma alloc_text (PAGE, MJCreateReadReportQueue)
#endif

// ==================================================
// DriverEntry
// ==================================================

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

	TRACE_INIT( DriverObject, RegistryPath );
	TRACE_INFO("DriverEntry Entry");

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.EvtCleanupCallback = EvtCleanup;
	WDF_DRIVER_CONFIG_INIT(&config, EvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE
                             );

    if (!NT_SUCCESS(status))
	{
        TRACE_ERROR("WdfDriverCreate failed [status_%08X]", status);
		TRACE_CLEANUP(DriverObject);
        return status;
    }

	TRACE_INFO("DriverEntry Exit");
    return status;
}

// ==================================================
// EvtDeviceAdd
// ==================================================

NTSTATUS
EvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);
    PAGED_CODE();
    TRACE_INFO("EvtDeviceAdd Entry");

	status = MJCreateDevice(DeviceInit);

    TRACE_INFO("EvtDeviceAdd Exit");
    return status;
}

// ==================================================
// DriverCleanup
// ==================================================

VOID
EvtCleanup(
    _In_ WDFOBJECT DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);
    PAGED_CODE ();
	TRACE_CLEANUP( WdfDriverWdmGetDriverObject(DriverObject) );
}

// ==================================================
// MJCreateDevice
// ==================================================

NTSTATUS MJCreateDevice(_Inout_ PWDFDEVICE_INIT DeviceInit)
{
	NTSTATUS				status;
	UCHAR					PNPMinor;
	WDF_OBJECT_ATTRIBUTES	deviceAttributes;
	WDFDEVICE				device;
	DEVICE_CONTEXT*			deviceContext;
	BYTE					deviceIndex;

	PAGED_CODE();
	
    WdfFdoInitSetFilter(DeviceInit);

	PNPMinor = IRP_MN_QUERY_ID;
    status = WdfDeviceInitAssignWdmIrpPreprocessCallback(
				DeviceInit,
				EvtPNPQueryID,
				IRP_MJ_PNP,
				&PNPMinor, 
				1
				); 
	
	if (!NT_SUCCESS(status))
	{
		TRACE_ERROR("WdfDeviceInitAssignWdmIrpPreprocessCallback failed [status_%08X]", status);
		return status;
	}

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
				&deviceAttributes, 
				DEVICE_CONTEXT);

    status = WdfDeviceCreate(
				&DeviceInit, 
				&deviceAttributes, 
				&device);
	
    if (!NT_SUCCESS(status))
	{
		TRACE_ERROR("WdfDeviceCreate failed [status_%08X]", status);
		return status;
	}

	deviceContext = WdfObjectGet_DEVICE_CONTEXT(device);
	for (deviceIndex = 0; deviceIndex < JOY_COUNT; deviceIndex++)
	{
		deviceContext->input[deviceIndex].reportId = REPORT_ID_JOY(deviceIndex);
		deviceContext->input[deviceIndex].state.xAxis = 0;
		deviceContext->input[deviceIndex].state.yAxis = 0;
		deviceContext->input[deviceIndex].state.buttons = 0;
	}

	status = MJCreateDefaultQueue(device);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	
	status = MJCreateReadReportQueue(device);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	return status;
}

// ==================================================
// DriverCleanup
// ==================================================

NTSTATUS MJCreateDefaultQueue(_In_ WDFDEVICE Device)
{
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;
	WDFQUEUE queue;

    PAGED_CODE();

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchParallel
        );

	queueConfig.EvtIoInternalDeviceControl = EvtHIDInternalDeviceControl;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue
                 );

    if (!NT_SUCCESS(status)) 
	{
        TRACE_ERROR("WdfIoQueueCreate (defaultQueue) failed [status_%08X]", status);
        return status;
    }

	return status;
}

// ==================================================
// DriverCleanup
// ==================================================

NTSTATUS MJCreateReadReportQueue(_In_ WDFDEVICE Device)
{
	NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;
	DEVICE_CONTEXT* deviceContext;

    PAGED_CODE();

    WDF_IO_QUEUE_CONFIG_INIT(
		&queueConfig,
		WdfIoQueueDispatchManual
		);
	
	queueConfig.PowerManaged = WdfFalse;
	deviceContext = WdfObjectGet_DEVICE_CONTEXT(Device);

	status = WdfIoQueueCreate(
				Device,
				&queueConfig,
				WDF_NO_OBJECT_ATTRIBUTES,
				&deviceContext->readReportQueue
				);

	if (!NT_SUCCESS(status)) 
	{
        TRACE_ERROR("WdfIoQueueCreate (readReportQueue) failed [status_%08X]", status);
        return status;
    }

    return status;
}