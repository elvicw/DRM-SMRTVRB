#include "TempoSync.h"

#include <algorithm>
#include <cmath>

namespace drmspace
{
namespace
{
bool validBpm(double bpm) noexcept { return std::isfinite(bpm) && bpm >= 20.0 && bpm <= 300.0; }
}

const char* divisionName(Division division) noexcept
{
    switch (division)
    {
        case Division::sixteenth: return "1/16";
        case Division::eighth: return "1/8";
        case Division::quarter: return "1/4";
        case Division::half: return "1/2";
        case Division::oneBar: return "1 bar";
        case Division::twoBars: return "2 bars";
    }
    return "1/4";
}

double divisionQuarterNotes(Division division) noexcept
{
    switch (division)
    {
        case Division::sixteenth: return 0.25;
        case Division::eighth: return 0.5;
        case Division::quarter: return 1.0;
        case Division::half: return 2.0;
        case Division::oneBar: return 4.0;
        case Division::twoBars: return 8.0;
    }
    return 1.0;
}

void TempoSync::reset() noexcept
{
    lastHostBpm = 120.0;
    lastTapBpm = 120.0;
    hasHostBpm = false;
    hasTapBpm = false;
}

TempoResult TempoSync::calculate(SyncMode mode, double hostBpm, double tappedBpm,
                                Division division, double freeSeconds, Engine engine) noexcept
{
    TempoResult result;
    if (mode == SyncMode::free)
    {
        const auto duration = std::clamp(std::isfinite(freeSeconds) ? freeSeconds : 1.0, 0.08, 8.0);
        result.decaySeconds = static_cast<float>(duration);
        result.gateSeconds = static_cast<float>(std::clamp(duration, 0.03, 4.0));
        result.durationMilliseconds = duration * 1000.0;
        result.gateClamped = engine == Engine::gated && duration > 4.0;
        result.decayClamped = freeSeconds != duration;
        return result;
    }

    if (mode == SyncMode::host)
    {
        if (validBpm(hostBpm))
        {
            lastHostBpm = hostBpm;
            hasHostBpm = true;
            result.bpm = hostBpm;
        }
        else
        {
            result.bpm = hasHostBpm ? lastHostBpm : 120.0;
            result.usedFallback = true;
        }
    }
    else
    {
        if (validBpm(tappedBpm))
        {
            lastTapBpm = tappedBpm;
            hasTapBpm = true;
            result.bpm = tappedBpm;
        }
        else
        {
            result.bpm = hasTapBpm ? lastTapBpm : 120.0;
            result.usedFallback = true;
        }
    }

    result.hasBpm = true;
    result.synchronized = true;
    result.durationMilliseconds = 60000.0 / result.bpm * divisionQuarterNotes(division);
    const double durationSeconds = result.durationMilliseconds * 0.001;
    result.decaySeconds = static_cast<float>(std::clamp(durationSeconds, 0.08, 8.0));
    result.gateSeconds = static_cast<float>(std::clamp(durationSeconds, 0.03, 4.0));
    result.gateClamped = engine == Engine::gated && durationSeconds > 4.0;
    result.decayClamped = durationSeconds < 0.08 || durationSeconds > 8.0;
    return result;
}

void TapTempo::reset() noexcept
{
    previousTapMilliseconds = 0.0;
    averageIntervalMilliseconds = 0.0;
    hasPreviousTap = false;
    hasInterval = false;
}

std::optional<double> TapTempo::bpm() const noexcept
{
    if (!hasInterval || averageIntervalMilliseconds <= 0.0) return std::nullopt;
    return 60000.0 / averageIntervalMilliseconds;
}

std::optional<double> TapTempo::tap(double timeMilliseconds) noexcept
{
    if (!std::isfinite(timeMilliseconds) || timeMilliseconds < 0.0)
    {
        reset();
        return std::nullopt;
    }
    if (!hasPreviousTap)
    {
        previousTapMilliseconds = timeMilliseconds;
        hasPreviousTap = true;
        return bpm();
    }

    const double interval = timeMilliseconds - previousTapMilliseconds;
    previousTapMilliseconds = timeMilliseconds;
    if (interval < 200.0 || interval > 2000.0)
    {
        return bpm();
    }
    if (hasInterval && (interval > averageIntervalMilliseconds * 2.0 || interval < averageIntervalMilliseconds * 0.5))
    {
        averageIntervalMilliseconds = interval;
        return bpm();
    }
    averageIntervalMilliseconds = hasInterval
        ? averageIntervalMilliseconds * 0.75 + interval * 0.25 : interval;
    hasInterval = true;
    return bpm();
}

void TempoSmoother::prepare(double rate, float rampMs) noexcept
{
    sampleRate = std::clamp(rate, 8000.0, 384000.0);
    rampMilliseconds = std::clamp(rampMs, 1.0f, 100.0f);
    remainingSamples = 0;
}

void TempoSmoother::reset(double milliseconds) noexcept
{
    currentMilliseconds = targetMilliseconds = std::clamp(milliseconds, 20.0, 30000.0);
    remainingSamples = 0;
}

void TempoSmoother::setTarget(double milliseconds) noexcept
{
    targetMilliseconds = std::clamp(std::isfinite(milliseconds) ? milliseconds : currentMilliseconds, 20.0, 30000.0);
    remainingSamples = std::max(1, static_cast<int>(sampleRate * rampMilliseconds * 0.001));
}

double TempoSmoother::next() noexcept
{
    if (remainingSamples <= 0) return currentMilliseconds;
    currentMilliseconds += (targetMilliseconds - currentMilliseconds) / remainingSamples;
    if (--remainingSamples == 0) currentMilliseconds = targetMilliseconds;
    return currentMilliseconds;
}
}
