#pragma once

#include <array>
#include <vector>

namespace drmspace
{
enum class Engine { room, spring, gated };

struct ReverbParameters
{
    float decaySeconds = 1.2f;
    float preDelayMs = 12.0f;
    float damping = 0.45f;
    float width = 1.0f;
    float gateSeconds = 0.45f;
    float density = 0.65f;
    float wetLevel = 1.0f;
};

// Wet-only reverb engines. Call prepare() before processing; process() performs
// no allocation, locking, or I/O. Parameters are block-rate controls for now.
class ReverbEngines
{
public:
    void prepare(double sampleRate, int maximumBlockSize);
    void reset() noexcept;
    void setEngine(Engine engine) noexcept;
    void setParameters(const ReverbParameters& parameters) noexcept;
    void process(const float* const* input, float* const* output,
                 int channels, int samples) noexcept;

private:
    struct DelayLine
    {
        std::vector<float> data;
        int cursor = 0;
        void prepare(int size);
        void reset() noexcept;
        float read(int delay) const noexcept;
        void write(float value) noexcept;
    };
    struct Allpass
    {
        DelayLine delay;
        float process(float input, float coefficient) noexcept;
    };
    struct ChannelState
    {
        DelayLine predelay;
        DelayLine early;
        std::array<DelayLine, 4> tank;
        std::array<Allpass, 2> diffusion;
        std::array<float, 4> dampingState {};
        float gateGain = 0.0f;
        int gateRemaining = 0;
    };

    float processRoom(int channel, float input) noexcept;
    float processSpring(int channel, float input) noexcept;
    float processGated(int channel, float input) noexcept;
    float readPredelay(ChannelState& state, float input) noexcept;

    std::array<ChannelState, 2> state;
    Engine engine = Engine::room;
    ReverbParameters params;
    double sampleRate = 44100.0;
    int predelaySamples = 0;
    int gateSamples = 0;
    float feedback = 0.72f;
};
}
