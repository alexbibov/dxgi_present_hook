#include <algorithm>
#include <d3dcompiler.h>

#include "imgui.h"
#include "overlay.h"

namespace PresentHook {

namespace {

char const* g_vertex_shader_source =
"struct MVPData\
{\
    float4x4 projectionMatrix;\
};\
ConstantBuffer<MVPData> vsData : register(b0);\
\
struct VS_INPUT \
{\
    float2 position : POSITION;\
    float4 color : COLOR0;\
    float2 uv : TEXCOORD0;\
};\
\
struct PS_INPUT \
{\
    float2 position : SV_POSITION;\
    float4 color : COLOR0;\
    float2 uv : TEXCOORD0;\
};\
\
PS_INPUT main(VS_INPUT input)\
{\
    PS_INPUT output;\
    output.position = mul(vsData.projectionMatrix, float4(input.position, 0.f, 1.f));\
    output.color = input.color;\
    output.uv = input.uv\
    return output;\
}\
";

char const* g_pixel_shader_source = 
"struct PS_INPUT \
{\
    float2 position : SV_POSITION;\
    float4 color : COLOR0;\
    float2 uv : TEXCOORD0;\
};\
SamplerState sampler0 : register(s0);\
Texture2D<float4> texture0 : register(t0);\
float4 main(PS_INPUT input) : SV_Target\
{\
    float4 output_color = input.color * texture0.Sample(sample0, input.uv);\
    return output_color;\
}\
";

}

std::atomic_uint Overlay::m_subclass_id{ 1u };

LRESULT Overlay::controlsHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

Overlay::Overlay(IDXGISwapChain* p_swap_chain)
    : m_vertex_buffer_size{ 5000 }
    , m_index_buffer_size{ 10000 }
{
    {
        // Hijack D3D device and the context
        p_swap_chain->GetDevice(IID_PPV_ARGS(&m_hijacked_device));
        m_hijacked_device->GetImmediateContext(&m_hijacked_device_context);

        {
            DXGI_SWAP_CHAIN_DESC desc{};
            p_swap_chain->GetDesc(&desc);
            m_client_window = desc.OutputWindow;
            SetWindowSubclass(m_client_window, &Overlay::controlsHookProc, m_subclass_id.load(std::memory_order::memory_order_acquire), reinterpret_cast<DWORD_PTR>(this));
            ++m_subclass_id;
        }

        m_hijacked_device_context->OMGetRenderTargets(1, &m_rtv, NULL);
        if (!m_rtv) {
            // If we cannot for some reason retrieve the active render target view, we proceed by creating one of our own

            ID3D11Texture2D* p_backbuffer_texture{ nullptr };
            p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&p_backbuffer_texture));

            if (p_backbuffer_texture) {
                m_hijacked_device->CreateRenderTargetView(p_backbuffer_texture, NULL, &m_rtv);
            }
        }
    }

    {
        // Setup imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendPlatformUserData = this;
        io.BackendPlatformName = "win32_subclass_controls_hook";
        io.BackendRendererUserData = this;
        io.BackendRendererName = "d3d11";
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_RendererHasVtxOffset;

        ImGui::GetMainViewport()->PlatformHandleRaw = reinterpret_cast<void*>(m_client_window);

        {
            // Fetch OS counters

            LARGE_INTEGER aux;
            QueryPerformanceFrequency(&aux);
            m_performance_frequency = static_cast<uint64_t>(aux.QuadPart);

            QueryPerformanceCounter(&aux);
            m_performance_counter = static_cast<uint64_t>(aux.QuadPart);
        }

        {
            // Create font textures and samplers
            unsigned char* data{ nullptr };
            int width{}, height{};
            io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

            D3D11_TEXTURE2D_DESC desc{
                .Width = static_cast<UINT>(width),
                .Height = static_cast<UINT>(height),
                .MipLevels = 1,
                .ArraySize = 1,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {.Count = 1},
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_SHADER_RESOURCE,
                .CPUAccessFlags = 0
            };

            ID3D11Texture2D* p_texture{ nullptr };
            D3D11_SUBRESOURCE_DATA init_data{
                .pSysMem = data,
                .SysMemPitch = desc.Width * 4,
                .SysMemSlicePitch = 0
            };
            m_hijacked_device->CreateTexture2D(&desc, &init_data, &p_texture);


            // Create SRV
            D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .Texture2D = {.MostDetailedMip = 0, .MipLevels = desc.MipLevels}
            };
            m_hijacked_device->CreateShaderResourceView(p_texture, &srv_desc, &m_font_texture_srv);
            p_texture->Release();

            io.Fonts->SetTexID(m_font_texture_srv.Get());

            D3D11_SAMPLER_DESC sampler_desc{
                .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
                .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
                .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
                .MipLODBias = 0,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
                .BorderColor = {0.f, 0.f, 0.f, 0.f},
                .MinLOD = 0,
                .MaxLOD = 0
            };
            m_hijacked_device->CreateSamplerState(&sampler_desc, &m_font_sampler);
        }

        {
            // Compile shaders

        }

        {
            // Create input layout, blender, rasterizer, and depth-stencil states
        }

        {
            // Setup constant buffers
        }
    }


}

void Overlay::updateOverlay()
{
    ImDrawData* p_draw_data = ImGui::GetDrawData();
    if (p_draw_data->DisplaySize.x <= 0.f || p_draw_data->DisplaySize.y <= 0.f) return;

    // Create/grow vertex and index buffers as needed
    if (!m_vertex_buffer || m_vertex_buffer_size < p_draw_data->TotalVtxCount) {
        if (m_vertex_buffer) {
            m_vertex_buffer = nullptr;
            m_vertex_buffer_size = (std::max)(p_draw_data->TotalVtxCount, 2 * static_cast<int>(m_vertex_buffer_size));
        }
        else {
            m_vertex_buffer_size = (std::max)(p_draw_data->TotalVtxCount, static_cast<int>(m_vertex_buffer_size));
        }

        D3D11_BUFFER_DESC vb_desc{
            .ByteWidth = static_cast<UINT>(m_vertex_buffer_size * sizeof(ImDrawVert)),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
        };
        m_hijacked_device->CreateBuffer(&vb_desc, NULL, &m_vertex_buffer);
    }
    if (!m_index_buffer_size || m_index_buffer_size < p_draw_data->TotalIdxCount) {
        if (m_index_buffer) {
            m_index_buffer = nullptr;
            m_index_buffer_size = (std::max)(p_draw_data->TotalIdxCount, 2 * static_cast<int>(m_index_buffer_size));
        }
        else {
            m_index_buffer_size = (std::max)(p_draw_data->TotalIdxCount, static_cast<int>(m_index_buffer_size));
        }

        D3D11_BUFFER_DESC ib_desc{
            .ByteWidth = static_cast<UINT>(m_index_buffer_size * sizeof(ImDrawIdx)),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
        };
        m_hijacked_device->CreateBuffer(&ib_desc, NULL, &m_index_buffer);
    }

    D3D11_MAPPED_SUBRESOURCE vertex_data_subresource{}, index_data_subresource{};
    m_hijacked_device_context->Map(m_vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &vertex_data_subresource);
    m_hijacked_device_context->Map(m_index_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &index_data_subresource);
    
    ImDrawVert* vertex_data_dst = reinterpret_cast<ImDrawVert*>(vertex_data_subresource.pData);
    ImDrawIdx* index_data_dst = reinterpret_cast<ImDrawIdx*>(index_data_subresource.pData);
    for (int i = 0; i < p_draw_data->CmdListsCount; ++i) {
        ImDrawList const* cmd_list = p_draw_data->CmdLists[i];
        memcpy(vertex_data_subresource.pData, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(index_data_subresource.pData, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vertex_data_dst += cmd_list->VtxBuffer.Size;
        index_data_dst += cmd_list->IdxBuffer.Size;
    }
    m_hijacked_device_context->Unmap(m_vertex_buffer.Get(), 0);
    m_hijacked_device_context->Unmap(m_index_buffer.Get(), 0);

    {

    }
}

Overlay::~Overlay()
{
    ImGui::DestroyContext();
}

}