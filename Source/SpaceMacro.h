#pragma once

#include "TargetProfiles.h"

#include <vector>

namespace drmspace
{
struct SpaceMacroResult
{
    ReverbParameters reverb;
    float dryGain = 1.0f;
    float wetGain = 0.0f;
    float normalizedSpace = 0.0f;
};

class SpaceMacro
{
public:
    static SpaceMacroResult evaluate(SourceTarget target, Engine engine, float percent) noexcept;
};

class SpaceMacroSmoother
{
public:
    void prepare(double sampleRate, float rampMilliseconds = 20.0f) noexcept;
    void setRampMilliseconds(float rampMilliseconds) noexcept;
    void reset(const SpaceMacroResult& value) noexcept;
    void setTarget(const SpaceMacroResult& value) noexcept;
    SpaceMacroResult next() noexcept;
    const SpaceMacroResult& current() const noexcept { return currentValue; }

private:
    SpaceMacroResult currentValue;
    SpaceMacroResult targetValue;
    double sampleRate = 44100.0;
    float rampMilliseconds = 20.0f;
    int remainingSamples = 0;
};

// Applies the SPACE macro, smooths its wet/dry gains and parameter targets,
// and mixes the dry input with the profiled wet signal. Call prepare() first.
class SpaceMacroProcessor
{
public:
    void prepare(double sampleRate, int maximumBlockSize,
                 SourceTarget target = SourceTarget::shells,
                 Engine engine = Engine::room);
    void reset() noexcept;
    void setProfile(SourceTarget target, Engine engine) noexcept;
    void setSpace(float percent, float smoothingMilliseconds = 20.0f) noexcept;
    float spacePercent() const noexcept { return smoother.current().normalizedSpace * 100.0f; }
    void process(const float* const* input, float* const* output,
                 int channels, int samples) noexcept;

private:
    ProfiledReverb reverb;
    SpaceMacroSmoother smoother;
    SourceTarget target = SourceTarget::shells;
    Engine engine = Engine::room;
    int maximumBlockSize = 1;
    std::vector<float> wetBuffer[2];
    std::vector<SpaceMacroResult> controlBuffer;
};
}
