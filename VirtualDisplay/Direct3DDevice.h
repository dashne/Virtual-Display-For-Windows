#pragma once
class Direct3DDevice
{
public:
    Direct3DDevice(LUID AdapterLuid);
    Direct3DDevice();
    HRESULT Init();

    LUID AdapterLuid;
    Microsoft::WRL::ComPtr<IDXGIFactory5> DxgiFactory;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> Adapter;
    Microsoft::WRL::ComPtr<ID3D11Device> Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
};

