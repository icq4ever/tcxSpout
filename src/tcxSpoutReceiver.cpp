#include "tcxSpoutReceiver.h"
#include "tcxSpoutInternal.h"   // Spout + windows.h stay out of the public header

using namespace tc;

namespace tcx::spout {

#ifdef _WIN32

struct Receiver::Impl {
    mutable spoutDX dx;
    bool open = false;
    bool connected = false;
    std::string wanted;         // "" = active sender
    Fbo fbo;
    long frames = 0;
    DXGI_FORMAT warnedFormat = DXGI_FORMAT_UNKNOWN;

    // Size the Fbo like the sender; false if the format has no TrussC match
    bool matchSender() {
        int w = (int)dx.GetSenderWidth();
        int h = (int)dx.GetSenderHeight();
        DXGI_FORMAT f = dx.GetSenderFormat();
        TextureFormat tf;
        if (!internal::toTextureFormat(f, tf)) {
            if (warnedFormat != f) {
                logWarning("tcxSpout") << "sender \"" << dx.GetSenderName() << "\": texture format "
                                       << (int)f << " is not supported";
                warnedFormat = f;
            }
            return false;
        }
        if (!fbo.isAllocated() || fbo.getWidth() != w || fbo.getHeight() != h || fbo.getTextureFormat() != tf) {
            fbo.allocate(w, h, 1, tf);
            logNotice("tcxSpout") << "receiving \"" << dx.GetSenderName() << "\" " << w << "x" << h;
        }
        return fbo.isAllocated();
    }
};

Receiver::Receiver() : impl_(std::make_unique<Impl>()) {}

Receiver::~Receiver() { close(); }

bool Receiver::setup(const std::string& senderName) {
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
    impl_->open = true;
    impl_->frames = 0;
    setSenderName(senderName);
    return true;
}

void Receiver::close() {
    if (!impl_ || !impl_->open) return;
    impl_->dx.ReleaseReceiver();
    impl_->dx.CloseDirectX11();
    impl_->open = false;
    impl_->connected = false;
}

void Receiver::setSenderName(const std::string& senderName) {
    impl_->wanted = senderName;
    if (!impl_->open) return;
    impl_->dx.ReleaseReceiver();
    impl_->dx.SetReceiverName(senderName.empty() ? nullptr : senderName.c_str());
    impl_->connected = false;
}

bool Receiver::update() {
    if (!impl_->open) return false;
    Impl& m = *impl_;

    // One GPU copy, from the sender's shared texture straight into our Fbo's
    // texture. When the sender is new or changed size, SpoutDX copies nothing
    // and raises IsUpdated(): resize the Fbo, the next call copies.
    ID3D11Texture2D* dst = m.fbo.isAllocated() ? internal::texture(m.fbo.getColorImage()) : nullptr;
    if (!m.dx.ReceiveTexture(&dst)) {
        m.connected = false;
        return false;
    }
    if (m.dx.IsUpdated()) {
        m.connected = m.matchSender();
        return false;
    }
    m.connected = m.dx.IsConnected() && m.fbo.isAllocated();
    if (!m.connected || !m.dx.IsFrameNew()) return false;
    ++m.frames;
    return true;
}

bool Receiver::isSetup() const { return impl_->open; }
bool Receiver::isConnected() const { return impl_->connected && impl_->fbo.isAllocated(); }
std::string Receiver::getSenderName() const {
    return impl_->connected ? std::string(impl_->dx.GetSenderName()) : std::string();
}
int Receiver::getWidth() const { return impl_->fbo.isAllocated() ? impl_->fbo.getWidth() : 0; }
int Receiver::getHeight() const { return impl_->fbo.isAllocated() ? impl_->fbo.getHeight() : 0; }
double Receiver::getSenderFps() const { return impl_->connected ? impl_->dx.GetSenderFps() : 0.0; }
long Receiver::getFrameCount() const { return impl_->frames; }
const Fbo& Receiver::getFbo() const { return impl_->fbo; }

void Receiver::draw(float x, float y, float w, float h) const {
    if (isConnected()) impl_->fbo.draw(x, y, w, h);
}

std::vector<std::string> Receiver::listSenders() const { return impl_->dx.GetSenderList(); }

#else // not Windows: stub

struct Receiver::Impl {
    Fbo fbo;
};

Receiver::Receiver() : impl_(std::make_unique<Impl>()) {}
Receiver::~Receiver() = default;

bool Receiver::setup(const std::string&) {
    logWarning("tcxSpout") << "Spout is Windows only; the receiver is not started";
    return false;
}
void Receiver::close() {}
void Receiver::setSenderName(const std::string&) {}
bool Receiver::update() { return false; }
bool Receiver::isSetup() const { return false; }
bool Receiver::isConnected() const { return false; }
std::string Receiver::getSenderName() const { return {}; }
int Receiver::getWidth() const { return 0; }
int Receiver::getHeight() const { return 0; }
double Receiver::getSenderFps() const { return 0.0; }
long Receiver::getFrameCount() const { return 0; }
const Fbo& Receiver::getFbo() const { return impl_->fbo; }
void Receiver::draw(float, float, float, float) const {}
std::vector<std::string> Receiver::listSenders() const { return {}; }

#endif

} // namespace tcx::spout
