#pragma once

#include "SpaceMacro.h"
#include "TempoSync.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace drmspace
{
struct AdvancedControls
{
    bool preDelayEnabled = false;
    bool toneEnabled = false;
    bool widthEnabled = false;
    bool mixEnabled = false;
    bool tailEnabled = false;
    float preDelayMs = 12.0f;
    float tone = 0.0f; // -1 dark, +1 bright
    float width = 1.0f;
    float mix = 1.0f; // wet proportion when the Mix override is enabled
    float tailSeconds = 1.2f;
};

struct AdvancedResult
{
    ReverbParameters reverb;
    float dryGain = 1.0f;
    float wetGain = 0.0f;
};

AdvancedResult applyAdvancedControls(const SpaceMacroResult& macro,
                                     const AdvancedControls& controls) noexcept;

struct PluginState
{
    SourceTarget target = SourceTarget::shells;
    Engine engine = Engine::room;
    float spacePercent = 0.0f;
    SyncMode syncMode = SyncMode::host;
    Division division = Division::quarter;
    float freeDecaySeconds = 1.2f;
    AdvancedControls advanced;
};

struct FactoryPreset
{
    const char* name;
    PluginState state;
};

const FactoryPreset* factoryPresets(std::size_t& count) noexcept;
const FactoryPreset* findFactoryPreset(std::string_view name) noexcept;

class StateCodec
{
public:
    static std::string encode(const PluginState& state);
    static bool decode(std::string_view serialized, PluginState& state, std::string& error);
};
}
