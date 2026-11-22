#include "TargetProfiles.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace drmspace
{
namespace
{
// Each target/engine pair is independently tuned. The wet send filters keep
// source-specific buildup out of the reverb excitation; the engine controls
// separately set decay, density, timing, damping, width, and tail behavior.
constexpr TargetProfile profiles[4][3] {
    {
        { "Shells + Room", 85.0f, 15000.0f, 0.65f, { 0.80f, 9.0f, 0.60f, 0.75f, 0.35f, 0.58f, 0.72f } },
        { "Shells + Spring", 100.0f, 14000.0f, 0.58f, { 1.00f, 12.0f, 0.42f, 0.85f, 0.38f, 0.72f, 0.68f } },
        { "Shells + Gated", 85.0f, 16000.0f, 0.78f, { 0.58f, 4.0f, 0.52f, 0.90f, 0.30f, 0.84f, 0.80f } }
    },
    {
        { "Snare + Room", 70.0f, 17000.0f, 0.76f, { 1.35f, 16.0f, 0.42f, 1.05f, 0.32f, 0.72f, 0.80f } },
        { "Snare + Spring", 90.0f, 16000.0f, 0.70f, { 1.65f, 22.0f, 0.30f, 1.12f, 0.42f, 0.80f, 0.74f } },
        { "Snare + Gated", 80.0f, 17000.0f, 0.92f, { 0.85f, 6.0f, 0.44f, 1.16f, 0.28f, 0.92f, 0.96f } }
    },
    {
        { "Room + Room", 90.0f, 13000.0f, 0.52f, { 1.50f, 4.0f, 0.62f, 0.90f, 0.40f, 0.82f, 0.62f } },
        { "Room + Spring", 130.0f, 12000.0f, 0.45f, { 1.15f, 9.0f, 0.52f, 0.82f, 0.45f, 0.68f, 0.55f } },
        { "Room + Gated", 110.0f, 11000.0f, 0.45f, { 0.75f, 1.0f, 0.70f, 0.80f, 0.22f, 0.72f, 0.48f } }
    },
    {
        { "Overheads + Room", 170.0f, 9000.0f, 0.38f, { 0.72f, 5.0f, 0.82f, 0.72f, 0.40f, 0.65f, 0.48f } },
        { "Overheads + Spring", 200.0f, 8500.0f, 0.32f, { 0.82f, 8.0f, 0.72f, 0.70f, 0.45f, 0.62f, 0.38f } },
        { "Overheads + Gated", 180.0f, 9000.0f, 0.38f, { 0.48f, 0.0f, 0.85f, 0.66f, 0.16f, 0.68f, 0.43f } }
    }
};

std::size_t targetIndex(SourceTarget target) noexcept
{
    switch (target)
    {
        case SourceTarget::shells: return 0;
        case SourceTarget::snare: return 1;
        case SourceTarget::room: return 2;
        case SourceTarget::overheads: return 3;
    }
    return 0;
}

std::size_t engineIndex(Engine engine) noexcept
{
    switch (engine)
    {
        case Engine::room: return 0;
        case Engine::spring: return 1;
        case Engine::gated: return 2;
    }
    return 0;
}
}

const TargetProfile& targetProfile(SourceTarget target, Engine engine) noexcept
{
    return profiles[targetIndex(target)][engineIndex(engine)];
}

void ProfiledReverb::prepare(double rate, int blockSize)
{
    sampleRate = std::clamp(rate, 8000.0, 384000.0);
    maxBlockSize = std::max(1, blockSize);
    for (auto& buffer : scratch) buffer.assign(static_cast<std::size_t>(maxBlockSize), 0.0f);
    reverb.prepare(sampleRate, maxBlockSize);
    if (profile == nullptr) setProfile(SourceTarget::shells, Engine::room);
    updateFilterCoefficients();
    reset();
}

void ProfiledReverb::reset() noexcept
{
    reverb.reset();
    for (int channel = 0; channel < 2; ++channel)
    {
        highpassState[channel] = 0.0f;
        lowpassState[channel] = 0.0f;
    }
}

void ProfiledReverb::setProfile(SourceTarget target, Engine engine) noexcept
{
    profile = &targetProfile(target, engine);
    reverb.setEngine(engine);
    reverb.setParameters(profile->reverb);
    updateFilterCoefficients();
}

void ProfiledReverb::updateFilterCoefficients() noexcept
{
    if (profile == nullptr) return;
    constexpr double twoPi = 6.2831853071795864769;
    const auto hp = std::clamp<double>(profile->sendHighpassHz, 1.0, sampleRate * 0.45);
    const auto lp = std::clamp<double>(profile->sendLowpassHz, hp + 1.0, sampleRate * 0.48);
    highpassCoefficient = static_cast<float>(1.0 - std::exp(-twoPi * hp / sampleRate));
    lowpassCoefficient = static_cast<float>(1.0 - std::exp(-twoPi * lp / sampleRate));
}

float ProfiledReverb::filterInput(int channel, float input) noexcept
{
    if (!std::isfinite(input)) input = 0.0f;
    auto& hpState = highpassState[channel];
    hpState += highpassCoefficient * (input - hpState);
    const auto highPassed = input - hpState;
    auto& lpState = lowpassState[channel];
    lpState += lowpassCoefficient * (highPassed - lpState);
    return lpState * profile->sendLevel;
}

void ProfiledReverb::process(const float* const* input, float* const* output,
                             int channels, int samples) noexcept
{
    if (output == nullptr || profile == nullptr || samples <= 0) return;
    const int count = std::clamp(channels, 0, 2);
    for (int offset = 0; offset < samples; offset += maxBlockSize)
    {
        const int n = std::min(maxBlockSize, samples - offset);
        for (int channel = 0; channel < count; ++channel)
            for (int i = 0; i < n; ++i)
            {
                const float sample = input != nullptr && input[channel] != nullptr ? input[channel][offset + i] : 0.0f;
                scratch[channel][static_cast<std::size_t>(i)] = filterInput(channel, sample);
            }
        const float* wetInput[] { scratch[0].data(), scratch[1].data() };
        float* wetOutput[2] {};
        for (int channel = 0; channel < count; ++channel)
            wetOutput[channel] = output[channel] != nullptr ? output[channel] + offset : nullptr;
        reverb.process(wetInput, wetOutput, count, n);
    }
}
}
