#pragma once

#include <TrussC.h>
#include <tcxSpout.h>
using namespace std;
using namespace tc;

// Shows a Spout sender running on this PC (the active one by default).
// Left / right arrows step through the senders; 0 goes back to the active one.
class tcApp : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;

private:
    tcx::spout::Receiver receiver_;
    int pick_ = -1;     // -1: active sender, else index into listSenders()
};
