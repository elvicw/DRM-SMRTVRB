#include "WavFile.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <cstdint>

namespace
{
void require(bool condition, const char* message)
{
    if (!condition) { std::fprintf(stderr, "FAILED: %s\n", message); std::abort(); }
}

void put16(std::ostream& out, std::uint16_t v)
{
    const unsigned char b[] { static_cast<unsigned char>(v), static_cast<unsigned char>(v >> 8) };
    out.write(reinterpret_cast<const char*>(b), 2);
}

void put32(std::ostream& out, std::uint32_t v)
{
    const unsigned char b[] { static_cast<unsigned char>(v), static_cast<unsigned char>(v >> 8),
                              static_cast<unsigned char>(v >> 16), static_cast<unsigned char>(v >> 24) };
    out.write(reinterpret_cast<const char*>(b), 4);
}

void makePcm(const std::string& path, int bits, const float* samples, std::size_t count)
{
    const auto bytes = static_cast<std::uint32_t>(count * (bits / 8));
    std::ofstream out(path, std::ios::binary);
    out.write("RIFF", 4); put32(out, 36 + bytes); out.write("WAVE", 4);
    out.write("fmt ", 4); put32(out, 16); put16(out, 1); put16(out, 2);
    put32(out, 48000); put32(out, 48000 * 2 * (bits / 8));
    put16(out, static_cast<std::uint16_t>(2 * (bits / 8))); put16(out, static_cast<std::uint16_t>(bits));
    out.write("data", 4); put32(out, bytes);
    for (std::size_t i = 0; i < count; ++i)
    {
        const auto x = std::clamp(static_cast<double>(samples[i]), -1.0, 1.0);
        if (bits == 16) put16(out, static_cast<std::uint16_t>(static_cast<std::int16_t>(std::lround(x < 0 ? x * 32768 : x * 32767))));
        else if (bits == 24)
        {
            const auto value = static_cast<std::int32_t>(std::lround(x < 0 ? x * 8388608 : x * 8388607));
            const auto raw = static_cast<std::uint32_t>(value);
            const unsigned char b[] { static_cast<unsigned char>(raw), static_cast<unsigned char>(raw >> 8), static_cast<unsigned char>(raw >> 16) };
            out.write(reinterpret_cast<const char*>(b), 3);
        }
        else
        {
            const auto value = static_cast<std::int32_t>(std::llround(x <= -1.0 ? -2147483648.0 : x * 2147483647.0));
            put32(out, static_cast<std::uint32_t>(value));
        }
    }
}
}

int main()
{
    const auto path = (std::filesystem::temp_directory_path() / "drmsmrtvrb_wav_roundtrip.wav").string();
    drmspace::wav::Audio source;
    source.sampleRate = 44100;
    source.channels = 2;
    source.interleaved = { -1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 0.125f };
    std::string error;
    require(drmspace::wav::writeFloat32(path, source, error), "write float32 WAV");
    drmspace::wav::Audio decoded;
    require(drmspace::wav::read(path, decoded, error), "read float32 WAV");
    require(decoded.sampleRate == source.sampleRate, "preserve sample rate");
    require(decoded.channels == source.channels, "preserve channel count");
    require(decoded.interleaved.size() == source.interleaved.size(), "preserve sample count");
    for (std::size_t i = 0; i < source.interleaved.size(); ++i)
        require(std::abs(decoded.interleaved[i] - source.interleaved[i]) < 1.0e-7f, "preserve float samples");
    std::filesystem::remove(path);

    const float pcmSamples[] { -1.0f, -0.5f, 0.0f, 0.5f, 0.999f, -0.125f };
    for (const int bits : { 16, 24, 32 })
    {
        const auto pcmPath = (std::filesystem::temp_directory_path() / ("drmsmrtvrb_pcm" + std::to_string(bits) + ".wav")).string();
        makePcm(pcmPath, bits, pcmSamples, std::size(pcmSamples));
        require(drmspace::wav::read(pcmPath, decoded, error), "read PCM WAV");
        const double tolerance = bits == 16 ? 2.0 / 32768.0 : bits == 24 ? 2.0 / 8388608.0 : 2.0 / 2147483648.0;
        for (std::size_t i = 0; i < std::size(pcmSamples); ++i)
            if (std::abs(decoded.interleaved[i] - pcmSamples[i]) > tolerance * 1.1)
            {
                std::fprintf(stderr, "PCM%d sample %zu: got %.9f expected %.9f tolerance %.9g\n",
                             bits, i, decoded.interleaved[i], pcmSamples[i], tolerance);
                require(false, "decode PCM sample accurately");
            }
        std::filesystem::remove(pcmPath);
    }
    std::puts("DRM-SMRTVRB WAV tests passed");
    return 0;
}
