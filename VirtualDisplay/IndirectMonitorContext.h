#pragma once
class SwapChainProcessor;

class IndirectMonitorContext
{
public:
    IndirectMonitorContext(_In_ IDDCX_MONITOR Monitor);
    virtual ~IndirectMonitorContext();

    void AssignSwapChain(IDDCX_SWAPCHAIN SwapChain, LUID RenderAdapter, HANDLE NewFrameEvent);
    void UnassignSwapChain();

private:
    IDDCX_MONITOR m_Monitor;
    std::unique_ptr<SwapChainProcessor> m_ProcessingThread;
};

