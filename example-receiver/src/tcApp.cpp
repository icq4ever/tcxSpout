#include "tcApp.h"

void tcApp::setup() {
    mcp::setToolLevel(mcp::ToolLevel::ReadOnly);   // screenshots when run with TRUSSC_MCP=1
    receiver_.setup();      // the active sender
}

void tcApp::update() {
    receiver_.update();
}

void tcApp::draw() {
    clear(0.1f);
    setColor(1, 1, 1);

    if (receiver_.isConnected()) {
        // Fit the received image into the window
        float s = std::min(getWidth() / receiver_.getWidth(), getHeight() / receiver_.getHeight());
        float w = receiver_.getWidth() * s, h = receiver_.getHeight() * s;
        receiver_.draw((getWidth() - w) * 0.5f, (getHeight() - h) * 0.5f, w, h);
    }

    vector<string> senders = receiver_.listSenders();
    string line = receiver_.isConnected()
        ? "\"" + receiver_.getSenderName() + "\"  " + toString(receiver_.getWidth()) + "x" +
              toString(receiver_.getHeight()) + "  " + toString(receiver_.getSenderFps(), 1) +
              " fps  received " + toString(receiver_.getFrameCount())
        : (receiver_.isSetup() ? "waiting for a sender" : "Spout not available");
    drawBitmapString(line, 10, 20);
    drawBitmapString(toString(senders.size()) + " sender(s)   [<-/->] pick   [0] active sender", 10, 36);
    for (size_t i = 0; i < senders.size(); ++i) {
        drawBitmapString((int(i) == pick_ ? "> " : "  ") + senders[i], 10, 56 + i * 16);
    }
}

void tcApp::keyPressed(int key) {
    vector<string> senders = receiver_.listSenders();
    if (key == '0') {
        pick_ = -1;
        receiver_.setSenderName("");
        return;
    }
    if (senders.empty()) return;
    int n = (int)senders.size();
    if (key == KEY_RIGHT) pick_ = (pick_ + 1) % n;
    else if (key == KEY_LEFT) pick_ = (pick_ <= 0 ? n : pick_) - 1;
    else return;
    receiver_.setSenderName(senders[pick_]);
}
