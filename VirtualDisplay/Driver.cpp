#include "Driver.h"


#pragma region SampleMonitor
//extern  constexpr DWORD IDD_SAMPLE_MONITOR_COUNT = 3;
// Default modes reported for edid-less monitors. The first mode is set as preferred
extern  const struct IndirectSampleMonitor::SampleMonitorMode s_SampleDefaultModes[] =
{
    { 1920, 1080, 60 },
    { 1600,  900, 60 },
    { 1024,  768, 75 },
};

// FOR SAMPLE PURPOSES ONLY, Static info about monitors that will be reported to OS
extern const struct IndirectSampleMonitor s_SampleMonitors[] =
{
    // Modified EDID from Dell S2719DGF
    {
        {
            0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x10,0xAC,0xE6,0xD0,0x55,0x5A,0x4A,0x30,0x24,0x1D,0x01,
            0x04,0xA5,0x3C,0x22,0x78,0xFB,0x6C,0xE5,0xA5,0x55,0x50,0xA0,0x23,0x0B,0x50,0x54,0x00,0x02,0x00,
            0xD1,0xC0,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x58,0xE3,0x00,
            0xA0,0xA0,0xA0,0x29,0x50,0x30,0x20,0x35,0x00,0x55,0x50,0x21,0x00,0x00,0x1A,0x00,0x00,0x00,0xFF,
            0x00,0x37,0x4A,0x51,0x58,0x42,0x59,0x32,0x0A,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,0x00,
            0x53,0x32,0x37,0x31,0x39,0x44,0x47,0x46,0x0A,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFD,0x00,0x28,
            0x9B,0xFA,0xFA,0x40,0x01,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x2C
        },
        {
            { 2560, 1440, 144 },
            { 1920, 1080,  60 },
            { 1024,  768,  60 },
        },
        0
    },
    // Modified EDID from Lenovo Y27fA
    {
        {
            0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x30,0xAE,0xBF,0x65,0x01,0x01,0x01,0x01,0x20,0x1A,0x01,
            0x04,0xA5,0x3C,0x22,0x78,0x3B,0xEE,0xD1,0xA5,0x55,0x48,0x9B,0x26,0x12,0x50,0x54,0x00,0x08,0x00,
            0xA9,0xC0,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x68,0xD8,0x00,
            0x18,0xF1,0x70,0x2D,0x80,0x58,0x2C,0x45,0x00,0x53,0x50,0x21,0x00,0x00,0x1E,0x00,0x00,0x00,0x10,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFD,0x00,
            0x30,0x92,0xB4,0xB4,0x22,0x01,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,0x00,0x4C,
            0x45,0x4E,0x20,0x59,0x32,0x37,0x66,0x41,0x0A,0x20,0x20,0x20,0x00,0x11
        },
        {
            { 3840, 2160,  60 },
            { 1600,  900,  60 },
            { 1024,  768,  60 },
        },
        0
    }
};

extern const size_t s_SampleMonitorsCount = _countof(s_SampleMonitors);
#pragma  endregion



extern "C" DRIVER_INITIALIZE DriverEntry;


EVT_WDF_DRIVER_DEVICE_ADD                        VirtualDriverAdd;
EVT_WDF_DEVICE_D0_ENTRY                          VirtualDisplayD0Entry;
EVT_IDD_CX_DEVICE_IO_CONTROL                     VirtualDisplayDeviceIoControl;
EVT_IDD_CX_PARSE_MONITOR_DESCRIPTION             VirtualDisplayParseMonitorDescription;
EVT_IDD_CX_ADAPTER_INIT_FINISHED                 VirtualDisplayAdapterInitFinished;
EVT_IDD_CX_ADAPTER_COMMIT_MODES                  VirtualDisplayAdapterCommitModes;
EVT_IDD_CX_MONITOR_GET_DEFAULT_DESCRIPTION_MODES VirtualDisplayMonitorGetDefaultDescriptionModes;
EVT_IDD_CX_MONITOR_QUERY_TARGET_MODES            VirtualDisplayMonitorQueryTargetModes;
EVT_IDD_CX_MONITOR_ASSIGN_SWAPCHAIN              VirtualDisplayMonitorAssignSwapChain;
EVT_IDD_CX_MONITOR_UNASSIGN_SWAPCHAIN            VirtualDisplayMonitorUnassignSwapChain;


//----------------
static inline void FillSignalInfo(DISPLAYCONFIG_VIDEO_SIGNAL_INFO& Mode, DWORD Width, DWORD Height, DWORD VSync, bool bMonitorMode)
{
    Mode.totalSize.cx = Mode.activeSize.cx = Width;
    Mode.totalSize.cy = Mode.activeSize.cy = Height;

    // See https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-displayconfig_video_signal_info
    Mode.AdditionalSignalInfo.vSyncFreqDivider = bMonitorMode ? 0 : 1;
    Mode.AdditionalSignalInfo.videoStandard = 255;

    Mode.vSyncFreq.Numerator = VSync;
    Mode.vSyncFreq.Denominator = 1;
    Mode.hSyncFreq.Numerator = VSync * Height;
    Mode.hSyncFreq.Denominator = 1;

    Mode.scanLineOrdering = DISPLAYCONFIG_SCANLINE_ORDERING_PROGRESSIVE;

    Mode.pixelRate = ((UINT64)VSync) * ((UINT64)Width) * ((UINT64)Height);
}
static IDDCX_MONITOR_MODE CreateIddCxMonitorMode(DWORD Width, DWORD Height, DWORD VSync, IDDCX_MONITOR_MODE_ORIGIN Origin = IDDCX_MONITOR_MODE_ORIGIN_DRIVER)
{
    IDDCX_MONITOR_MODE Mode = {};

    Mode.Size = sizeof(Mode);
    Mode.Origin = Origin;
    FillSignalInfo(Mode.MonitorVideoSignalInfo, Width, Height, VSync, true);

    return Mode;
}
static IDDCX_TARGET_MODE CreateIddCxTargetMode(DWORD Width, DWORD Height, DWORD VSync)
{
    IDDCX_TARGET_MODE Mode = {};

    Mode.Size = sizeof(Mode);
    FillSignalInfo(Mode.TargetVideoSignalInfo.targetVideoSignalInfo, Width, Height, VSync, false);

    return Mode;
}






extern "C" NTSTATUS DriverEntry(
    PDRIVER_OBJECT  pDriverObject,
    PUNICODE_STRING pRegistryPath
)
{
    WDF_DRIVER_CONFIG driverConfig;
    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    WDF_DRIVER_CONFIG_INIT(&driverConfig, VirtualDriverAdd);

    NTSTATUS Status = WdfDriverCreate(pDriverObject, pRegistryPath, &attributes, &driverConfig, WDF_NO_HANDLE);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    return Status;
}




_Use_decl_annotations_
NTSTATUS VirtualDriverAdd(_In_ WDFDRIVER driver, _Inout_ PWDFDEVICE_INIT pDeviceInit)
{
    UNREFERENCED_PARAMETER(driver);

    WDF_PNPPOWER_EVENT_CALLBACKS PnpPowerEventCallback;
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&PnpPowerEventCallback);
    PnpPowerEventCallback.EvtDeviceD0Entry = VirtualDisplayD0Entry;
    WdfDeviceInitSetPnpPowerEventCallbacks(pDeviceInit, &PnpPowerEventCallback);

    IDD_CX_CLIENT_CONFIG config;
    IDD_CX_CLIENT_CONFIG_INIT(&config);

    config.EvtIddCxDeviceIoControl =                   VirtualDisplayDeviceIoControl;
    config.EvtIddCxParseMonitorDescription =           VirtualDisplayParseMonitorDescription;
    config.EvtIddCxAdapterInitFinished =               VirtualDisplayAdapterInitFinished;
    config.EvtIddCxAdapterCommitModes =                VirtualDisplayAdapterCommitModes;
    config.EvtIddCxMonitorGetDefaultDescriptionModes = VirtualDisplayMonitorGetDefaultDescriptionModes;
    config.EvtIddCxMonitorQueryTargetModes =           VirtualDisplayMonitorQueryTargetModes;
    config.EvtIddCxMonitorAssignSwapChain =            VirtualDisplayMonitorAssignSwapChain;
    config.EvtIddCxMonitorUnassignSwapChain =          VirtualDisplayMonitorUnassignSwapChain;

    auto status = IddCxDeviceInitConfig(pDeviceInit, &config);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, IndirectDeviceContextWrapper);
    attributes.EvtCleanupCallback = [](WDFOBJECT object)
        {
            auto* pContext = WdfObjectGet_IndirectDeviceContextWrapper(object);
            if (pContext)
            {
                pContext->Cleanup();
            }
        };
    WDFDEVICE device = nullptr;

    status = WdfDeviceCreate(&pDeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //TODO:: defensive condition needed
    status = IddCxDeviceInitialize(device);

    auto* pContext = WdfObjectGet_IndirectDeviceContextWrapper(device);
    pContext->pContext = new IndirectDeviceContext(device);
    return status;
}

_Use_decl_annotations_
NTSTATUS
VirtualDisplayD0Entry(_In_ WDFDEVICE Device, _In_  WDF_POWER_DEVICE_STATE PreviousState)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(PreviousState);

    auto* pContext = WdfObjectGet_IndirectDeviceContextWrapper(Device);
    if (pContext == nullptr)
    {
        return STATUS_DEVICE_DATA_ERROR;
    }

    pContext->pContext->InitAdaptr();
    return STATUS_SUCCESS;
}

_Use_decl_annotations_
VOID
VirtualDisplayDeviceIoControl(_In_ WDFDEVICE Device, _In_ WDFREQUEST Request, _In_ size_t OutputBufferLength, _In_  size_t InputBufferLength, _In_ ULONG IoControlCode)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(Request);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    auto* ctx = WdfObjectGet_IndirectDeviceContextWrapper(Device);
    

    
}



_Use_decl_annotations_
NTSTATUS
VirtualDisplayParseMonitorDescription(
    _In_ const IDARG_IN_PARSEMONITORDESCRIPTION* pInArgs,
    _Out_ IDARG_OUT_PARSEMONITORDESCRIPTION* pOutArgs)
{
    
        // ==============================
    // TODO: In a real driver, this function would be called to generate monitor modes for an EDID by parsing it. In
    // this sample driver, we hard-code the EDID, so this function can generate known modes.
    // ==============================

    pOutArgs->MonitorModeBufferOutputCount = IndirectSampleMonitor::szModeList;

    if (pInArgs->MonitorModeBufferInputCount < IndirectSampleMonitor::szModeList)
    {
        // Return success if there was no buffer, since the caller was only asking for a count of modes
        return (pInArgs->MonitorModeBufferInputCount > 0) ? STATUS_BUFFER_TOO_SMALL : STATUS_SUCCESS;
    }
    else
    {
        // In the sample driver, we have reported some static information about connected monitors
        // Check which of the reported monitors this call is for by comparing it to the pointer of
        // our known EDID blocks.

        if (pInArgs->MonitorDescription.DataSize != IndirectSampleMonitor::szEdidBlock)
            return STATUS_INVALID_PARAMETER;

        DWORD SampleMonitorIdx = 0;
        for (; SampleMonitorIdx < ARRAYSIZE(s_SampleMonitors); SampleMonitorIdx++)
        {  
            
            if (memcmp(pInArgs->MonitorDescription.pData, s_SampleMonitors[SampleMonitorIdx].pEdidBlock, IndirectSampleMonitor::szEdidBlock) == 0)
            {
                // Copy the known modes to the output buffer
                for (DWORD ModeIndex = 0; ModeIndex < IndirectSampleMonitor::szModeList; ModeIndex++)
                {
                    pInArgs->pMonitorModes[ModeIndex] = CreateIddCxMonitorMode(
                        s_SampleMonitors[SampleMonitorIdx].pModeList[ModeIndex].Width,
                        s_SampleMonitors[SampleMonitorIdx].pModeList[ModeIndex].Height,
                        s_SampleMonitors[SampleMonitorIdx].pModeList[ModeIndex].VSync,
                        IDDCX_MONITOR_MODE_ORIGIN_MONITORDESCRIPTOR
                    );
                }

                // Set the preferred mode as represented in the EDID
                pOutArgs->PreferredMonitorModeIdx = s_SampleMonitors[SampleMonitorIdx].ulPreferredModeIdx;

                return STATUS_SUCCESS;
            }
        }

        // This EDID block does not belong to the monitors we reported earlier
        return STATUS_INVALID_PARAMETER;
    }    
}

_Use_decl_annotations_
NTSTATUS
VirtualDisplayAdapterInitFinished( _In_ IDDCX_ADAPTER AdapterObject, _In_  const IDARG_IN_ADAPTER_INIT_FINISHED* pInArgs)
{
    // This is called when the OS has finished setting up the adapter for use by the IddCx driver. It's now possible
    // to report attached monitors.
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(pInArgs);
    auto* contextWrapper = WdfObjectGet_IndirectDeviceContextWrapper(AdapterObject);
    if (contextWrapper == nullptr || contextWrapper->pContext == nullptr)
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (!NT_SUCCESS(pInArgs->AdapterInitStatus))
    {
        return STATUS_UNSUCCESSFUL;
    }
    auto pContext = contextWrapper->pContext;
    for (DWORD i=0; i < IDD_SAMPLE_MONITOR_COUNT;i++)
    {
        pContext->FinishInit(i);
    }
    return 0;
}

_Use_decl_annotations_
NTSTATUS
VirtualDisplayAdapterCommitModes(_In_ IDDCX_ADAPTER AdapterObject, _In_ const IDARG_IN_COMMITMODES* pInArgs)
{
    UNREFERENCED_PARAMETER(AdapterObject);
    UNREFERENCED_PARAMETER(pInArgs);
    // For the sample, do nothing when modes are picked - the swap-chain is taken care of by IddCx

    // ==============================
    // TODO: In a real driver, this function would be used to reconfigure the device to commit the new modes. Loop
    // through pInArgs->pPaths and look for IDDCX_PATH_FLAGS_ACTIVE. Any path not active is inactive (e.g. the monitor
    // should be turned off).
    // ==============================
    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS VirtualDisplayMonitorGetDefaultDescriptionModes(_In_ IDDCX_MONITOR MonitorObject, 
    _In_ const IDARG_IN_GETDEFAULTDESCRIPTIONMODES* pInArgs, _Out_ IDARG_OUT_GETDEFAULTDESCRIPTIONMODES* pOutArgs)
{
    UNREFERENCED_PARAMETER(MonitorObject);
    UNREFERENCED_PARAMETER(pInArgs);
    UNREFERENCED_PARAMETER(pOutArgs);

    // ==============================
    // TODO: In a real driver, this function would be called to generate monitor modes for a monitor with no EDID.
    // Drivers should report modes that are guaranteed to be supported by the transport protocol and by nearly all
    // monitors (such 640x480, 800x600, or 1024x768). If the driver has access to monitor modes from a descriptor other
    // than an EDID, those modes would also be reported here.
    // ==============================

    if (pInArgs->DefaultMonitorModeBufferInputCount == 0)
    {
        pOutArgs->DefaultMonitorModeBufferOutputCount = ARRAYSIZE(s_SampleDefaultModes);
    }
    else
    {
        for (DWORD ModeIndex = 0; ModeIndex < ARRAYSIZE(s_SampleDefaultModes); ModeIndex++)
        {
            pInArgs->pDefaultMonitorModes[ModeIndex] = CreateIddCxMonitorMode(
                s_SampleDefaultModes[ModeIndex].Width,
                s_SampleDefaultModes[ModeIndex].Height,
                s_SampleDefaultModes[ModeIndex].VSync,
                IDDCX_MONITOR_MODE_ORIGIN_DRIVER
            );
        }

        pOutArgs->DefaultMonitorModeBufferOutputCount = ARRAYSIZE(s_SampleDefaultModes);
        pOutArgs->PreferredMonitorModeIdx = 0;
    }

    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS
VirtualDisplayMonitorQueryTargetModes(_In_ IDDCX_MONITOR MonitorObject,
    _In_ const IDARG_IN_QUERYTARGETMODES* pInArgs, _Out_ IDARG_OUT_QUERYTARGETMODES* pOutArgs)
{
    UNREFERENCED_PARAMETER(MonitorObject);
    UNREFERENCED_PARAMETER(pInArgs);
    UNREFERENCED_PARAMETER(pOutArgs);

    std::vector<IDDCX_TARGET_MODE> TargetModes;

    // Create a set of modes supported for frame processing and scan-out. These are typically not based on the
    // monitor's descriptor and instead are based on the static processing capability of the device. The OS will
    // report the available set of modes for a given output as the intersection of monitor modes with target modes.

    TargetModes.push_back(CreateIddCxTargetMode(3840, 2160, 60));
    TargetModes.push_back(CreateIddCxTargetMode(2560, 1440, 144));
    TargetModes.push_back(CreateIddCxTargetMode(2560, 1440, 90));
    TargetModes.push_back(CreateIddCxTargetMode(2560, 1440, 60));
    TargetModes.push_back(CreateIddCxTargetMode(1920, 1080, 144));
    TargetModes.push_back(CreateIddCxTargetMode(1920, 1080, 90));
    TargetModes.push_back(CreateIddCxTargetMode(1920, 1080, 60));
    TargetModes.push_back(CreateIddCxTargetMode(1600, 900, 60));
    TargetModes.push_back(CreateIddCxTargetMode(1024, 768, 75));
    TargetModes.push_back(CreateIddCxTargetMode(1024, 768, 60));

    pOutArgs->TargetModeBufferOutputCount = (UINT)TargetModes.size();

    if (pInArgs->TargetModeBufferInputCount >= TargetModes.size())
    {
        std::copy(TargetModes.begin(), TargetModes.end(), pInArgs->pTargetModes);
    }

    return STATUS_SUCCESS;
}
_Use_decl_annotations_
NTSTATUS
VirtualDisplayMonitorAssignSwapChain( _In_ IDDCX_MONITOR MonitorObject, _In_  const IDARG_IN_SETSWAPCHAIN* pInArgs)
{
    UNREFERENCED_PARAMETER(MonitorObject);
    UNREFERENCED_PARAMETER(pInArgs);
    auto* pMonitorContextWrapper = WdfObjectGet_IndirectMonitorContextWrapper(MonitorObject);
    if (pMonitorContextWrapper == nullptr)
    {
        return STATUS_UNSUCCESSFUL;
    }
    pMonitorContextWrapper->pContext->AssignSwapChain(pInArgs->hSwapChain, pInArgs->RenderAdapterLuid, pInArgs->hNextSurfaceAvailable);
    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS
VirtualDisplayMonitorUnassignSwapChain(_In_ IDDCX_MONITOR MonitorObject)
{
    UNREFERENCED_PARAMETER(MonitorObject);

    auto* pMonitorContextWrapper = WdfObjectGet_IndirectMonitorContextWrapper(MonitorObject);
    if (pMonitorContextWrapper == nullptr)
    {
        return STATUS_UNSUCCESSFUL;
    }
    pMonitorContextWrapper->pContext->UnassignSwapChain();
    return STATUS_SUCCESS;
}



