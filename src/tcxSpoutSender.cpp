#include "tcxSpoutSender.h"

#include "tcxSpoutInternal.h"   // Spout + windows.h stay out of the public header

using namespace tc;

namespace tcx::spout {

#ifdef _WIN32

struct Sender::Impl {
    spoutDX dx;
    std::string name;
    bool open = false;

    sg_image pending = {};      // Fbo texture queued by send(), copied in afterFrame
    bool hasPending = false;

    int width = 0;
    int height = 0;
    long frames = 0;
    bool warnedNoTexture = false;

    EventListener afterFrame;

    // Runs after the frame was submitted: the Fbo content is on the GPU queue,
    // and the copy below is queued behind it on the same immediate context.
    void flush() {
        if (!open || !hasPending) return;
        hasPending = false;

        ID3D11Texture2D* tex = internal::texture(pending);
        if (!tex) {
            if (!warnedNoTexture) {
                logWarning("tcxSpout") << "send(): the Fbo has no Direct3D 11 texture";
                warnedNoTexture = true;
            }
            return;
        }
        if (dx.SendTexture(tex)) {
            D3D11_TEXTURE2D_DESC desc;
            tex->GetDesc(&desc);
            width = (int)desc.Width;
            height = (int)desc.Height;
            ++frames;
        }
    }
};

Sender::Sender() : impl_(std::make_unique<Impl>()) {}

Sender::~Sender() { close(); }

bool Sender::setup(const std::string& name) {
    close();

    ID3D11Device* device = internal::device();
    if (!device) {
        logError("tcxSpout") << "no Direct3D 11 device (setup() must run after the window is created)";
        return false;
    }
    if (!impl_->dx.OpenDirectX11(device)) {
        logError("tcxSpout") << "could not open Spout on the Direct3D 11 device";
        return false;
    }
    impl_->dx.SetSenderName(name.c_str());
    impl_->name = name;
    impl_->open = true;
    impl_->frames = 0;
    impl_->width = impl_->height = 0;

    Impl* impl = impl_.get();
    impl_->afterFrame = events().afterFrame.listen([impl] { impl->flush(); });

    logNotice("tcxSpout") << "sender \"" << name << "\" ready";
    return true;
}

void Sender::close() {
    if (!impl_ || !impl_->open) return;
    impl_->afterFrame = EventListener{};
    impl_->hasPending = false;
    impl_->dx.ReleaseSender();
    impl_->dx.CloseDirectX11();
    impl_->open = false;
}

void Sender::send(const Fbo& fbo) {
    if (!impl_->open || !fbo.isAllocated()) return;
    impl_->pending = fbo.getColorImage();
    impl_->hasPending = true;
}

bool Sender::isSetup() const { return impl_->open; }
const std::string& Sender::getName() const { return impl_->name; }
int Sender::getWidth() const { return impl_->width; }
int Sender::getHeight() const { return impl_->height; }
long Sender::getFrameCount() const { return impl_->frames; }

#else // not Windows: stub

struct Sender::Impl {
    std::string name;
};

Sender::Sender() : impl_(std::make_unique<Impl>()) {}
Sender::~Sender() = default;

bool Sender::setup(const std::string& name) {
    impl_->name = name;
    logWarning("tcxSpout") << "Spout is Windows only; sender \"" << name << "\" is not started";
    return false;
}
void Sender::close() {}
void Sender::send(const Fbo&) {}
bool Sender::isSetup() const { return false; }
const std::string& Sender::getName() const { return impl_->name; }
int Sender::getWidth() const { return 0; }
int Sender::getHeight() const { return 0; }
long Sender::getFrameCount() const { return 0; }

#endif

} // namespace tcx::spout
