#pragma once

//class IndirectDeviceContext;

/// <summary>
        /// Manages the creation and lifetime of a Direct3D render device.
        /// </summary>
struct IndirectSampleMonitor
{
    static constexpr size_t szEdidBlock = 128;
    static constexpr size_t szModeList = 3;

    const BYTE pEdidBlock[szEdidBlock];
    const struct SampleMonitorMode {
        DWORD Width;
        DWORD Height;
        DWORD VSync;
    } pModeList[szModeList];
    const DWORD ulPreferredModeIdx;
};



struct IndirectDeviceContextWrapper
{
    IndirectDeviceContext* pContext;

    void Cleanup()
    {
        delete pContext;
        pContext = nullptr;
    }
};


struct IndirectMonitorContextWrapper
{
    IndirectMonitorContext* pContext;

    void Cleanup()
    {
        delete pContext;
        pContext = nullptr;
    }
};



//--------------------- const and initialize-------------------

WDF_DECLARE_CONTEXT_TYPE(IndirectDeviceContextWrapper);
WDF_DECLARE_CONTEXT_TYPE(IndirectMonitorContextWrapper);

#pragma region SampleMonitor
extern  constexpr DWORD IDD_SAMPLE_MONITOR_COUNT = 3;
// Default modes reported for edid-less monitors. The first mode is set as preferred
extern  const IndirectSampleMonitor::SampleMonitorMode s_SampleDefaultModes[];

// FOR SAMPLE PURPOSES ONLY, Static info about monitors that will be reported to OS
extern const IndirectSampleMonitor s_SampleMonitors[];
extern const size_t s_SampleMonitorsCount;
#pragma  endregion
