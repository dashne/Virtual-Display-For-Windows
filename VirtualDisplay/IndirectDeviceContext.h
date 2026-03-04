#pragma once

class IndirectDeviceContext
{
public:
	IndirectDeviceContext(_In_ WDFDEVICE device);
	~IndirectDeviceContext();
	void InitAdaptr();
	void FinishInit(UINT index);


	WDFDEVICE WdfDevice;
	IDDCX_ADAPTER Adapter;
	
};



