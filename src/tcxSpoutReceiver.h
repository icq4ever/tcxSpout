#pragma once

#include <TrussC.h>
#include <memory>
#include <string>
#include <vector>

namespace tcx::spout {

// =============================================================================
// Receiver - receive a Spout sender's texture from another app on the same PC.
//
//   tcx::spout::Receiver receiver;
//
//   setup():   receiver.setup();              // the active sender, or a name
//   update():  receiver.update();             // true when a new frame arrived
//   draw():    if (receiver.isConnected()) receiver.draw(0, 0, getWidth(), getHeight());
//
// The frame is copied GPU to GPU (one copy) into an Fbo the receiver owns, sized and
// formatted like the sender (BGRA8 / RGBA8 / RGBA16 / RGBA16F / RGBA32F). It
// follows the sender when it changes size or restarts; with no name it follows
// whichever sender is active (the one last started, or picked in SpoutSettings).
//
// Windows only. Elsewhere setup() returns false and the other calls do nothing.
// =============================================================================
class Receiver {
public:
    Receiver();
    ~Receiver();
    Receiver(const Receiver&) = delete;
    Receiver& operator=(const Receiver&) = delete;

    // Connect to `senderName`, or to the active sender when empty
    bool setup(const std::string& senderName = "");
    void close();

    // Switch to another sender ("" = the active one)
    void setSenderName(const std::string& senderName);

    // Call once per frame (update()). Returns true when a new frame was received.
    bool update();

    bool isSetup() const;
    bool isConnected() const;           // a sender is present and was received
    std::string getSenderName() const;  // the connected sender's name
    int getWidth() const;
    int getHeight() const;
    double getSenderFps() const;        // as reported by the sender
    long getFrameCount() const;         // new frames received

    // The received image (valid while isConnected())
    const tc::Fbo& getFbo() const;
    void draw(float x, float y, float w, float h) const;

    // Names of the senders currently running on this PC
    std::vector<std::string> listSenders() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace tcx::spout
