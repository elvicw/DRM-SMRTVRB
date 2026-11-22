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

    std::puts("DRM-SMRTVRB DSP engine tests passed");
    return 0;
}
