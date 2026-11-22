#include "ReverbEngines.h"

#include <algorithm>
#include <cmath>

namespace drmspace
{
namespace
{
constexpr std::array<int, 4> roomMs { 29, 37, 41, 53 };
constexpr std::array<int, 4> springMs { 31, 43, 59, 71 };
constexpr std::array<int, 4> gatedMs { 23, 31, 47, 61 };
}

void ReverbEngines::DelayLine::prepare(int size)
{
    data.assign(static_cast<size_t>(std::max(2, size)), 0.0f);
    cursor = 0;
}

void ReverbEngines::DelayLine::reset() noexcept
{
    std::fill(data.begin(), data.end(), 0.0f);
    cursor = 0;
}

float ReverbEngines::DelayLine::read(int delay) const noexcept
{
    if (data.empty()) return 0.0f;
    const auto d = std::clamp(delay, 1, static_cast<int>(data.size()) - 1);
    auto index = cursor - d;
    if (index < 0) index += static_cast<int>(data.size());
    return data[static_cast<size_t>(index)];
}

void ReverbEngines::DelayLine::write(float value) noexcept
{
    if (data.empty()) return;
    data[static_cast<size_t>(cursor)] = std::isfinite(value) ? value : 0.0f;
    if (++cursor == static_cast<int>(data.size())) cursor = 0;
}

float ReverbEngines::Allpass::process(float input, float coefficient) noexcept
{
    const auto delayed = delay.read(static_cast<int>(delay.data.size()) - 1);
    const auto output = delayed - coefficient * input;
    delay.write(input + coefficient * output);
    return output;
}

void ReverbEngines::prepare(double rate, int)
{
    sampleRate = std::clamp(rate, 8000.0, 384000.0);
    const int maxDelay = static_cast<int>(sampleRate * 4.0) + 4;
    for (auto& ch : state)
    {
        ch.predelay.prepare(static_cast<int>(sampleRate * 0.25) + 4);
        ch.early.prepare(static_cast<int>(sampleRate * 0.12) + 4);
        for (auto& line : ch.tank) line.prepare(maxDelay);
        for (auto& diffuser : ch.diffusion) diffuser.delay.prepare(static_cast<int>(sampleRate * 0.006) + 4);
    }
    setParameters(params);
    reset();
}

void ReverbEngines::reset() noexcept
{
    for (auto& ch : state)
    {
        ch.predelay.reset();
        ch.early.reset();
        for (auto& line : ch.tank) line.reset();
        for (auto& diffuser : ch.diffusion) diffuser.delay.reset();
        ch.dampingState.fill(0.0f);
        ch.gateGain = 0.0f;
        ch.gateRemaining = 0;
    }
}

void ReverbEngines::setEngine(Engine value) noexcept { engine = value; }

void ReverbEngines::setParameters(const ReverbParameters& value) noexcept
{
    params.decaySeconds = std::clamp(value.decaySeconds, 0.08f, 8.0f);
    params.preDelayMs = std::clamp(value.preDelayMs, 0.0f, 200.0f);
    params.damping = std::clamp(value.damping, 0.0f, 1.0f);
    params.width = std::clamp(value.width, 0.0f, 2.0f);
    params.gateSeconds = std::clamp(value.gateSeconds, 0.03f, 4.0f);
    predelaySamples = static_cast<int>(sampleRate * params.preDelayMs * 0.001f);
    gateSamples = static_cast<int>(sampleRate * params.gateSeconds);
    // Approximate RT60 feedback for the shortest tank delay. Kept below unity.
    const float shortest = 0.023f;
    feedback = std::clamp(std::pow(0.001f, shortest / params.decaySeconds), 0.0f, 0.985f);
}

float ReverbEngines::readPredelay(ChannelState& ch, float input) noexcept
{
    const auto delayed = ch.predelay.read(std::max(1, predelaySamples));
    ch.predelay.write(input);
    return predelaySamples == 0 ? input : delayed;
}

float ReverbEngines::processRoom(int channel, float input) noexcept
{
    auto& ch = state[static_cast<size_t>(channel)];
    const float excitation = readPredelay(ch, input);
    const float skew = channel == 0 ? 0.0f : 2.7f;
    const int earlyDelays[] = { 11, 19, 31, 43, 59 };
    constexpr float earlyGains[] = { 0.62f, -0.43f, 0.34f, -0.25f, 0.18f };
    float early = 0.0f;
    for (size_t i = 0; i < 5; ++i)
        early += earlyGains[i] * ch.early.read(static_cast<int>(sampleRate * earlyDelays[i] * 0.001f + skew));
    ch.early.write(excitation);

    constexpr float matrix[4][4] = {
        { 0.5f, 0.5f, 0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f, -0.5f },
        { 0.5f, 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f, 0.5f }
    };
    std::array<float, 4> delayed {};
    float sum = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        delayed[static_cast<size_t>(i)] = ch.tank[static_cast<size_t>(i)].read(static_cast<int>(sampleRate * roomMs[static_cast<size_t>(i)] * 0.001f) + channel * (i + 1) * 7);
        auto& lp = ch.dampingState[static_cast<size_t>(i)];
        lp += (1.0f - params.damping * 0.92f) * (delayed[static_cast<size_t>(i)] - lp);
        sum += delayed[static_cast<size_t>(i)];
    }
    for (int i = 0; i < 4; ++i)
    {
        float mixed = 0.0f;
        for (int j = 0; j < 4; ++j) mixed += matrix[i][j] * ch.dampingState[static_cast<size_t>(j)];
        ch.tank[static_cast<size_t>(i)].write(excitation * 0.24f + mixed * feedback * (1.0f - params.damping * 0.08f));
    }
    const float diffuse = ch.diffusion[1].process(ch.diffusion[0].process(sum * 0.22f, 0.67f), 0.61f);
    return early * 0.58f + diffuse * 0.56f;
}

float ReverbEngines::processSpring(int channel, float input) noexcept
{
    auto& ch = state[static_cast<size_t>(channel)];
    const float excitation = readPredelay(ch, input);
    // A spring-like dispersive chain: strongly resonant short lines and serial
    // allpass dispersion create the characteristic pitchy, metallic response.
    float resonant = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        const auto index = static_cast<size_t>(i);
        const int delay = static_cast<int>(sampleRate * springMs[index] * 0.001f) + channel * (i + 2) * 11;
        const float tap = ch.tank[index].read(delay);
        auto& lp = ch.dampingState[index];
        lp += (1.0f - params.damping * 0.8f) * (tap - lp);
        const float resonance = std::min(feedback, 0.97f) * (0.88f + 0.07f * (1.0f - params.damping));
        ch.tank[index].write(excitation * 0.45f + resonance * lp);
        resonant += tap * 0.25f;
    }
    resonant = ch.diffusion[1].process(ch.diffusion[0].process(resonant, 0.76f), 0.68f);
    return resonant;
}

float ReverbEngines::processGated(int channel, float input) noexcept
{
    auto& ch = state[static_cast<size_t>(channel)];
    const float excitation = readPredelay(ch, input);
    if (std::abs(input) > 0.001f) ch.gateRemaining = gateSamples;

    // Dense short-delay feedback tank; termination is a separate timed gate,
    // rather than an attempt to equate RT60 with the tail's audible endpoint.
    float sum = 0.0f;
    std::array<float, 4> taps {};
    for (int i = 0; i < 4; ++i)
    {
        auto& line = ch.tank[static_cast<size_t>(i)];
        const int delay = static_cast<int>(sampleRate * gatedMs[static_cast<size_t>(i)] * 0.001f) + channel * (i + 3) * 5;
        taps[static_cast<size_t>(i)] = line.read(delay);
        auto& lp = ch.dampingState[static_cast<size_t>(i)];
        lp += (1.0f - params.damping * 0.9f) * (taps[static_cast<size_t>(i)] - lp);
        sum += taps[static_cast<size_t>(i)];
    }
    constexpr float signs[4][4] = {
        { 0.5f, 0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f, -0.5f }
    };
    for (int i = 0; i < 4; ++i)
    {
        float mixed = 0.0f;
        for (int j = 0; j < 4; ++j) mixed += signs[i][j] * taps[static_cast<size_t>(j)];
        ch.tank[static_cast<size_t>(i)].write(excitation * 0.32f + mixed * std::min(feedback, 0.88f));
    }
    const float dense = ch.diffusion[1].process(ch.diffusion[0].process(sum * 0.3f, 0.7f), 0.63f);
    if (ch.gateRemaining > 0) --ch.gateRemaining;
    const float target = ch.gateRemaining > 0 ? 1.0f : 0.0f;
    const float coefficient = target > ch.gateGain ? 0.992f : 0.985f;
    ch.gateGain = target + coefficient * (ch.gateGain - target);
    return dense * ch.gateGain;
}

void ReverbEngines::process(const float* const* input, float* const* output,
                            int channels, int samples) noexcept
{
    const int count = std::clamp(channels, 0, 2);
    for (int n = 0; n < samples; ++n)
    {
        float wet[2] {};
        for (int ch = 0; ch < count; ++ch)
        {
            const float x = input[ch] != nullptr ? input[ch][n] : 0.0f;
            wet[ch] = engine == Engine::room ? processRoom(ch, x)
                    : engine == Engine::spring ? processSpring(ch, x)
                    : processGated(ch, x);
            if (!std::isfinite(wet[ch])) wet[ch] = 0.0f;
        }
        if (count == 2)
        {
            const float mid = 0.5f * (wet[0] + wet[1]);
            const float side = 0.5f * (wet[0] - wet[1]) * params.width;
            wet[0] = mid + side;
            wet[1] = mid - side;
        }
        for (int ch = 0; ch < count; ++ch)
            if (output[ch] != nullptr) output[ch][n] = wet[ch];
    }
}
}
