#include <memory>
#include "reflective_loader.h"
#include "d3d11_hook.h"
#include "overlay.h"

namespace PresentHook {

namespace {

std::unique_ptr<D3D11Hook> g_d3d11_init{ nullptr };

}

HRESULT Present(IDXGISwapChain* p_this, UINT sync_interval, UINT present_flags) 
{
    return S_OK;
}

HRESULT Present1(IDXGISwapChain1* p_this, UINT sync_interval, UINT present_flags, DXGI_PRESENT_PARAMETERS* p_present_parameters) 
{
    return Present(p_this, sync_interval, present_flags);
}

}

extern HINSTANCE hAppInstance;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID* lpReserved)
{
    switch(fdwReason)
    {
    case DLL_QUERY_HMODULE:
        if (lpReserved != NULL) {
            *reinterpret_cast<HMODULE*>(lpReserved) = hAppInstance;
        }
        break;

    case DLL_PROCESS_ATTACH:
        hAppInstance = hinstDLL;
        {
            bool hook_succeeded = PresentHook::D3D11Hook::hookPresent();
            if (!hook_succeeded) {
                MessageBox(NULL, "Hooking IDXGISwapChain::Present() failed", "Injection failed", MB_OK | MB_ICONERROR);
            }
            else {
                PresentHook::g_d3d11_init = std::make_unique<PresentHook::D3D11Hook>(&PresentHook::Present, &PresentHook::Present1);
            }
        }
        break;

    case DLL_THREAD_ATTACH:
        break;
        
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
}

