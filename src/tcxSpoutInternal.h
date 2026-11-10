#pragma once

// Internal to tcxSpout (.cpp files only): Spout SDK + Direct3D 11 access.

#ifdef _WIN32

#include <TrussC.h>

// Spout's SpoutUtils.h defines WIN32_LEAN_AND_MEAN, which TrussC already sets
#pragma warning(push)
#pragma warning(disable : 4005)
#include "SpoutDX.h"
#pragma warning(pop)

namespace tcx::spout::internal {

// TrussC's (sokol's) Direct3D 11 device, or nullptr before the window exists
inline ID3D11Device* device() {
    return static_cast<ID3D11Device*>(const_cast<void*>(sg_d3d11_device()));
}

// The Direct3D 11 texture behind a sokol image
inline ID3D11Texture2D* texture(sg_image img) {
    return static_cast<ID3D11Texture2D*>(const_cast<void*>(sg_d3d11_query_image_info(img).tex2d));
}

// Spout sender format -> Fbo format; false if TrussC has no matching format
inline bool toTextureFormat(DXGI_FORMAT f, tc::TextureFormat& out) {
    switch (f) {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            out = tc::TextureFormat::BGRA8; return true;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            out = tc::TextureFormat::RGBA8; return true;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            out = tc::TextureFormat::RGBA16F; return true;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            out = tc::TextureFormat::RGBA16; return true;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            out = tc::TextureFormat::RGBA32F; return true;
        default:
            return false;
    }
}

} // namespace tcx::spout::internal

#endif
