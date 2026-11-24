#include "WetFilters.h"

#include <algorithm>
#include <cmath>

namespace drmspace
{
namespace
{
constexpr float glideSeconds = 0.02f;
constexpr float edgeTolerance = 1.005f;
constexpr float pi = 3.14159265358979323846f;
}

void WetFilters::prepare(double rate) noexcept
{
    sampleRate = rate > 1000.0 ? rate : 44100.0;
    highPassCurrent = highPassTarget;
    lowPassCurrent = lowPassTarget;
    reset();
}

void WetFilters::reset() noexcept
{
    highPass.clear();
    lowPass.clear();
    highPass.active = highPassCurrent > highPassMinHz * edgeTolerance;
    lowPass.active = lowPassCurrent < lowPassMaxHz / edgeTolerance;
    if (highPass.active) updateCoefficients(highPass, highPassCurrent, true);
    if (lowPass.active) updateCoefficients(lowPass, lowPassCurrent, false);
}

void WetFilters::setCutoffs(float highPassHz, float lowPassHz) noexcept
{
    highPassTarget = std::clamp(highPassHz, highPassMinHz, highPassMaxHz);
    lowPassTarget = std::clamp(lowPassHz, lowPassMinHz, lowPassMaxHz);
}

float WetFilters::glide(float current, float target, int samples) const noexcept
{
    if (current == target) return target;
    const float amount = 1.0f - std::exp(-static_cast<float>(samples) / (static_cast<float>(sampleRate) * glideSeconds));
    const float next = std::exp(std::log(current) + (std::log(target) - std::log(current)) * amount);
    return std::abs(next / target - 1.0f) < 0.005f ? target : next;
}

void WetFilters::updateCoefficients(Biquad& filter, float hz, bool highPassMode) const noexcept
{
    const float limited = std::min(hz, static_cast<float>(sampleRate) * 0.45f);
    const float omega = 2.0f * pi * limited / static_cast<float>(sampleRate);
    const float cosine = std::cos(omega);
    const float alpha = std::sin(omega) / (2.0f * 0.70710678f);
    const float a0 = 1.0f + alpha;
    const float edge = highPassMode ? (1.0f + cosine) * 0.5f : (1.0f - cosine) * 0.5f;
    filter.b0 = edge / a0;
    filter.b1 = (highPassMode ? -2.0f * edge : 2.0f * edge) / a0;
    filter.b2 = edge / a0;
    filter.a1 = -2.0f * cosine / a0;
    filter.a2 = (1.0f - alpha) / a0;
}

void WetFilters::Biquad::run(float* data, int channel, int samples) noexcept
{
    auto& s1 = z1[static_cast<std::size_t>(channel)];
    auto& s2 = z2[static_cast<std::size_t>(channel)];
    for (int i = 0; i < samples; ++i)
    {
        const float in = data[i];
        const float out = b0 * in + s1;
        s1 = b1 * in - a1 * out + s2;
        s2 = b2 * in - a2 * out;
        data[i] = out;
    }
}

void WetFilters::process(float* const* data, int channels, int samples) noexcept
{
    if (samples <= 0) return;
    channels = std::min(channels, 2);

    highPassCurrent = glide(highPassCurrent, highPassTarget, samples);
    lowPassCurrent = glide(lowPassCurrent, lowPassTarget, samples);

    const bool highPassWanted = highPassCurrent > highPassMinHz * edgeTolerance;
    const bool lowPassWanted = lowPassCurrent < lowPassMaxHz / edgeTolerance;
    if (highPassWanted && !highPass.active) highPass.clear();
    if (lowPassWanted && !lowPass.active) lowPass.clear();
    highPass.active = highPassWanted;
    lowPass.active = lowPassWanted;
    if (highPass.active) updateCoefficients(highPass, highPassCurrent, true);
    if (lowPass.active) updateCoefficients(lowPass, lowPassCurrent, false);

    for (int channel = 0; channel < channels; ++channel)
    {
        if (highPass.active) highPass.run(data[channel], channel, samples);
        if (lowPass.active) lowPass.run(data[channel], channel, samples);
    }
}
}
