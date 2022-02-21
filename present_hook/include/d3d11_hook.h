#ifndef PRESENT_HOOK_D3D11_HOOK_H
#define PRESENT_HOOK_D3D11_HOOK_H

#include <Windows.h>
#include <dxgi1_6.h>
#include <functional>

namespace PresentHook {

class D3D11Hook {
public:
    using PresentCallback = std::function<HRESULT(IDXGISwapChain*, UINT, UINT)>;
    using Present1Callback = std::function<HRESULT(IDXGISwapChain1*, UINT, UINT, DXGI_PRESENT_PARAMETERS*)>;
    
public:
    D3D11Hook(PresentCallback const& present_callback, Present1Callback const& present1_callback);
    static bool hookPresent();

private:
    static HRESULT PresentGeneralHook(IDXGISwapChain*, UINT, UINT);
    static HRESULT Present1GeneralHook(IDXGISwapChain1*, UINT, UINT, DXGI_PRESENT_PARAMETERS*);
    static HRESULT finalize(IDXGISwapChain* p_this, UINT sync_interval, UINT present_flags) { return m_present_original(p_this, sync_interval, present_flags); }
    static HRESULT finalize1(IDXGISwapChain1* p_this, UINT sync_interval, UINT present_flags, DXGI_PRESENT_PARAMETERS* p_present_parameters) { return m_present1_original(p_this, sync_interval, present_flags, p_present_parameters); }


    HRESULT invokePresentHook(IDXGISwapChain* p_this, UINT sync_interval, UINT present_flags) const { return m_present_callback(p_this, sync_interval, present_flags); }
    HRESULT invokePresent1Hook(IDXGISwapChain1* p_this, UINT sync_interval, UINT present_flags, DXGI_PRESENT_PARAMETERS* p_present_parameters) const { return m_present1_callback(p_this, sync_interval, present_flags, p_present_parameters); }

private:
    static PresentCallback m_present_original;
    static Present1Callback m_present1_original;
    PresentCallback m_present_callback;
    Present1Callback m_present1_callback;
};

}

#endif