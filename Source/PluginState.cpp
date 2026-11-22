#include "PluginState.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <unordered_map>

namespace drmspace
{
namespace
{
constexpr AdvancedControls overrides(bool preDelay, bool tone, bool width, bool mix, bool tail,
                                     float predelayMs, float toneValue, float widthValue,
                                     float mixValue, float tailValue) noexcept
{
    return { preDelay, tone, width, mix, tail, predelayMs, toneValue, widthValue, mixValue, tailValue };
}

constexpr PluginState presetState(SourceTarget target, Engine engine, float space,
                                  SyncMode sync, Division division, float freeDecay,
                                  AdvancedControls advanced) noexcept
{
    return { target, engine, space, sync, division, freeDecay, advanced };
}

constexpr FactoryPreset presets[] {
    { "Tight Shells", presetState(SourceTarget::shells, Engine::room, 28.0f,
        SyncMode::free, Division::quarter, 0.48f, overrides(true, true, true, false, true, 5.0f, -0.08f, 0.82f, 1.0f, 0.48f)) },
    { "Big Snare", presetState(SourceTarget::snare, Engine::room, 76.0f,
        SyncMode::host, Division::half, 1.2f, overrides(false, true, true, false, true, 16.0f, 0.12f, 1.24f, 1.0f, 2.1f)) },
    { "Explosive Snare", presetState(SourceTarget::snare, Engine::gated, 86.0f,
        SyncMode::tap, Division::quarter, 0.8f, overrides(true, false, true, true, true, 3.0f, 0.0f, 1.2f, 0.92f, 0.32f)) },
    { "Small Studio", presetState(SourceTarget::room, Engine::room, 32.0f,
        SyncMode::free, Division::quarter, 0.62f, overrides(true, true, true, true, true, 0.0f, -0.12f, 0.78f, 0.42f, 0.62f)) },
    { "Large Room", presetState(SourceTarget::room, Engine::room, 74.0f,
        SyncMode::host, Division::oneBar, 2.0f, overrides(false, false, true, true, true, 4.0f, 0.0f, 1.08f, 0.8f, 2.0f)) },
    { "Wide Overheads", presetState(SourceTarget::overheads, Engine::room, 48.0f,
        SyncMode::free, Division::quarter, 0.85f, overrides(false, true, true, true, true, 5.0f, -0.2f, 1.32f, 0.72f, 0.85f)) },
    { "Metallic Spring", presetState(SourceTarget::snare, Engine::spring, 61.0f,
        SyncMode::free, Division::quarter, 1.6f, overrides(true, true, true, true, true, 18.0f, 0.24f, 1.14f, 0.84f, 1.6f)) },
    { "Vintage Gated", presetState(SourceTarget::snare, Engine::gated, 73.0f,
        SyncMode::tap, Division::quarter, 0.3f, overrides(true, false, true, true, true, 4.0f, 0.0f, 1.0f, 0.9f, 0.3f)) }
};

bool validState(const PluginState& s) noexcept
{
    const auto& a = s.advanced;
    return static_cast<std::size_t>(s.target) < 4 && static_cast<std::size_t>(s.engine) < 3
        && static_cast<std::size_t>(s.syncMode) < 3 && static_cast<std::size_t>(s.division) < 6
        && std::isfinite(s.spacePercent) && s.spacePercent >= 0.0f && s.spacePercent <= 100.0f
        && std::isfinite(s.freeDecaySeconds) && s.freeDecaySeconds >= 0.08f && s.freeDecaySeconds <= 8.0f
        && std::isfinite(a.preDelayMs) && a.preDelayMs >= 0.0f && a.preDelayMs <= 200.0f
        && std::isfinite(a.tone) && a.tone >= -1.0f && a.tone <= 1.0f
        && std::isfinite(a.width) && a.width >= 0.0f && a.width <= 2.0f
        && std::isfinite(a.mix) && a.mix >= 0.0f && a.mix <= 1.0f
        && std::isfinite(a.tailSeconds) && a.tailSeconds >= 0.08f && a.tailSeconds <= 8.0f;
}

const std::string* field(const std::unordered_map<std::string, std::string>& values, const char* key)
{
    const auto it = values.find(key);
    return it == values.end() ? nullptr : &it->second;
}

bool parseFloatField(const std::unordered_map<std::string, std::string>& values,
                     const char* key, float& target, std::string& error)
{
    const auto* value = field(values, key);
    if (value == nullptr) return true;
    char* end = nullptr;
    const float parsed = std::strtof(value->c_str(), &end);
    if (end == value->c_str() || *end != '\0' || !std::isfinite(parsed))
    { error = std::string("Invalid number for ") + key; return false; }
    target = parsed;
    return true;
}

bool parseBoolField(const std::unordered_map<std::string, std::string>& values,
                    const char* key, bool& target, std::string& error)
{
    const auto* value = field(values, key);
    if (value == nullptr) return true;
    if (*value == "1" || *value == "true") target = true;
    else if (*value == "0" || *value == "false") target = false;
    else { error = std::string("Invalid boolean for ") + key; return false; }
    return true;
}

template <typename Enum, std::size_t N>
bool parseEnumField(const std::unordered_map<std::string, std::string>& values, const char* key,
                    Enum& target, const std::array<std::pair<const char*, Enum>, N>& names,
                    std::string& error)
{
    const auto* value = field(values, key);
    if (value == nullptr) return true;
    for (const auto& name : names)
        if (*value == name.first) { target = name.second; return true; }
    error = std::string("Invalid value for ") + key;
    return false;
}

constexpr std::array<std::pair<const char*, SourceTarget>, 4> targetNames {{
    { "shells", SourceTarget::shells }, { "snare", SourceTarget::snare },
    { "room", SourceTarget::room }, { "overheads", SourceTarget::overheads }
}};
constexpr std::array<std::pair<const char*, Engine>, 3> engineNames {{
    { "room", Engine::room }, { "spring", Engine::spring }, { "gated", Engine::gated }
}};
constexpr std::array<std::pair<const char*, SyncMode>, 3> syncNames {{
    { "free", SyncMode::free }, { "host", SyncMode::host }, { "tap", SyncMode::tap }
}};
constexpr std::array<std::pair<const char*, Division>, 6> divisionNames {{
    { "sixteenth", Division::sixteenth }, { "eighth", Division::eighth },
    { "quarter", Division::quarter }, { "half", Division::half },
    { "oneBar", Division::oneBar }, { "twoBars", Division::twoBars }
}};
}

AdvancedResult applyAdvancedControls(const SpaceMacroResult& macro,
                                     const AdvancedControls& controls) noexcept
{
    AdvancedResult result { macro.reverb, macro.dryGain, macro.wetGain };
    if (controls.preDelayEnabled)
        result.reverb.preDelayMs = std::clamp(std::isfinite(controls.preDelayMs) ? controls.preDelayMs : macro.reverb.preDelayMs,
                                             0.0f, 200.0f);
    if (controls.toneEnabled)
        result.reverb.damping = std::clamp(macro.reverb.damping
            - (std::isfinite(controls.tone) ? controls.tone : 0.0f) * 0.25f, 0.0f, 1.0f);
    if (controls.widthEnabled)
        result.reverb.width = std::clamp(std::isfinite(controls.width) ? controls.width : macro.reverb.width,
                                        0.0f, 2.0f);
    if (controls.tailEnabled)
    {
        const auto tail = std::clamp(std::isfinite(controls.tailSeconds) ? controls.tailSeconds
                                                                         : macro.reverb.decaySeconds,
                                     0.08f, 8.0f);
        result.reverb.decaySeconds = tail;
        result.reverb.gateSeconds = std::clamp(tail, 0.03f, 4.0f);
    }
    if (controls.mixEnabled)
    {
        constexpr float halfPi = 1.57079632679489661923f;
        const float normalizedSpace = std::clamp(macro.normalizedSpace, 0.0f, 1.0f);
        const float mix = std::clamp(std::isfinite(controls.mix) ? controls.mix : 1.0f, 0.0f, 1.0f);
        result.wetGain = std::sin(halfPi * normalizedSpace) * mix;
        result.dryGain = std::sqrt(std::max(0.0f, 1.0f - result.wetGain * result.wetGain));
    }
    return result;
}

const FactoryPreset* factoryPresets(std::size_t& count) noexcept
{
    count = sizeof(presets) / sizeof(presets[0]);
    return presets;
}

const FactoryPreset* findFactoryPreset(std::string_view name) noexcept
{
    std::size_t count = 0;
    const auto* list = factoryPresets(count);
    for (std::size_t i = 0; i < count; ++i)
        if (name == list[i].name) return &list[i];
    return nullptr;
}

std::string StateCodec::encode(const PluginState& state)
{
    if (!validState(state)) return {};
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out << std::setprecision(std::numeric_limits<float>::max_digits10);
    const auto& a = state.advanced;
    out << "DRM-SMRTVRB_STATE=1\n"
        << "target=" << targetNames[static_cast<std::size_t>(state.target)].first << '\n'
        << "engine=" << engineNames[static_cast<std::size_t>(state.engine)].first << '\n'
        << "space=" << state.spacePercent << '\n'
        << "sync=" << syncNames[static_cast<std::size_t>(state.syncMode)].first << '\n'
        << "division=" << divisionNames[static_cast<std::size_t>(state.division)].first << '\n'
        << "freeDecay=" << state.freeDecaySeconds << '\n'
        << "advanced.preDelayEnabled=" << a.preDelayEnabled << '\n'
        << "advanced.toneEnabled=" << a.toneEnabled << '\n'
        << "advanced.widthEnabled=" << a.widthEnabled << '\n'
        << "advanced.mixEnabled=" << a.mixEnabled << '\n'
        << "advanced.tailEnabled=" << a.tailEnabled << '\n'
        << "advanced.preDelayMs=" << a.preDelayMs << '\n'
        << "advanced.tone=" << a.tone << '\n'
        << "advanced.width=" << a.width << '\n'
        << "advanced.mix=" << a.mix << '\n'
        << "advanced.tailSeconds=" << a.tailSeconds << '\n';
    return out.str();
}

bool StateCodec::decode(std::string_view serialized, PluginState& state, std::string& error)
{
    const auto newline = serialized.find('\n');
    if (newline == std::string_view::npos || serialized.substr(0, newline) != "DRM-SMRTVRB_STATE=1")
    { error = "Unsupported or malformed DRM-SMRTVRB state header"; return false; }

    std::unordered_map<std::string, std::string> values;
    std::size_t start = newline + 1;
    while (start < serialized.size())
    {
        const auto end = serialized.find('\n', start);
        auto line = serialized.substr(start, end == std::string_view::npos ? serialized.size() - start : end - start);
        if (!line.empty() && line.back() == '\r') line.remove_suffix(1);
        const auto equal = line.find('=');
        if (equal != std::string_view::npos && equal > 0)
            values[std::string(line.substr(0, equal))] = std::string(line.substr(equal + 1));
        if (end == std::string_view::npos) break;
        start = end + 1;
    }

    PluginState candidate;
    if (!parseEnumField(values, "target", candidate.target, targetNames, error)
        || !parseEnumField(values, "engine", candidate.engine, engineNames, error)
        || !parseEnumField(values, "sync", candidate.syncMode, syncNames, error)
        || !parseEnumField(values, "division", candidate.division, divisionNames, error)
        || !parseFloatField(values, "space", candidate.spacePercent, error)
        || !parseFloatField(values, "freeDecay", candidate.freeDecaySeconds, error)
        || !parseBoolField(values, "advanced.preDelayEnabled", candidate.advanced.preDelayEnabled, error)
        || !parseBoolField(values, "advanced.toneEnabled", candidate.advanced.toneEnabled, error)
        || !parseBoolField(values, "advanced.widthEnabled", candidate.advanced.widthEnabled, error)
        || !parseBoolField(values, "advanced.mixEnabled", candidate.advanced.mixEnabled, error)
        || !parseBoolField(values, "advanced.tailEnabled", candidate.advanced.tailEnabled, error)
        || !parseFloatField(values, "advanced.preDelayMs", candidate.advanced.preDelayMs, error)
        || !parseFloatField(values, "advanced.tone", candidate.advanced.tone, error)
        || !parseFloatField(values, "advanced.width", candidate.advanced.width, error)
        || !parseFloatField(values, "advanced.mix", candidate.advanced.mix, error)
        || !parseFloatField(values, "advanced.tailSeconds", candidate.advanced.tailSeconds, error))
        return false;
    if (!validState(candidate))
    { error = "State contains out-of-range parameters"; return false; }
    state = candidate;
    error.clear();
    return true;
}
}
