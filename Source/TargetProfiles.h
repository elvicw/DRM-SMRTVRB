#pragma once

#include "ReverbEngines.h"

#include <vector>

namespace drmspace
{
enum class SourceTarget { shells, snare, room, overheads };

struct TargetProfile
{
    const char* name;
    float sendHighpassHz;
    float sendLowpassHz;
    float sendLevel;
    ReverbParameters reverb;
};

const TargetProfile& targetProfile(SourceTarget target, Engine engine) noexcept;

// Applies source-specific filtering to the wet send, then runs the selected
// reverb engine. Scratch memory is allocated only in prepare().
class ProfiledReverb
{
public:
    void prepare(double sampleRate, int maximumBlockSize);
    void reset() noexcept;
    void setProfile(SourceTarget target, Engine engine) noexcept;
    void setParameters(const ReverbParameters& parameters) noexcept { reverb.setParameters(parameters); }
    const TargetProfile& getProfile() const noexcept { return *profile; }
    void process(const float* const* input, float* const* output,
                 int channels, int samples) noexcept;

private:
    void updateFilterCoefficients() noexcept;
    float filterInput(int channel, float input) noexcept;

    ReverbEngines reverb;
    const TargetProfile* profile = nullptr;
    double sampleRate = 44100.0;
    int maxBlockSize = 1;
    float highpassCoefficient = 0.0f;
    float lowpassCoefficient = 1.0f;
    float highpassState[2] {};
    float lowpassState[2] {};
    std::vector<float> scratch[2];
};
}
