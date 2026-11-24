#pragma once

#include <array>

namespace drmspace
{
// 12 dB/oct Butterworth high-pass and low-pass for the wet signal. A filter sitting at its
// "off" edge is bypassed exactly. Cutoff changes glide in the log-frequency domain.
// process() performs no allocation, locking, or I/O.
class WetFilters
{
public:
    static constexpr float highPassMinHz = 20.0f;
    static constexpr float highPassMaxHz = 1000.0f;
    static constexpr float lowPassMinHz = 1000.0f;
    static constexpr float lowPassMaxHz = 20000.0f;

    void prepare(double sampleRate) noexcept;
    void reset() noexcept;
    void setCutoffs(float highPassHz, float lowPassHz) noexcept;
    void process(float* const* data, int channels, int samples) noexcept;

private:
    struct Biquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
        std::array<float, 2> z1 {}, z2 {};
        bool active = false;
        void clear() noexcept { z1.fill(0.0f); z2.fill(0.0f); }
        void run(float* data, int channel, int samples) noexcept;
    };

    void updateCoefficients(Biquad& filter, float hz, bool highPass) const noexcept;
    float glide(float current, float target, int samples) const noexcept;

    double sampleRate = 44100.0;
    float highPassTarget = highPassMinHz, lowPassTarget = lowPassMaxHz;
    float highPassCurrent = highPassMinHz, lowPassCurrent = lowPassMaxHz;
    Biquad highPass, lowPass;
};
}
