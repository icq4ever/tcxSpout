# tcxSpout

[Spout2](https://spout.zeal.co/) addon for TrussC: share textures with other
apps on the same Windows PC (Resolume, TouchDesigner, OBS, media servers, ...).
The texture stays on the GPU (Direct3D 11 shared texture): no readback, no
compression.

Windows only. On other platforms the addon compiles and `setup()` returns false.

## Usage

`addons.make`:

```
tcxSpout
```

### Sending

```cpp
#include <tcxSpout.h>

tcx::spout::Sender sender;
Fbo fbo;

void tcApp::setup() {
    fbo.allocate(1920, 1080, 1, TextureFormat::BGRA8);
    sender.setup("My App");
}

void tcApp::draw() {
    fbo.begin(0, 0, 0, 1);
    // ... draw ...
    fbo.end();
    sender.send(fbo);   // copied after the frame is submitted

    fbo.draw(0, 0, getWidth(), getHeight());   // optional preview
}
```

- Call `send()` once per frame, after `fbo.end()`.
- `BGRA8` is Spout's native format and is accepted by every receiver. `RGBA8`
  works with current receivers.
- Any size up to the GPU's texture limit (16384 on Direct3D 11 hardware).

Check the output with the Spout demo receiver (`SpoutReceiver.exe` in the
[Spout release](https://github.com/leadedge/Spout2/releases)) or any Spout
capable app.

### Receiving

```cpp
#include <tcxSpout.h>

tcx::spout::Receiver receiver;

void tcApp::setup() {
    receiver.setup();            // the active sender, or setup("Sender Name")
}

void tcApp::update() {
    receiver.update();           // true when a new frame arrived
}

void tcApp::draw() {
    if (receiver.isConnected()) {
        receiver.draw(0, 0, getWidth(), getHeight());   // or receiver.getFbo()
    }
}
```

- Call `update()` once per frame. The frame is copied GPU to GPU into an `Fbo`
  the receiver owns, with the sender's size and format (BGRA8, RGBA8, RGBA16,
  RGBA16F, RGBA32F).
- With no name it follows the active sender (the one last started, or picked in
  SpoutSettings), and keeps following it when it resizes or restarts.
- `listSenders()` returns the senders running on this PC; `setSenderName()`
  switches to another one (`""` = the active sender).
- `getSenderName()`, `getWidth()`, `getHeight()`, `getSenderFps()` and
  `getFrameCount()` describe the current connection.

Test it with the Spout demo sender (`SpoutSender.exe` in the Spout release) or
`example-sender`.

## Examples

- `example-sender`: an animated pattern sent as "tcxSpout example".
- `example-receiver`: shows a sender running on this PC (the active one by
  default). Left / right arrows step through the senders, `0` goes back to the
  active one.

## License

MIT. `libs/Spout2` is part of the Spout2 SDK (Simplified BSD, Lynn Jarvis); see
`libs/Spout2/LICENSE.txt` and `libs/Spout2/VERSION.txt`. All licenses are
listed in [LICENSES.md](LICENSES.md).
