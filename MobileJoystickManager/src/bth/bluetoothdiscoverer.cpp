#include "bluetoothdiscoverer.h"
#include "initwinsock.h"
#include "bluetoothconfig.h"
#include "utl/logger.h"
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#pragma comment(lib, "Bthprops.lib")

namespace bth
{
    CHAR devQuerySetBuffer[4096];
    CHAR srvQuerySetBuffer[4096];
    WCHAR devAddresBuffer[1024];

    void BluetoothDiscoverer::run()
    {
        if (!initWinSock())
        {
            emit discoveringFinished("Unable to initialize WinSock library");
            return;
        }

        discoverDevices();
    }

    // ================================================================
    // DEVICES
    // ================================================================

    void BluetoothDiscoverer::discoverDevices()
    {
        WSAQUERYSETW wsaq;
        ZeroMemory(&wsaq, sizeof(wsaq));
        wsaq.dwSize = sizeof(wsaq);
        wsaq.dwNameSpace = NS_BTH;
        wsaq.lpcsaBuffer = NULL;

        HANDLE lookupHandle;
        LPWSAQUERYSETW results = (LPWSAQUERYSET) devQuerySetBuffer;
        DWORD bufferSize = sizeof(devQuerySetBuffer);

        if (WSALookupServiceBeginW(&wsaq, LUP_CONTAINERS, &lookupHandle))
        {
            emitErrorMsg(L"Unable to start discovering remote devices");
            return;
        }

        WLOGGER << L"Discovering devices..." << std::endl;

        while (ERROR_SUCCESS == WSALookupServiceNextW(lookupHandle,
                                                      LUP_RETURN_ADDR | LUP_RETURN_NAME,
                                                      &bufferSize, results))
        {
            if (results->lpszServiceInstanceName && results->dwNumberOfCsAddrs > 0)
            {
                WLOGGER << L"Device found: " << results->lpszServiceInstanceName << std::endl;

                discoverServices(results->lpszServiceInstanceName,
                                 &results->lpcsaBuffer->RemoteAddr);
            }

            bufferSize = sizeof(devQuerySetBuffer);
        }

        emit discoveringFinished(QString());
        WSALookupServiceEnd(lookupHandle);
    }

    // ================================================================
    // SERVICES
    // ================================================================

    void BluetoothDiscoverer::discoverServices(LPWSTR deviceName, LPSOCKET_ADDRESS remoteAddr)
    {
        DWORD addrBufferSize = sizeof(devAddresBuffer) / sizeof(WCHAR);

        if (WSAAddressToStringW(remoteAddr->lpSockaddr, remoteAddr->iSockaddrLength,
                                NULL, devAddresBuffer, &addrBufferSize))
        {
            WLOGGER << L"Cannot retrieve remote address string " << ::GetLastError() << std::endl;
            return;
        }

        WLOGGER << L"Device address: " << devAddresBuffer << std::endl;

        GUID uuid = MJ_SERVICE_UUID;
        WSAQUERYSETW wsaq;
        ZeroMemory(&wsaq, sizeof(wsaq));
        wsaq.dwSize = sizeof(wsaq);
        wsaq.dwNameSpace = NS_BTH;
        wsaq.dwNumberOfCsAddrs = 0;
        wsaq.lpszContext = devAddresBuffer;
        wsaq.lpServiceClassId = &uuid;

        HANDLE lookupHandle;
        LPWSAQUERYSETW results = (LPWSAQUERYSETW) srvQuerySetBuffer;
        DWORD bufferSize = sizeof(srvQuerySetBuffer);

        if (WSALookupServiceBeginW(&wsaq, LUP_FLUSHCACHE, &lookupHandle))
        {
            WLOGGER << L"Unable to start discovering services " << ::GetLastError() << std::endl;
            return;
        }

        while (ERROR_SUCCESS == WSALookupServiceNextW(lookupHandle,
                                                      LUP_RETURN_ADDR | LUP_RETURN_NAME,
                                                      &bufferSize, results))
        {
            if (results->lpszServiceInstanceName && results->dwNumberOfCsAddrs > 0)
            {
                WLOGGER << L"Service found: " << results->lpszServiceInstanceName << std::endl;

                LPSOCKET_ADDRESS addr = &results->lpcsaBuffer->RemoteAddr;
                PSOCKADDR_BTH bthAddr = (PSOCKADDR_BTH) addr->lpSockaddr;

                emit remoteDeviceFound(QString::fromWCharArray(deviceName),
                                       QString::fromWCharArray(devAddresBuffer),
                                       *bthAddr);
            }

            bufferSize = sizeof(srvQuerySetBuffer);
        }

        WSALookupServiceEnd(lookupHandle);
    }

    // ================================================================
    // ERROR
    // ================================================================

    void BluetoothDiscoverer::emitErrorMsg(const wchar_t* caption)
    {
        LPWSTR errorStr;
        DWORD dw = ::GetLastError();

        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK),
            (LPWSTR) &errorStr,
            0,
            NULL);

        emit discoveringFinished(QString().sprintf("%ls:\n%ls", caption, errorStr));
        LocalFree(errorStr);
    }
}
