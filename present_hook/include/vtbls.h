#ifndef PRESENT_HOOK_VTBLS_H
#define PRESENT_HOOK_VTBLS_H

namespace PresentHook {

enum class IDXGISwapChain1VTBL 
{
    QueryInterface,
    AddRef,
    Release,
    SetPrivateData,
    SetPrivateDataInterface,
    GetPrivateData,
    GetParent,
    GetDevice,
    Present,
    GetBuffer,
    SetFullscreenState,
    GetFullscreenState,
    GetDesc,
    ResizeBuffers,
    ResizeTarget,
    GetContainingOutput,
    GetFrameStatistics,
    GetLastPresentCount,
    GetDesc1,
    GetFullscreenDesc,
    GetHwnd,
    GetCoreWindow,
    Present1,
    IsTemporaryMonoSupported,
    GetRestrictToOutput,
    SetBackgroundColor,
    GetBackgroundColor,
    SetRotation,
    GetRotation,
    COUNT
};

enum class ID3D11DeviceVTBL
{
    QueryInterface,
    AddRef,
    Release,
    CreateBuffer,
    CreateTexture1D,
    CreateTexture2D,
    CreateTexture3D,
    CreateShaderResourceView,
    CreateUnorderedAccessView,
    CreateRenderTargetView,
    CreateDepthStencilView,
    CreateInputLayout,
    CreateVertexShader,
    CreateGeometryShader,
    CreateGeometryShaderWithStreamOutput,
    CreatePixelShader,
    CreateHullShader,
    CreateDomainShader,
    CreateComputeShader,
    CreateClassLinkage,
    CreateBlendState,
    CreateDepthStencilState,
    CreateRasterizerState,
    CreateSamplerState,
    CreateQuery,
    CreatePredicate,
    CreateCounter,
    CreateDeferredContext,
    OpenSharedResource,
    CheckFormatSupport,
    CheckMultisampleQualityLevels,
    CheckCounterInfo,
    CheckCounter,
    CheckFeatureSupport,
    GetPrivateData,
    SetPrivateData,
    SetPrivateDataInterface,
    GetFeatureLevel,
    GetCreationFlags,
    GetDeviceRemovedReason,
    GetImmediateContext,
    SetExceptionMode,
    GetExceptionMode,
    COUNT
};

enum class ID3D11DeviceContextVTBL
{
    QueryInterface,
    AddRef,
    Release,
    GetDevice,
    GetPrivateData,
    SetPrivateData,
    SetPrivateDataInterface,
    VSSetConstantBuffers,
    PSSetShaderResources,
    PSSetShader,
    PSSetSamplers,
    VSSetShader,
    DrawIndexed,
    Draw,
    Map,
    Unmap,
    PSSetConstantBuffers,
    IASetInputLayout,
    IASetVertexBuffers,
    IASetIndexBuffer,
    DrawIndexedInstanced,
    DrawInstanced,
    GSSetConstantBuffers,
    GSSetShader,
    IASetPrimitiveTopology,
    VSSetShaderResources,
    VSSetSamplers,
    Begin,
    End,
    GetData,
    SetPredication,
    GSSetShaderResources,
    GSSetSamplers,
    OMSetRenderTargets,
    OMSetRenderTargetsAndUnorderedAccessViews,
    OMSetBlendState,
    OMSetDepthStencilState,
    SOSetTargets,
    DrawAuto,
    DrawIndexedInstancedIndirect,
    DrawInstancedIndirect,
    Dispatch,
    DispatchIndirect,
    RSSetState,
    RSSetViewports,
    RSSetScissorRects,
    CopySubresourceRegion,
    CopyResource,
    UpdateSubresource,
    CopyStructureCount,
    ClearRenderTargetView,
    ClearUnorderedAccessViewUint,
    ClearUnorderedAccessViewFloat,
    ClearDepthStencilView,
    GenerateMips,
    SetResourceMinLOD,
    GetResourceMinLOD,
    ResolveSubresource,
    ExecuteCommandList,
    HSSetShaderResources,
    HSSetShader,
    HSSetSamplers,
    HSSetConstantBuffers,
    DSSetShaderResources,
    DSSetShader,
    DSSetSamplers,
    DSSetConstantBuffers,
    CSSetShaderResources,
    CSSetUnorderedAccessViews,
    CSSetShader,
    CSSetSamplers,
    CSSetConstantBuffers,
    VSGetConstantBuffers,
    PSGetShaderResources,
    PSGetShader,
    PSGetSamplers,
    VSGetShader,
    PSGetConstantBuffers,
    IAGetInputLayout,
    IAGetVertexBuffers,
    IAGetIndexBuffer,
    GSGetConstantBuffers,
    GSGetShader,
    IAGetPrimitiveTopology,
    VSGetShaderResources,
    VSGetSamplers,
    GetPredication,
    GSGetShaderResources,
    GSGetSamplers,
    OMGetRenderTargets,
    OMGetRenderTargetsAndUnorderedAccessViews,
    OMGetBlendState,
    OMGetDepthStencilState,
    SOGetTargets,
    RSGetState,
    RSGetViewports,
    RSGetScissorRects,
    HSGetShaderResources,
    HSGetShader,
    HSGetSamplers,
    HSGetConstantBuffers,
    DSGetShaderResources,
    DSGetShader,
    DSGetSamplers,
    DSGetConstantBuffers,
    CSGetShaderResources,
    CSGetUnorderedAccessViews,
    CSGetShader,
    CSGetSamplers,
    CSGetConstantBuffers,
    ClearState,
    Flush,
    GetType,
    GetContextFlags,
    FinishCommandList,
    COUNT
};

}

#endif