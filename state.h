#pragma once

enum states {
    Idle,
    Recording,
    Playing,
};

class State {
public:
    State();
    states state;
    bool transition(states newState);
    double timeRatio = 1.0;
};