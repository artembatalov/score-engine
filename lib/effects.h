#pragma once
#include <memory>
#include <string>
#include <vector>

class Effect {
public:
    virtual void AddSetting(std::string& name, std::string& value) {};
    virtual void Proccess(std::vector<double>&buffer) {};
protected:
    bool Validate(std::string& value);
};

class Gain : public Effect {
public:
    void AddSetting(std::string& name, std::string& value) override;
    void Proccess(std::vector<double>&buffer) override;
private:
    double gain_;
};

class Echo : public Effect {
public:
    void AddSetting(std::string& name, std::string& value) override;
    void Proccess(std::vector<double>&buffer) override;
private:
    size_t delay_samples_;
    double decay_;
};

class Tremolo : public Effect {
public:
    void AddSetting(std::string& name, std::string& value) override;
    void Proccess(std::vector<double>&buffer) override;
private:
    double freq_;
    double depth_;
};

class EffectMaker {
public:
    static std::unique_ptr<Effect> Create(std::string& type);
};