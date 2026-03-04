#pragma once

class Direct3DDevice;

/// <summary>
        /// Manages a thread that consumes buffers from an indirect display swap-chain object.
        /// </summary>
class SwapChainProcessor
{
public:
    SwapChainProcessor(IDDCX_SWAPCHAIN hSwapChain, std::shared_ptr<Direct3DDevice> Device, HANDLE NewFrameEvent);
    ~SwapChainProcessor();

private:
    static DWORD CALLBACK RunThread(LPVOID Argument);

    void Run();
    void RunCore();

    IDDCX_SWAPCHAIN m_hSwapChain;
    std::shared_ptr<Direct3DDevice> m_Device;
    HANDLE m_hAvailableBufferEvent;
    Microsoft::WRL::Wrappers::HandleT<Microsoft::WRL::Wrappers::HandleTraits::HANDLENullTraits> m_hThread; //Microsoft::WRL::Wrappers::
    Microsoft::WRL::Wrappers::Event m_hTerminateEvent;
};

