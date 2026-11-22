#pragma once

#include "ReverbEngines.h"

#include <optional>

namespace drmspace
{
enum class SyncMode { free, host, tap };
enum class Division { sixteenth, eighth, quarter, half, oneBar, twoBars };

struct TempoResult
{
    bool hasBpm = false;
    bool synchronized = false;
    bool usedFallback = false;
    bool gateClamped = false;
    bool decayClamped = false;
    double bpm = 0.0;
    double durationMilliseconds = 0.0;
    float decaySeconds = 1.0f;
    float gateSeconds = 1.0f;
};

const char* divisionName(Division division) noexcept;
double divisionQuarterNotes(Division division) noexcept;

// Converts host/tap/free timing into a division duration. Natural engines use
// duration as RT60; Gated uses it as a separate termination time.
class TempoSync
{
public:
    TempoResult calculate(SyncMode mode, double hostBpm, double tapBpm,
                          Division division, double freeDecaySeconds,
                          Engine engine) noexcept;
    void reset() noexcept;

private:
    double lastHostBpm = 120.0;
    double lastTapBpm = 120.0;
    bool hasHostBpm = false;
    bool hasTapBpm = false;
};

class TapTempo
{
public:
    std::optional<double> tap(double timeMilliseconds) noexcept;
    void reset() noexcept;
    std::optional<double> bpm() const noexcept;

private:
    double previousTapMilliseconds = 0.0;
    double averageIntervalMilliseconds = 0.0;
    bool hasPreviousTap = false;
    bool hasInterval = false;
};

class TempoSmoother
{
public:
    void prepare(double sampleRate, float rampMilliseconds = 25.0f) noexcept;
    void reset(double milliseconds) noexcept;
    void setTarget(double milliseconds) noexcept;
    double next() noexcept;
    double current() const noexcept { return currentMilliseconds; }

private:
    double currentMilliseconds = 500.0;
    double targetMilliseconds = 500.0;
    double sampleRate = 44100.0;
    float rampMilliseconds = 25.0f;
    int remainingSamples = 0;
};
}
