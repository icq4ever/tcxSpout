#include "tcApp.h"

void tcApp::setup() {
    mcp::setToolLevel(mcp::ToolLevel::ReadOnly);   // screenshots when run with TRUSSC_MCP=1
    // BGRA8: Spout's native format, accepted by every receiver
    fbo_.allocate(1280, 720, 1, TextureFormat::BGRA8);
    sender_.setup("tcxSpout example");
}

void tcApp::draw() {
    float t = (float)getElapsedTime();
    float w = (float)fbo_.getWidth();
    float h = (float)fbo_.getHeight();

    fbo_.begin(0.05f, 0.05f, 0.08f, 1.0f);
    for (int i = 0; i < 12; ++i) {
        float a = TAU * i / 12.0f + t * 0.5f;
        setColor(0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a), 0.8f);
        drawCircle(w * 0.5f + cos(a) * h * 0.3f, h * 0.5f + sin(a) * h * 0.3f, h * 0.06f);
    }
    // A bar sweeping left to right shows that frames keep arriving
    setColor(1, 1, 1);
    drawRect(fmod(t * 300.0f, w), 0, 8, h);
    drawBitmapString("tcxSpout example  frame " + toString(getFrameCount()), 20, 20, 2.0f);
    fbo_.end();

    sender_.send(fbo_);

    // Preview
    clear(0);
    setColor(1, 1, 1);
    float s = std::min(getWidth() / w, getHeight() / h);
    fbo_.draw((getWidth() - w * s) * 0.5f, (getHeight() - h * s) * 0.5f, w * s, h * s);
    string status = sender_.isSetup()
        ? "Spout sender \"" + sender_.getName() + "\"  " + toString(sender_.getWidth()) + "x" +
              toString(sender_.getHeight()) + "  sent " + toString(sender_.getFrameCount())
        : "Spout not available";
    drawBitmapString(status, 10, getHeight() - 14);
}
