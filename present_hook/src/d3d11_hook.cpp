#include <d3d11_4.h>
#include <cstdint>
#include <vector>
#include <array>
#include "d3d11_hook.h"
#include "vtbls.h"

namespace PresentHook {

namespace {

using PresentPtrType = HRESULT(*)(IDXGISwapChain*, UINT, UINT);
using Present1PtrType = HRESULT(*)(IDXGISwapChain1*, UINT, UINT, DXGI_PRESENT_PARAMETERS*);

std::vector<D3D11Hook*> g_callback_object_pool;
std::array<void*, static_cast<size_t>(IDXGISwapChain1VTBL::COUNT)> g_IDXGISwapChain1VTBLPointers;
std::array<void*, static_cast<size_t>(ID3D11DeviceVTBL::COUNT)> g_ID3D11DeviceVTBLPointers;
std::array<void*, static_cast<size_t>(ID3D11DeviceContextVTBL::COUNT)> g_ID3D11DeviceContextVTBLPointers;

void* Tramp64(void* src, void* dst, size_t source_len)
{
    uint32_t const min_length{ 14 };
    static unsigned char jmp_stub[] = {
        0xff, 0x25, 0x00, 0x00, 0x00, 0x00,    // assembly instruction: jmp qword ptr [$+6]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    // 64-bit pointer
    };

    void* p_trampoline = VirtualAlloc(NULL, source_len + sizeof(jmp_stub), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!p_trampoline) return nullptr;

    DWORD dw_old_protection_constant{};
    VirtualProtect(src, source_len, PAGE_EXECUTE_READWRITE, &dw_old_protection_constant);
    uintptr_t return_to_addr = reinterpret_cast<uintptr_t>(src) + source_len;

    // create trampoline code
    memcpy(jmp_stub + 6, &return_to_addr, 8);
    memcpy(p_trampoline, src, source_len);
    memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p_trampoline) + source_len), jmp_stub, sizeof(jmp_stub));


    // inject hook into the original code
    memcpy(jmp_stub + 6, &dst, 8);
    memcpy(src, jmp_stub, sizeof(jmp_stub));
    for (uint32_t i = min_length; i < source_len; ++i) {
        *(reinterpret_cast<unsigned char*>(src) + min_length) = 0x90;    // nop
    }

    VirtualProtect(src, source_len, dw_old_protection_constant, &dw_old_protection_constant);

    return p_trampoline;
}

}

D3D11Hook::PresentCallback D3D11Hook::m_present_original = {};
D3D11Hook::Present1Callback D3D11Hook::m_present1_original = {};

HRESULT D3D11Hook::PresentGeneralHook(IDXGISwapChain* p_this, UINT sync_interval, UINT present_flags)
{
    HRESULT res{};
    for (auto e : g_callback_object_pool) {
        if ((res = e->invokePresentHook(p_this, sync_interval, present_flags)) != S_OK) return res;
    }
    return finalize(p_this, sync_interval, present_flags);
}

HRESULT D3D11Hook::Present1GeneralHook(IDXGISwapChain1* p_this, UINT sync_interval, UINT present_flags, DXGI_PRESENT_PARAMETERS* p_present_parameters)
{
    HRESULT res{};
    for (auto e : g_callback_object_pool) {
        if ((res = e->invokePresent1Hook(p_this, sync_interval, present_flags, p_present_parameters)) != S_OK) return res;
    }
    return finalize1(p_this, sync_interval, present_flags, p_present_parameters);;
}


bool D3D11Hook::hookPresent()
{
    // invoke init code
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = "dummy_cls";

    if (!RegisterClassEx(&wc)) return false;

    // Create dummy D3D11 device and swap chain (we only need them to get virtual function tables filled, so we know the addresses of the functions to hook into)
    ID3D11Device* p_device{ nullptr };
    ID3D11DeviceContext* p_device_context{ nullptr };
    if (D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &p_device, NULL, &p_device_context) != S_OK) return false;

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc = {.Count = 1, .Quality = 0},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .Scaling = DXGI_SCALING_NONE,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
    };

    IDXGIFactory4* p_dxgi_factory4{ nullptr };
    {
        IDXGIFactory2* p_dxgi_factory2{ nullptr };
        if (CreateDXGIFactory2(NULL, IID_PPV_ARGS(&p_dxgi_factory2)) != S_OK) return false;
        if (p_dxgi_factory2->QueryInterface(IID_PPV_ARGS(&p_dxgi_factory4)) != S_OK) return false;
        p_dxgi_factory2->Release();
    }

    IDXGISwapChain1* p_swap_chain1{ nullptr };
    if (p_dxgi_factory4->CreateSwapChainForHwnd(p_device, GetForegroundWindow(), &swap_chain_desc1, NULL, NULL, &p_swap_chain1) != S_OK) return false;
    p_dxgi_factory4->Release();

    {
        // Copy virtual pointer tables of DXGI and D3D11 interfaces
        memcpy(g_IDXGISwapChain1VTBLPointers.data(), *reinterpret_cast<void***>(p_swap_chain1), g_IDXGISwapChain1VTBLPointers.size());
        memcpy(g_ID3D11DeviceVTBLPointers.data(), *reinterpret_cast<void***>(p_device), g_ID3D11DeviceVTBLPointers.size());
        memcpy(g_ID3D11DeviceContextVTBLPointers.data(), *reinterpret_cast<void***>(p_device_context), g_ID3D11DeviceContextVTBLPointers.size());
    }

    // Once VTBLs are filled we don't need the dummy interfaces anymore
    p_swap_chain1->Release();
    p_device->Release();
    p_device_context->Release();


    {
        // Hook into Present and Present1
        PresentPtrType original_present_ptr = reinterpret_cast<PresentPtrType>(Tramp64(&g_IDXGISwapChain1VTBLPointers[static_cast<size_t>(IDXGISwapChain1VTBL::Present)], &PresentGeneralHook, 19));
        Present1PtrType original_present1_ptr = reinterpret_cast<Present1PtrType>(Tramp64(&g_IDXGISwapChain1VTBLPointers[static_cast<size_t>(IDXGISwapChain1VTBL::Present1)], &Present1GeneralHook, 19));

        m_present_original = original_present_ptr;
        m_present1_original = original_present1_ptr;
    }

    return true;
}

D3D11Hook::D3D11Hook(PresentCallback const& present_callback, Present1Callback const& present1_callback)
    : m_present_callback{present_callback}
    , m_present1_callback{present1_callback}
{
    g_callback_object_pool.push_back(this);
}

}