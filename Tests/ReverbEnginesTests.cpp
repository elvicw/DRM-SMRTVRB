#include "ReverbEngines.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace
{
constexpr int block = 256;
constexpr int count = 48128;

void require(bool condition, const char* message);

struct Result
{
    std::vector<float> left;
    std::vector<float> right;
    double energy = 0.0;
    float peak = 0.0f;
};

Result render(drmspace::Engine engine, int channels)
{
    drmspace::ReverbEngines reverb;
    reverb.prepare(48000.0, block);
    reverb.setEngine(engine);
    drmspace::ReverbParameters p;
    p.decaySeconds = 1.1f;
    p.preDelayMs = 8.0f;
    p.gateSeconds = 0.2f;
    reverb.setParameters(p);

    Result result;
    result.left.resize(count);
    result.right.resize(count);
    std::array<float, block> inL {}, inR {}, outL {}, outR {};
    inL[0] = 1.0f;
    inR[0] = channels == 2 ? 0.7f : 0.0f;
    const float* inputs[] { inL.data(), inR.data() };
    float* outputs[] { outL.data(), outR.data() };
    for (int offset = 0; offset < count; offset += block)
    {
        inL.fill(0.0f);
        inR.fill(0.0f);
        if (offset == 0) { inL[0] = 1.0f; inR[0] = channels == 2 ? 0.7f : 0.0f; }
        reverb.process(inputs, outputs, channels, block);
        for (int i = 0; i < block; ++i)
        {
            result.left[static_cast<size_t>(offset + i)] = outL[static_cast<size_t>(i)];
            result.right[static_cast<size_t>(offset + i)] = outR[static_cast<size_t>(i)];
            require(std::isfinite(outL[static_cast<size_t>(i)]), "finite left output");
            require(std::isfinite(outR[static_cast<size_t>(i)]), "finite right output");
            result.peak = std::max(result.peak, std::abs(outL[static_cast<size_t>(i)]));
            result.energy += static_cast<double>(outL[static_cast<size_t>(i)]) * outL[static_cast<size_t>(i)];
        }
    }
    return result;
}

void require(bool condition, const char* message)
{
    if (!condition) { std::fprintf(stderr, "FAILED: %s\n", message); std::abort(); }
}

double windowEnergy(const Result& result, int start, int end)
{
    double value = 0.0;
    for (int i = start; i < end; ++i)
    {
        const auto sample = result.left[static_cast<size_t>(i)];
        value += static_cast<double>(sample) * sample;
    }
    return value;
}

void stressSampleRatesAndBlocks()
{
    constexpr std::array<double, 3> rates { 44100.0, 48000.0, 96000.0 };
    constexpr std::array<int, 5> blocks { 32, 64, 128, 256, 1024 };
    constexpr std::array<drmspace::Engine, 3> engines {
        drmspace::Engine::room, drmspace::Engine::spring, drmspace::Engine::gated
    };
    for (std::size_t e = 0; e < engines.size(); ++e)
        for (std::size_t r = 0; r < rates.size(); ++r)
            for (std::size_t b = 0; b < blocks.size(); ++b)
            {
                drmspace::ReverbEngines reverb;
                reverb.prepare(rates[r], blocks[b]);
                reverb.setEngine(engines[e]);
                drmspace::ReverbParameters p;
                const bool high = ((e + r + b) & 1u) != 0;
                p.decaySeconds = high ? 8.0f : 0.08f;
                p.preDelayMs = high ? 200.0f : 0.0f;
                p.damping = high ? 0.0f : 1.0f;
                p.width = high ? 2.0f : 0.0f;
                p.gateSeconds = high ? 4.0f : 0.03f;
                reverb.setParameters(p);

                std::vector<float> inL(static_cast<std::size_t>(blocks[b]));
                std::vector<float> inR(static_cast<std::size_t>(blocks[b]));
                std::vector<float> outL(inL.size()), outR(inR.size());
                const float* input[] { inL.data(), inR.data() };
                float* output[] { outL.data(), outR.data() };
                const auto frames = static_cast<int>(rates[r] * 0.35);
                unsigned noise = 0x1234567u;
                float peak = 0.0f;
                for (int offset = 0; offset < frames; offset += blocks[b])
                {
                    const int n = std::min(blocks[b], frames - offset);
                    for (int i = 0; i < n; ++i)
                    {
                        const int absolute = offset + i;
                        const float hit = absolute % 8192 == 0 ? 0.9f : 0.0f;
                        noise = noise * 1664525u + 1013904223u;
                        const float texture = static_cast<float>((noise >> 8) & 0xffffu) / 65535.0f - 0.5f;
                        inL[static_cast<std::size_t>(i)] = hit + texture * 0.025f;
                        inR[static_cast<std::size_t>(i)] = hit * 0.8f - texture * 0.02f;
                    }
                    reverb.process(input, output, 2, n);
                    for (int i = 0; i < n; ++i)
                    {
                        const auto l = outL[static_cast<std::size_t>(i)];
                        const auto rr = outR[static_cast<std::size_t>(i)];
                        require(std::isfinite(l) && std::isfinite(rr), "finite output at all rates, blocks, and control extremes");
                        peak = std::max(peak, std::max(std::abs(l), std::abs(rr)));
                    }
                }
                require(peak < 12.0f, "bounded output at all rates, blocks, and control extremes");
            }
}
}

int main()
{
    const auto room = render(drmspace::Engine::room, 2);
    const auto spring = render(drmspace::Engine::spring, 2);
    const auto gated = render(drmspace::Engine::gated, 2);
    require(room.energy > 1.0e-5, "Room produces a nonzero tail");
    require(spring.energy > 1.0e-5, "Spring produces a nonzero tail");
    require(gated.energy > 1.0e-5, "Gated produces a nonzero tail");
    require(room.peak < 8.0f && spring.peak < 8.0f && gated.peak < 8.0f, "bounded output for impulse input");

    double roomSpringDifference = 0.0;
    double roomGatedDifference = 0.0;
    for (int i = 0; i < count; ++i)
    {
        roomSpringDifference += std::abs(room.left[static_cast<size_t>(i)] - spring.left[static_cast<size_t>(i)]);
        roomGatedDifference += std::abs(room.left[static_cast<size_t>(i)] - gated.left[static_cast<size_t>(i)]);
    }
    require(roomSpringDifference > 0.1, "Room and Spring have distinct responses");
    require(roomGatedDifference > 0.1, "Room and Gated have distinct responses");
    require(windowEnergy(gated, 2400, 7200) > 1.0e-5, "Gated output is audible while its timed gate is open");
    require(windowEnergy(gated, 28800, 38400) < windowEnergy(gated, 2400, 7200) * 0.02,
            "Gated tail is terminated after its selected time");

    const auto mono = render(drmspace::Engine::room, 1);
    require(mono.energy > 1.0e-5, "mono processing produces a tail");

    stressSampleRatesAndBlocks();

    std::puts("DRM-SMRTVRB DSP engine tests passed");
    return 0;
}
