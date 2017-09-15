#pragma once

class Mgr {
public:
    void(*_pLog)(std::string);

    Mgr();
    ~Mgr();

    virtual void Tick(float DeltaSeconds);
};

