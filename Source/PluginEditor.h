#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class DRMsmrtvrbAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                               private juce::Timer
{
public:
    explicit DRMsmrtvrbAudioProcessorEditor(DRMsmrtvrbAudioProcessor& pluginProcessor);
    ~DRMsmrtvrbAudioProcessorEditor() override = default;
    void paint(juce::Graphics& graphics) override;
    void resized() override;

private:
    void timerCallback() override;
    void styleSlider(juce::Slider& slider, bool rotary);
    void addLabel(juce::Label& label, const juce::String& text);
    void selectPreset();

    DRMsmrtvrbAudioProcessor& pluginProcessor;
    juce::LookAndFeel_V4 lookAndFeel;
    juce::ComboBox preset, target, character, sync, division;
    juce::TextButton tapButton { "TAP" };
    juce::Slider space, bpm, freeDecay, preDelay, tone, width, mix, tail;
    juce::ToggleButton preDelayOn { "Pre-delay" }, toneOn { "Tone" }, widthOn { "Width" },
                       mixOn { "Mix" }, tailOn { "Tail" };
    juce::Label title, brand, spaceLabel, bpmLabel, syncLabel, advancedLabel;
    juce::Label targetLabel, characterLabel, presetLabel, presetStatus, divisionLabel;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, 4> comboAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 7> sliderAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tailAttachment;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 5> buttonAttachments;
    std::array<float, 4> meterLevels {};
    std::array<float, 4> meterPeakMarkers {};
    std::array<int, 4> meterPeakHoldTicks {};
    std::array<float, DRMsmrtvrbAudioProcessor::activityHistorySize> activitySnapshot {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DRMsmrtvrbAudioProcessorEditor)
};
