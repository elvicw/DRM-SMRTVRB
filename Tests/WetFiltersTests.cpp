#include "WetFilters.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace
{
void require(bool condition, const char* message)
{
    if (!condition) { std::fprintf(stderr, "FAILED: %s\n", message); std::abort(); }
}

constexpr double sampleRate = 48000.0;

// RMS gain of a steady sine after the filters have settled.
double gainAt(float highPassHz, float lowPassHz, double frequency)
{
    drmspace::WetFilters filters;
    filters.setCutoffs(highPassHz, lowPassHz);
    filters.prepare(sampleRate);
    constexpr int block = 32, blocks = 3000;
    std::vector<float> buffer(block);
    float* pointers[1] { buffer.data() };
    double inputSum = 0.0, outputSum = 0.0;
    long n = 0;
    for (int b = 0; b < blocks; ++b)
    {
        for (int i = 0; i < block; ++i, ++n)
            buffer[static_cast<std::size_t>(i)] = static_cast<float>(std::sin(2.0 * 3.14159265358979 * frequency * static_cast<double>(n) / sampleRate));
        std::vector<float> input = buffer;
        filters.process(pointers, 1, block);
        if (b >= blocks / 2)
            for (int i = 0; i < block; ++i)
            {
                inputSum += static_cast<double>(input[static_cast<std::size_t>(i)]) * input[static_cast<std::size_t>(i)];
                outputSum += static_cast<double>(buffer[static_cast<std::size_t>(i)]) * buffer[static_cast<std::size_t>(i)];
            }
    }
    return std::sqrt(outputSum / inputSum);
}
}

int main()
{
    using drmspace::WetFilters;
    require(std::abs(gainAt(WetFilters::highPassMinHz, WetFilters::lowPassMaxHz, 1000.0) - 1.0) < 1.0e-6,
            "both filters at their off edge are transparent");

    require(std::abs(gainAt(500.0f, WetFilters::lowPassMaxHz, 500.0) - 0.7071) < 0.02, "high-pass is -3 dB at cutoff");
    require(gainAt(500.0f, WetFilters::lowPassMaxHz, 50.0) < 0.02, "high-pass attenuates below cutoff");
    require(std::abs(gainAt(500.0f, WetFilters::lowPassMaxHz, 8000.0) - 1.0) < 0.02, "high-pass passes the high band");

    require(std::abs(gainAt(WetFilters::highPassMinHz, 4000.0f, 4000.0) - 0.7071) < 0.02, "low-pass is -3 dB at cutoff");
    require(gainAt(WetFilters::highPassMinHz, 4000.0f, 16000.0) < 0.1, "low-pass attenuates above cutoff");
    require(std::abs(gainAt(WetFilters::highPassMinHz, 4000.0f, 200.0) - 1.0) < 0.02, "low-pass passes the low band");

    // Sweeping the cutoffs across blocks must stay finite and bounded.
    drmspace::WetFilters filters;
    filters.prepare(sampleRate);
    std::vector<float> left(32), right(32);
    float* pointers[2] { left.data(), right.data() };
    for (int b = 0; b < 4000; ++b)
    {
        filters.setCutoffs(20.0f + 980.0f * static_cast<float>(b % 400) / 400.0f,
                           20000.0f - 19000.0f * static_cast<float>(b % 300) / 300.0f);
        for (std::size_t i = 0; i < 32; ++i) { left[i] = (i % 2 == 0) ? 1.0f : -1.0f; right[i] = left[i] * 0.5f; }
        filters.process(pointers, 2, 32);
        for (std::size_t i = 0; i < 32; ++i)
            require(std::isfinite(left[i]) && std::isfinite(right[i]) && std::abs(left[i]) < 4.0f, "swept filters stay finite and bounded");
    }
    std::puts("WetFilters tests passed");
    return 0;
}
