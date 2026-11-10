#pragma once

#include <TrussC.h>
#include <memory>
#include <string>

namespace tcx::spout {

// =============================================================================
// Sender - publish an Fbo to Spout receivers (Resolume, TouchDesigner, OBS,
// media servers, ...) on the same PC.
//
//   tcx::spout::Sender sender;
//   Fbo fbo;
//
//   setup():  fbo.allocate(1920, 1080, 1, TextureFormat::BGRA8);
//             sender.setup("My App");
//   draw():   fbo.begin(0, 0, 0, 1); ... fbo.end();
//             sender.send(fbo);
//
// The texture stays on the GPU: Spout shares it through a Direct3D 11 shared
// texture, so there is no readback and no compression.
//
// TrussC records drawing and submits it at the end of the frame, so send()
// only remembers the Fbo; the copy into the shared texture happens after the
// frame has been submitted (events().afterFrame of the window that was current
// at setup()). The Fbo must stay allocated until then.
//
// BGRA8 is Spout's native format and works with every receiver; RGBA8 also
// works with current (2.007) receivers. MSAA Fbos are fine: the resolved
// texture is sent.
//
// Windows only. Elsewhere setup() logs a warning and returns false, and the
// other calls do nothing, so an app can keep the calls unconditionally.
// =============================================================================
class Sender {
public:
    Sender();
    ~Sender();
    Sender(const Sender&) = delete;
    Sender& operator=(const Sender&) = delete;

    // Start a sender with this name (shown in receivers' sender lists).
    // Returns false if Spout is not available (no D3D11 device, other OS).
    bool setup(const std::string& name);

    // Remove the sender; receivers see it disappear.
    void close();

    // Queue this frame's content of `fbo`. Call once per frame after fbo.end().
    void send(const tc::Fbo& fbo);

    bool isSetup() const;
    const std::string& getName() const;

    // Size and number of frames actually sent (0 before the first send)
    int getWidth() const;
    int getHeight() const;
    long getFrameCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace tcx::spout
