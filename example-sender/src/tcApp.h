#pragma once

#include <TrussC.h>
#include <tcxSpout.h>
using namespace std;
using namespace tc;

// Draws an animated pattern into an Fbo and publishes it as the Spout sender
// "tcxSpout example". Open example-receiver (or any Spout receiver) to see it.
class tcApp : public App {
public:
    void setup() override;
    void draw() override;

private:
    Fbo fbo_;
    tcx::spout::Sender sender_;
};
