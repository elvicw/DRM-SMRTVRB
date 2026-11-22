#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace drmspace::wav
{
struct Audio
{
    std::uint32_t sampleRate = 0;
    std::uint16_t channels = 0;
    std::vector<float> interleaved;

    std::size_t frames() const noexcept
    {
        return channels == 0 ? 0 : interleaved.size() / channels;
    }
};

// Reads PCM 16/24/32-bit and IEEE float32 RIFF/WAVE, mono or stereo.
bool read(const std::string& path, Audio& audio, std::string& error);
// Writes IEEE float32 RIFF/WAVE.
bool writeFloat32(const std::string& path, const Audio& audio, std::string& error);
}
