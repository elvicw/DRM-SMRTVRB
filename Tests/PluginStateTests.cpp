#include "PluginState.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace
{
void require(bool condition, const char* message)
{
    if (!condition) { std::fprintf(stderr, "FAILED: %s\n", message); std::abort(); }
}

bool same(const drmspace::PluginState& a, const drmspace::PluginState& b)
{
    const auto& x = a.advanced;
    const auto& y = b.advanced;
    return a.target == b.target && a.engine == b.engine && a.spacePercent == b.spacePercent
        && a.syncMode == b.syncMode && a.division == b.division && a.freeDecaySeconds == b.freeDecaySeconds
        && x.preDelayEnabled == y.preDelayEnabled && x.toneEnabled == y.toneEnabled
        && x.widthEnabled == y.widthEnabled && x.mixEnabled == y.mixEnabled && x.tailEnabled == y.tailEnabled
        && x.preDelayMs == y.preDelayMs && x.tone == y.tone && x.width == y.width
        && x.mix == y.mix && x.tailSeconds == y.tailSeconds;
}
}

int main()
{
    const auto macro = drmspace::SpaceMacro::evaluate(drmspace::SourceTarget::snare,
                                                       drmspace::Engine::gated, 75.0f);
    const drmspace::AdvancedControls disabled;
    const auto untouched = drmspace::applyAdvancedControls(macro, disabled);
    require(untouched.reverb.decaySeconds == macro.reverb.decaySeconds
            && untouched.dryGain == macro.dryGain && untouched.wetGain == macro.wetGain,
            "disabled advanced controls preserve the SPACE result");

    drmspace::AdvancedControls advanced;
    advanced.preDelayEnabled = advanced.toneEnabled = advanced.widthEnabled = true;
    advanced.mixEnabled = advanced.tailEnabled = true;
    advanced.preDelayMs = 2.0f;
    advanced.tone = 0.8f;
    advanced.width = 1.7f;
    advanced.mix = 0.5f;
    advanced.tailSeconds = 0.31f;
    const auto changed = drmspace::applyAdvancedControls(macro, advanced);
    require(changed.reverb.preDelayMs == 2.0f && changed.reverb.width == 1.7f,
            "advanced pre-delay and width override macro values");
    require(changed.reverb.damping < macro.reverb.damping, "positive Tone brightens by reducing damping");
    require(changed.reverb.decaySeconds == 0.31f && changed.reverb.gateSeconds == 0.31f,
            "Tail override controls decay and gated termination");
    require(changed.wetGain < macro.wetGain && changed.wetGain > 0.0f,
            "manual Mix adjusts the macro wet amount");
    const auto zeroSpace = drmspace::SpaceMacro::evaluate(drmspace::SourceTarget::snare,
                                                           drmspace::Engine::gated, 0.0f);
    const auto zeroMix = drmspace::applyAdvancedControls(zeroSpace, advanced);
    require(zeroMix.dryGain == 1.0f && zeroMix.wetGain == 0.0f,
            "advanced overrides preserve exact dry at SPACE zero");

    std::size_t count = 0;
    const auto* presets = drmspace::factoryPresets(count);
    constexpr std::array<const char*, 8> expectedNames {
        "Tight Shells", "Big Snare", "Explosive Snare", "Small Studio",
        "Large Room", "Wide Overheads", "Metallic Spring", "Vintage Gated"
    };
    require(count == expectedNames.size(), "eight requested factory presets exist");
    for (std::size_t i = 0; i < count; ++i)
    {
        require(std::string(presets[i].name) == expectedNames[i], "factory preset name matches roadmap");
        require(drmspace::findFactoryPreset(expectedNames[i]) == &presets[i], "lookup finds each factory preset");
        const auto serialized = drmspace::StateCodec::encode(presets[i].state);
        require(!serialized.empty(), "preset state serializes");
        drmspace::PluginState restored;
        std::string error;
        require(drmspace::StateCodec::decode(serialized, restored, error), "preset state deserializes");
        require(same(restored, presets[i].state), "preset round-trip recalls every parameter");
    }
    require(drmspace::findFactoryPreset("not a preset") == nullptr, "unknown preset lookup returns null");

    auto futureCompatible = drmspace::StateCodec::encode(presets[0].state);
    futureCompatible += "futureField=ignored\r\n";
    drmspace::PluginState compatible;
    std::string error;
    require(drmspace::StateCodec::decode(futureCompatible, compatible, error),
            "unknown future fields and CRLF line endings are tolerated");
    require(same(compatible, presets[0].state), "unknown fields do not alter known values");

    auto unchanged = presets[1].state;
    require(!drmspace::StateCodec::decode("DRM-SMRTVRB_STATE=99\n", unchanged, error),
            "unsupported state version is rejected");
    require(same(unchanged, presets[1].state), "failed state load does not partially mutate the destination");
    drmspace::PluginState invalid;
    invalid.spacePercent = 120.0f;
    require(drmspace::StateCodec::encode(invalid).empty(),
            "invalid state does not serialize");

    std::puts("DRM-SMRTVRB advanced controls and preset tests passed");
    return 0;
}
