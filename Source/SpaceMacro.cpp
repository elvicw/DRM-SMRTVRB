#include "SpaceMacro.h"

#include <algorithm>
#include <cmath>

namespace drmspace
{
namespace
{
struct Curve
{
    float exponent;
    float decayIncrease;
    float preDelayIncrease;
    float dampingChange;
    float densityIncrease;
    float widthAtFull;
    float gateScale;
    float wetCap;
};

constexpr Curve curves[4][3] {
    {
        { 1.00f, 0.70f, 15.0f, -0.05f, 0.20f, 0.95f, 1.00f, 0.75f },
        { 1.20f, 0.85f, 18.0f, -0.12f, 0.18f, 1.05f, 1.00f, 0.72f },
        { 0.82f, 0.35f, 4.0f, -0.08f, 0.12f, 1.10f, 1.15f, 0.82f }
    },
    {
        { 0.90f, 0.90f, 22.0f, -0.08f, 0.20f, 1.25f, 1.00f, 0.80f },
        { 1.00f, 1.20f, 25.0f, -0.15f, 0.16f, 1.30f, 1.00f, 0.78f },
        { 0.78f, 0.45f, 8.0f, -0.08f, 0.08f, 1.40f, 1.50f, 0.90f }
    },
    {
        { 1.30f, 0.55f, 10.0f, -0.05f, 0.12f, 1.00f, 1.00f, 0.55f },
        { 1.40f, 0.75f, 12.0f, -0.12f, 0.14f, 1.10f, 1.00f, 0.48f },
        { 1.10f, 0.40f, 3.0f, -0.05f, 0.08f, 0.98f, 1.10f, 0.42f }
    },
    {
        { 1.40f, 0.45f, 8.0f, 0.08f, 0.10f, 0.92f, 1.00f, 0.38f },
        { 1.50f, 0.55f, 10.0f, 0.00f, 0.12f, 0.90f, 1.00f, 0.33f },
        { 1.20f, 0.25f, 3.0f, 0.05f, 0.08f, 0.86f, 1.20f, 0.30f }
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

float approach(float current, float target, float fraction) noexcept
{
    return current + (target - current) * fraction;
}

void approachParameters(ReverbParameters& current, const ReverbParameters& target, float fraction) noexcept
{
    current.decaySeconds = approach(current.decaySeconds, target.decaySeconds, fraction);
    current.preDelayMs = approach(current.preDelayMs, target.preDelayMs, fraction);
    current.damping = approach(current.damping, target.damping, fraction);
    current.width = approach(current.width, target.width, fraction);
    current.gateSeconds = approach(current.gateSeconds, target.gateSeconds, fraction);
    current.density = approach(current.density, target.density, fraction);
    current.wetLevel = approach(current.wetLevel, target.wetLevel, fraction);
}
}

SpaceMacroResult SpaceMacro::evaluate(SourceTarget target, Engine engine, float percent) noexcept
{
    const auto& base = targetProfile(target, engine);
    const auto& curve = curves[targetIndex(target)][engineIndex(engine)];
    const float safePercent = std::isfinite(percent) ? percent : 0.0f;
    const float x = std::clamp(safePercent, 0.0f, 100.0f) * 0.01f;
    const float smooth = x * x * (3.0f - 2.0f * x);
    const float amount = std::pow(smooth, curve.exponent);
    SpaceMacroResult result;
    result.normalizedSpace = x;
    result.reverb = base.reverb;
    result.reverb.decaySeconds = std::clamp(base.reverb.decaySeconds * (1.0f + curve.decayIncrease * amount), 0.08f, 8.0f);
    result.reverb.preDelayMs = std::clamp(base.reverb.preDelayMs + curve.preDelayIncrease * amount, 0.0f, 200.0f);
    result.reverb.damping = std::clamp(base.reverb.damping + curve.dampingChange * amount, 0.0f, 1.0f);
    result.reverb.density = std::clamp(base.reverb.density + curve.densityIncrease * amount, 0.0f, 1.0f);
    result.reverb.width = std::clamp(base.reverb.width + (curve.widthAtFull - base.reverb.width) * amount, 0.0f, 2.0f);
    result.reverb.gateSeconds = std::clamp(base.reverb.gateSeconds * (1.0f + curve.gateScale * amount), 0.03f, 4.0f);
    result.reverb.wetLevel = std::clamp(base.reverb.wetLevel * (0.85f + 0.15f * amount), 0.0f, 2.0f);
    constexpr float halfPi = 1.57079632679489661923f;
    result.dryGain = x >= 1.0f ? 0.0f : std::cos(halfPi * x);
    result.wetGain = x <= 0.0f ? 0.0f : std::sin(halfPi * x) * curve.wetCap;
    return result;
}

void SpaceMacroSmoother::prepare(double rate, float rampMs) noexcept
{
    sampleRate = std::clamp(std::isfinite(rate) ? rate : 44100.0, 8000.0, 384000.0);
    setRampMilliseconds(rampMs);
    remainingSamples = 0;
}

void SpaceMacroSmoother::setRampMilliseconds(float rampMs) noexcept
{
    rampMilliseconds = std::clamp(std::isfinite(rampMs) ? rampMs : 20.0f, 1.0f, 100.0f);
}

void SpaceMacroSmoother::reset(const SpaceMacroResult& value) noexcept
{
    currentValue = targetValue = value;
    remainingSamples = 0;
}

void SpaceMacroSmoother::setTarget(const SpaceMacroResult& value) noexcept
{
    targetValue = value;
    remainingSamples = std::max(1, static_cast<int>(sampleRate * rampMilliseconds * 0.001));
}

SpaceMacroResult SpaceMacroSmoother::next() noexcept
{
    if (remainingSamples <= 0) return currentValue;
    const float fraction = 1.0f / static_cast<float>(remainingSamples);
    approachParameters(currentValue.reverb, targetValue.reverb, fraction);
    currentValue.dryGain = approach(currentValue.dryGain, targetValue.dryGain, fraction);
    currentValue.wetGain = approach(currentValue.wetGain, targetValue.wetGain, fraction);
    currentValue.normalizedSpace = approach(currentValue.normalizedSpace, targetValue.normalizedSpace, fraction);
    if (--remainingSamples == 0) currentValue = targetValue;
    return currentValue;
}

void SpaceMacroProcessor::prepare(double rate, int blockSize, SourceTarget initialTarget, Engine initialEngine)
{
    maximumBlockSize = std::max(1, blockSize);
    for (auto& buffer : wetBuffer) buffer.assign(static_cast<std::size_t>(maximumBlockSize), 0.0f);
    controlBuffer.assign(static_cast<std::size_t>(maximumBlockSize), {});
    reverb.prepare(rate, maximumBlockSize);
    smoother.prepare(rate);
    target = initialTarget;
    engine = initialEngine;
    reverb.setProfile(target, engine);
    smoother.reset(SpaceMacro::evaluate(target, engine, 0.0f));
}

void SpaceMacroProcessor::reset() noexcept
{
    reverb.reset();
    smoother.reset(SpaceMacro::evaluate(target, engine, 0.0f));
}

void SpaceMacroProcessor::setProfile(SourceTarget nextTarget, Engine nextEngine) noexcept
{
    target = nextTarget;
    engine = nextEngine;
    reverb.setProfile(target, engine);
    smoother.setTarget(SpaceMacro::evaluate(target, engine, spacePercent()));
}

void SpaceMacroProcessor::setSpace(float percent, float smoothingMs) noexcept
{
    smoother.setRampMilliseconds(smoothingMs);
    smoother.setTarget(SpaceMacro::evaluate(target, engine, percent));
}

void SpaceMacroProcessor::process(const float* const* input, float* const* output,
                                  int channels, int samples) noexcept
{
    if (output == nullptr || samples <= 0) return;
    const int count = std::clamp(channels, 0, 2);
    constexpr int parameterUpdateQuantum = 32;
    for (int offset = 0; offset < samples;)
    {
        const int n = std::min({ parameterUpdateQuantum, maximumBlockSize, samples - offset });
        for (int i = 0; i < n; ++i) controlBuffer[static_cast<std::size_t>(i)] = smoother.next();
        reverb.setParameters(controlBuffer[0].reverb);
        const float* chunkInput[2] {};
        float* chunkWet[2] { wetBuffer[0].data(), wetBuffer[1].data() };
        for (int channel = 0; channel < count; ++channel)
            chunkInput[channel] = input != nullptr && input[channel] != nullptr ? input[channel] + offset : nullptr;
        reverb.process(chunkInput, chunkWet, count, n);
        for (int channel = 0; channel < count; ++channel)
            if (output[channel] != nullptr)
                for (int i = 0; i < n; ++i)
                {
                    const auto& control = controlBuffer[static_cast<std::size_t>(i)];
                    const float dry = chunkInput[channel] != nullptr ? chunkInput[channel][i] : 0.0f;
                    output[channel][offset + i] = dry * control.dryGain
                        + wetBuffer[channel][static_cast<std::size_t>(i)] * control.wetGain;
                }
        offset += n;
    }
}
}
