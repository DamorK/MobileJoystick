#include "drivermanager.h"

#define INITGUID
#include <Windows.h>
#include <SetupAPI.h>
#include <HidClass.h>
#include "utl/logger.h"
#include "drv/driver.h"
#include "Public.h"

EXTERN_C
{
    #include <hidsdi.h>
}

#define UP_VENDOR_SPECIFIC 0xff00
#define U_VENDOR_SPECIFIC 0x0001

namespace drv
{

DriverManager::DriverManager()
    : fileHandle(INVALID_HANDLE_VALUE)
{}

DriverManager::~DriverManager()
{
    clear();
}

void DriverManager::findDriver()
{
    WLOGGER << "Looking for a driver..." << std::endl;

    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_HID,
                                                 NULL, NULL,
                                                 DIGCF_DEVICEINTERFACE);

    SP_DEVICE_INTERFACE_DATA interfaceData;
    ZeroMemory(&interfaceData, sizeof(interfaceData));
    interfaceData.cbSize = sizeof(interfaceData);

    BYTE buffer[256];
    PSP_DEVICE_INTERFACE_DETAIL_DATA_W interfaceDetails = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W) buffer;
    DWORD index = 0;

    // enumerate all HID interfaces

    while (SetupDiEnumDeviceInterfaces(deviceInfoSet,
                                       NULL,
                                       &GUID_DEVINTERFACE_HID,
                                       index,
                                       &interfaceData))
    {
       WLOGGER << L"HID interface found" << std::endl;
       index ++;
       interfaceDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

       if (FALSE == SetupDiGetDeviceInterfaceDetail(deviceInfoSet,
                                                    &interfaceData,
                                                    interfaceDetails,
                                                    sizeof(buffer),
                                                    NULL, NULL))
       {
           WLOGGER << L"- SetupDiGetDeviceInterfaceDetail failed: " << ::GetLastError() << std::endl;
           continue;
       }

       WLOGGER << L"- Path: " << interfaceDetails->DevicePath << std::endl;

       fileHandle = openControlDevice(interfaceDetails->DevicePath);
       if (fileHandle != INVALID_HANDLE_VALUE)
       {
           WLOGGER << L"- Mobile Joystick Control!" << std::endl;
           for (BYTE i = 0; i < JOY_COUNT; i++)
               instances.push_back(new Driver(fileHandle, i));
           break;
       }
    }

    if (deviceInfoSet)
    {
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
    }
}

void DriverManager::clear()
{
    foreach (Driver* drv, instances)
        delete drv;
    instances.clear();
    if (fileHandle != INVALID_HANDLE_VALUE)
        CloseHandle(fileHandle);
}

int DriverManager::getDriverIndex(const Driver *driver)
{
    for (int i = 0; i < instances.size(); i++)
        if (instances[i] == driver)
            return i;
    return -1;
}

HANDLE DriverManager::openControlDevice(LPWSTR devicePath)
{
    HANDLE h = CreateFileW(devicePath,
                           GENERIC_WRITE,
                           0, NULL,
                           OPEN_EXISTING, 0, NULL);

    if (h == INVALID_HANDLE_VALUE)
    {
        WLOGGER << L"- CreateFileW failed: " << ::GetLastError() << std::endl;
        return INVALID_HANDLE_VALUE;
    }

    HIDD_ATTRIBUTES attributes;
    PHIDP_PREPARSED_DATA preparsedData;
    HIDP_CAPS caps;
    attributes.Size = sizeof(HIDD_ATTRIBUTES);

    if (!HidD_GetAttributes(h, &attributes))
    {
        WLOGGER << L"- HidD_GetAttributes failed: " << ::GetLastError() << std::endl;
        goto failed;
    }

    if (attributes.VendorID != VENDOR_ID_C || attributes.ProductID != PRODUCT_ID_C)
        goto failed;

    if (!HidD_GetPreparsedData(h, &preparsedData))
    {
        WLOGGER << L"- HidD_GetPreparsedData failed: " << ::GetLastError() << std::endl;
        goto failed;
    }

    if (!HidP_GetCaps(preparsedData, &caps))
    {
        WLOGGER << L"- HidP_GetCaps failed: " << ::GetLastError() << std::endl;
        goto failed;
    }

    if (caps.UsagePage != UP_VENDOR_SPECIFIC || caps.Usage != U_VENDOR_SPECIFIC)
        goto failed;

    HidD_SetNumInputBuffers(h, 10);
    return h;

failed:
    CloseHandle(h);
    return INVALID_HANDLE_VALUE;
}


}
