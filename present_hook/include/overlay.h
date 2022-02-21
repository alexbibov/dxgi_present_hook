#ifndef PRESENT_HOOK_OVERLAY_H
#define PRESENT_HOOK_OVERLAY_H

#include <atomic>
#include <cstdint>

#include <windows.h>
#include <CommCtrl.h>
#include <dxgi1_6.h>
#include <d3d11.h>
#include <wrl/client.h>

namespace PresentHook {

class Overlay final{
public:
    explicit Overlay(IDXGISwapChain* p_swap_chain);
    ~Overlay();
    void updateOverlay();

    static LRESULT controlsHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

private:
    static std::atomic_uint m_subclass_id;

    Microsoft::WRL::ComPtr<ID3D11Device> m_hijacked_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_hijacked_device_context;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    HWND m_client_window;
    
    uint64_t m_performance_frequency;
    uint64_t m_performance_counter;

    // imgui d3d device objects
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_index_buffer;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_ia_input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vs_constant_buffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_font_sampler;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_font_texture_srv;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer_state;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blend_state;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depth_stencil_state;
    size_t m_vertex_buffer_size;
    size_t m_index_buffer_size;
};

}

#endif