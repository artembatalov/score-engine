#pragma once
#include <memory>
#include <string>
#include <vector>
#include "effects.h"
#include <math.h>

class Instrument {
public:
    virtual void AddSetting(std::string& name, std::string& value) {};
    virtual double Sample(
        uint64_t time, 
        uint64_t duration, 
        double frequency, 
        double velocity
    ) { return 0.0f; };
    void AddEffect(std::unique_ptr<Effect>& effect);
    void SetName(std::string& name);
    std::string GetName();
    double Envelope(uint64_t time, uint64_t duration);
    void ApplyEffects(std::vector<double>& buffer) {
        for (auto& eff : effects_) {
            eff->Proccess(buffer);
        }
    }
protected:
    std::vector<std::unique_ptr<Effect> > effects_;
    std::string name_;
    double attack_ = 0;
    double release_ = 0;
};

class Sampler : public Instrument {
public:
    void AddSetting(std::string& name, std::string& value) override;
    double Sample(
        uint64_t time, 
        uint64_t duration, 
        double frequency, 
        double velocity) override;
private:
    void LoadSamples();
    std::vector<int16_t> samples_;
    std::string sample_;
    std::string root_;
    int loop1_ = 0;
    int loop2_ = 0;
};

class Square : public Instrument {
public:
    void AddSetting(std::string& name, std::string& value) override;
    double Sample(
        uint64_t time, 
        uint64_t duration, 
        double frequency, 
        double velocity) override;
private:
    int duty_;
};

class Sine : public Instrument {
private:
    void AddSetting(std::string& name, std::string& value) override;
    double Sample(
        uint64_t time, 
        uint64_t duration, 
        double frequency, 
        double velocity) override;
};

class Triangle : public Instrument {
private:
    void AddSetting(std::string& name, std::string& value) override;
    double Sample(
        uint64_t time, 
        uint64_t duration, 
        double frequency, 
        double velocity) override;
};

class InstrumentMaker {
public:
    static std::unique_ptr<Instrument> Create(const std::string& type);
};