#include "SpaceMacro.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace
{
constexpr std::array<drmspace::SourceTarget, 4> targets {
    drmspace::SourceTarget::shells, drmspace::SourceTarget::snare,
    drmspace::SourceTarget::room, drmspace::SourceTarget::overheads
};
constexpr std::array<drmspace::Engine, 3> engines {
    drmspace::Engine::room, drmspace::Engine::spring, drmspace::Engine::gated
};

void require(bool condition, const char* message)
{
    if (!condition) { std::fprintf(stderr, "FAILED: %s\n", message); std::abort(); }
}
}

int main()
{
    for (const auto target : targets)
        for (const auto engine : engines)
        {
            const auto zero = drmspace::SpaceMacro::evaluate(target, engine, 0.0f);
            require(zero.dryGain == 1.0f && zero.wetGain == 0.0f, "SPACE 0 is exactly dry");
            const auto full = drmspace::SpaceMacro::evaluate(target, engine, 100.0f);
            require(full.dryGain == 0.0f && full.wetGain > 0.0f && full.wetGain <= 1.0f,
                    "SPACE 100 reaches the profile wet cap");
            const auto negative = drmspace::SpaceMacro::evaluate(target, engine, -30.0f);
            const auto over = drmspace::SpaceMacro::evaluate(target, engine, 150.0f);
            require(negative.dryGain == zero.dryGain && negative.wetGain == zero.wetGain,
                    "SPACE input clamps below zero");
            require(over.dryGain == full.dryGain && over.wetGain == full.wetGain,
                    "SPACE input clamps above one hundred");

            float previousWet = 0.0f;
            for (float percent : { 0.0f, 25.0f, 50.0f, 75.0f, 100.0f })
            {
                const auto result = drmspace::SpaceMacro::evaluate(target, engine, percent);
                require(std::isfinite(result.dryGain) && std::isfinite(result.wetGain), "finite macro gains");
                require(result.wetGain + 1.0e-6f >= previousWet, "wet gain grows smoothly across the knob range");
                require(result.reverb.decaySeconds >= 0.08f && result.reverb.decaySeconds <= 8.0f,
                        "macro decay remains within engine bounds");
                previousWet = result.wetGain;
            }
        }

    const auto base = drmspace::SpaceMacro::evaluate(drmspace::SourceTarget::snare, drmspace::Engine::gated, 0.0f);
    const auto maximum = drmspace::SpaceMacro::evaluate(drmspace::SourceTarget::snare, drmspace::Engine::gated, 100.0f);
    drmspace::SpaceMacroSmoother smoother;
    smoother.prepare(48000.0, 20.0f);
    smoother.reset(base);
    smoother.setTarget(maximum);
    const auto first = smoother.next();
    require(first.wetGain > 0.0f && first.wetGain < maximum.wetGain, "automation starts with a gradual wet gain");
    require(first.dryGain < 1.0f && first.dryGain > maximum.dryGain, "automation starts with a gradual dry gain");
    for (int i = 1; i < 960; ++i) smoother.next();
    require(std::abs(smoother.current().wetGain - maximum.wetGain) < 1.0e-6f,
            "SPACE automation reaches its target after the configured ramp");

    drmspace::SpaceMacroProcessor processor;
    processor.prepare(48000.0, 16, drmspace::SourceTarget::snare, drmspace::Engine::room);
    std::array<float, 257> dry {}, output {};
    dry[0] = 0.75f;
    const float* inputs[] { dry.data() };
    float* outputs[] { output.data() };
    processor.process(inputs, outputs, 1, static_cast<int>(dry.size()));
    for (std::size_t i = 0; i < dry.size(); ++i)
        require(output[i] == dry[i], "SPACE 0 produces an exactly dry mono signal");

    processor.setSpace(100.0f, 20.0f);
    dry.fill(0.0f);
    std::array<float, 256> automatedOutput {};
    float* automatedOutputs[] { automatedOutput.data() };
    double wetEnergy = 0.0;
    for (int block = 0; block < 40; ++block)
    {
        processor.process(inputs, automatedOutputs, 1, static_cast<int>(automatedOutput.size()));
        for (const auto sample : automatedOutput)
        {
            require(std::isfinite(sample), "automated output remains finite");
            wetEnergy += static_cast<double>(sample) * sample;
        }
    }
    require(wetEnergy > 1.0e-8, "SPACE automation produces a wet signal");
    require(std::abs(processor.spacePercent() - 100.0f) < 1.0e-4f, "processor reports the settled SPACE value");

    std::puts("DRM-SMRTVRB SPACE macro tests passed");
    return 0;
}
