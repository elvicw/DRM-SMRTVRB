#include "TempoSync.h"

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

bool near(double a, double b, double tolerance = 1.0e-6)
{
    return std::abs(a - b) <= tolerance;
}
}

int main()
{
    drmspace::TempoSync sync;
    constexpr std::array<double, 6> expectedMs { 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0 };
    constexpr std::array<drmspace::Division, 6> divisions {
        drmspace::Division::sixteenth, drmspace::Division::eighth, drmspace::Division::quarter,
        drmspace::Division::half, drmspace::Division::oneBar, drmspace::Division::twoBars
    };
    for (std::size_t i = 0; i < divisions.size(); ++i)
    {
        const auto result = sync.calculate(drmspace::SyncMode::host, 120.0, 0.0, divisions[i], 1.0,
                                           drmspace::Engine::room);
        require(result.hasBpm && result.synchronized && !result.usedFallback, "valid host tempo is followed");
        require(near(result.durationMilliseconds, expectedMs[i]), "musical division timing at 120 BPM");
        require(near(result.decaySeconds, expectedMs[i] / 1000.0), "natural-engine decay follows division");
    }
    require(std::string(drmspace::divisionName(drmspace::Division::oneBar)) == "1 bar", "division display name");

    sync.reset();
    const auto noHost = sync.calculate(drmspace::SyncMode::host, 0.0, 0.0, drmspace::Division::quarter,
                                       1.0, drmspace::Engine::room);
    require(noHost.usedFallback && near(noHost.bpm, 120.0) && near(noHost.durationMilliseconds, 500.0),
            "missing initial host tempo falls back to 120 BPM");
    const auto changedHost = sync.calculate(drmspace::SyncMode::host, 90.0, 0.0, drmspace::Division::quarter,
                                            1.0, drmspace::Engine::room);
    require(near(changedHost.durationMilliseconds, 2000.0 / 3.0), "host tempo changes update the duration");
    const auto lostHost = sync.calculate(drmspace::SyncMode::host, std::nan(""), 0.0,
                                         drmspace::Division::quarter, 1.0, drmspace::Engine::room);
    require(lostHost.usedFallback && near(lostHost.bpm, 90.0), "missing host tempo uses last valid tempo");

    const auto free = sync.calculate(drmspace::SyncMode::free, 150.0, 0.0, drmspace::Division::twoBars,
                                     1.37, drmspace::Engine::spring);
    require(!free.synchronized && !free.hasBpm && near(free.durationMilliseconds, 1370.0),
            "free timing ignores musical division");

    sync.reset();
    const auto noTap = sync.calculate(drmspace::SyncMode::tap, 0.0, 0.0, drmspace::Division::quarter,
                                      1.0, drmspace::Engine::spring);
    require(noTap.usedFallback && near(noTap.bpm, 120.0), "missing tap tempo falls back gracefully");
    const auto tap = sync.calculate(drmspace::SyncMode::tap, 130.0, 100.0, drmspace::Division::eighth,
                                    1.0, drmspace::Engine::spring);
    require(near(tap.bpm, 100.0) && near(tap.durationMilliseconds, 300.0), "tap tempo takes precedence in tap mode");
    const auto noTapUpdate = sync.calculate(drmspace::SyncMode::tap, 130.0, 0.0, drmspace::Division::eighth,
                                            1.0, drmspace::Engine::spring);
    require(noTapUpdate.usedFallback && near(noTapUpdate.bpm, 100.0), "invalid tap data holds the last tap tempo");

    sync.reset();
    const auto longRoom = sync.calculate(drmspace::SyncMode::host, 20.0, 0.0, drmspace::Division::twoBars,
                                         1.0, drmspace::Engine::room);
    require(near(longRoom.durationMilliseconds, 24000.0) && near(longRoom.decaySeconds, 8.0)
            && longRoom.decayClamped && !longRoom.gateClamped,
            "natural decay is clamped without treating RT60 as a hard tail end");
    const auto longGate = sync.calculate(drmspace::SyncMode::host, 20.0, 0.0, drmspace::Division::twoBars,
                                         1.0, drmspace::Engine::gated);
    require(near(longGate.gateSeconds, 4.0) && longGate.gateClamped,
            "gated termination reports its four-second engine limit");

    drmspace::TapTempo tapper;
    require(!tapper.tap(0.0).has_value(), "first tap starts a clock without a BPM");
    require(tapper.tap(500.0).has_value() && near(*tapper.bpm(), 120.0), "two half-second taps give 120 BPM");
    require(tapper.tap(1001.0).has_value() && near(*tapper.bpm(), 119.88, 0.1), "tap tempo averages small timing variation");
    const auto retained = tapper.tap(1200.0); // Too fast to be a valid beat; keep the previous estimate.
    require(retained.has_value() && near(*retained, *tapper.bpm()), "invalid rapid tap retains the previous estimate");
    require(!tapper.tap(-1.0).has_value() && !tapper.bpm().has_value(), "invalid clock resets tap tempo");

    drmspace::TempoSmoother smoother;
    smoother.prepare(48000.0, 25.0f);
    smoother.reset(500.0);
    smoother.setTarget(1000.0);
    const auto first = smoother.next();
    require(first > 500.0 && first < 501.0, "tempo change begins with a short ramp step");
    for (int i = 1; i < 1200; ++i) smoother.next();
    require(near(smoother.current(), 1000.0), "tempo smoother reaches its target after 25 ms");

    std::puts("DRM-SMRTVRB tempo sync tests passed");
    return 0;
}
