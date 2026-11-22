#include "TargetProfiles.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
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
    constexpr int frames = 8192;
    constexpr int internalBlock = 64;
    std::array<std::vector<float>, 12> renders;
    std::array<double, 12> energies {};
    std::array<const drmspace::TargetProfile*, 12> seenProfiles {};
    std::size_t profileIndex = 0;
    for (const auto target : targets)
        for (const auto engine : engines)
        {
            const auto& profile = drmspace::targetProfile(target, engine);
            for (std::size_t i = 0; i < profileIndex; ++i)
            {
                const auto& previous = *seenProfiles[i];
                const auto& a = profile.reverb;
                const auto& b = previous.reverb;
                const bool identical = profile.sendHighpassHz == previous.sendHighpassHz
                    && profile.sendLowpassHz == previous.sendLowpassHz && profile.sendLevel == previous.sendLevel
                    && a.decaySeconds == b.decaySeconds && a.preDelayMs == b.preDelayMs
                    && a.damping == b.damping && a.width == b.width && a.gateSeconds == b.gateSeconds
                    && a.density == b.density && a.wetLevel == b.wetLevel;
                require(!identical, "all twelve profiles have distinct tuning");
            }
            seenProfiles[profileIndex] = &profile;
            require(profile.name != nullptr, "each target/engine combination has a profile name");
            require(profile.sendHighpassHz > 0.0f && profile.sendLowpassHz > profile.sendHighpassHz,
                    "profile send has a valid source-specific band");
            require(profile.sendLevel > 0.0f && profile.sendLevel <= 1.0f, "profile send level in range");
            require(profile.reverb.decaySeconds >= 0.08f && profile.reverb.decaySeconds <= 8.0f,
                    "profile decay in range");
            require(profile.reverb.preDelayMs >= 0.0f && profile.reverb.preDelayMs <= 200.0f,
                    "profile pre-delay in range");
            require(profile.reverb.density >= 0.0f && profile.reverb.density <= 1.0f,
                    "profile density in range");
            require(profile.reverb.wetLevel > 0.0f && profile.reverb.wetLevel <= 2.0f,
                    "profile wet level in range");

            drmspace::ProfiledReverb reverb;
            reverb.prepare(48000.0, internalBlock);
            reverb.setProfile(target, engine);
            std::vector<float> inL(frames, 0.0f), inR(frames, 0.0f);
            renders[profileIndex].resize(frames * 2);
            inL[0] = 1.0f;
            inR[0] = 0.63f;
            const float* input[] { inL.data(), inR.data() };
            float* output[] { renders[profileIndex].data(), renders[profileIndex].data() + frames };
            reverb.process(input, output, 2, frames); // Exercises chunking above the prepared block size.
            float peak = 0.0f;
            for (int i = 0; i < frames; ++i)
            {
                const auto l = output[0][i], r = output[1][i];
                require(std::isfinite(l) && std::isfinite(r), "profile output remains finite");
                peak = std::max(peak, std::max(std::abs(l), std::abs(r)));
                energies[profileIndex] += static_cast<double>(l) * l + static_cast<double>(r) * r;
            }
            require(peak < 8.0f, "profile output remains bounded");
            require(energies[profileIndex] > 1.0e-8, "each of twelve profiles produces a wet response");
            ++profileIndex;
        }

    double shellsSnareDifference = 0.0;
    for (int i = 0; i < frames * 2; ++i)
        shellsSnareDifference += std::abs(renders[0][static_cast<std::size_t>(i)]
                                          - renders[3][static_cast<std::size_t>(i)]);
    require(shellsSnareDifference > 0.01, "target wet-send conditioning changes the response");

    // Mono callers may provide one-element channel pointer arrays.
    drmspace::ProfiledReverb monoReverb;
    monoReverb.prepare(44100.0, 32);
    monoReverb.setProfile(drmspace::SourceTarget::overheads, drmspace::Engine::room);
    std::array<float, 257> monoIn {}, monoOut {};
    monoIn[0] = 1.0f;
    const float* monoInputs[] { monoIn.data() };
    float* monoOutputs[] { monoOut.data() };
    monoReverb.process(monoInputs, monoOutputs, 1, static_cast<int>(monoIn.size()));
    for (const auto sample : monoOut) require(std::isfinite(sample), "mono profile output remains finite");

    drmspace::ProfiledReverb invalidInputReverb;
    invalidInputReverb.prepare(std::numeric_limits<double>::quiet_NaN(), 16);
    invalidInputReverb.setProfile(drmspace::SourceTarget::shells, drmspace::Engine::room);
    std::array<float, 32> invalidInput {}, invalidOutput {};
    invalidInput[0] = std::numeric_limits<float>::quiet_NaN();
    const float* invalidInputs[] { invalidInput.data() };
    float* invalidOutputs[] { invalidOutput.data() };
    invalidInputReverb.process(invalidInputs, invalidOutputs, 1, static_cast<int>(invalidInput.size()));
    for (const auto sample : invalidOutput) require(std::isfinite(sample), "non-finite input and sample rate are contained");

    std::puts("DRM-SMRTVRB target profile tests passed (12 profiles)");
    return 0;
}
