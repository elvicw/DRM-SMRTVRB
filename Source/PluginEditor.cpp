#include "PluginEditor.h"

namespace
{
const juce::Colour face { 0xff202225 };
const juce::Colour panel { 0xff292c2f };
const juce::Colour display { 0xff101214 };
const juce::Colour text { 0xffe1ddcf };
const juce::Colour muted { 0xff9b9a91 };
const juce::Colour amber { 0xffd69a38 };
const juce::Colour steel { 0xff62676a };
}

DRMsmrtvrbAudioProcessorEditor::DRMsmrtvrbAudioProcessorEditor(DRMsmrtvrbAudioProcessor& p)
    : AudioProcessorEditor(&p), pluginProcessor(p)
{
    setLookAndFeel(&lookAndFeel);
    setResizable(true, true);
    setResizeLimits(640, 420, 1040, 700);

    auto configureCombo = [this](juce::ComboBox& combo)
    {
        combo.setColour(juce::ComboBox::backgroundColourId, display);
        combo.setColour(juce::ComboBox::textColourId, text);
        combo.setColour(juce::ComboBox::outlineColourId, steel);
        addAndMakeVisible(combo);
    };
    configureCombo(preset);
    configureCombo(target);
    configureCombo(character);
    configureCombo(sync);
    configureCombo(division);
    preset.setTitle("Factory Preset");
    target.setTitle("Source");
    character.setTitle("Character");
    sync.setTitle("Tempo Sync");
    division.setTitle("Tempo Division");

    target.addItemList({ "Shells", "Snare", "Room", "Overheads" }, 1);
    character.addItemList({ "Room", "Spring", "Gated" }, 1);
    sync.addItemList({ "Free", "Host", "Tap" }, 1);
    division.addItemList({ "1/16", "1/8", "1/4", "1/2", "1 bar", "2 bars" }, 1);

    std::size_t presetCount = 0;
    const auto* presets = drmspace::factoryPresets(presetCount);
    preset.addItem("Factory Preset", 1);
    for (std::size_t i = 0; i < presetCount; ++i)
        preset.addItem(presets[i].name, static_cast<int>(i) + 2);
    preset.onChange = [this] { selectPreset(); };

    styleSlider(space, true);
    styleSlider(bpm, false);
    styleSlider(freeDecay, false);
    styleSlider(preDelay, false);
    styleSlider(tone, false);
    styleSlider(width, false);
    styleSlider(mix, false);
    styleSlider(tail, false);
    space.setTitle("SPACE amount");
    bpm.setTitle("Tap BPM");
    freeDecay.setTitle("Free Decay");
    preDelay.setTitle("Pre-delay");
    tone.setTitle("Tone");
    width.setTitle("Width");
    mix.setTitle("Mix");
    tail.setTitle("Tail");
    space.setTextValueSuffix(" %");
    bpm.setTextValueSuffix(" BPM");
    freeDecay.setTextValueSuffix(" s");
    preDelay.setTextValueSuffix(" ms");
    tone.setTextValueSuffix(" ");
    width.setTextValueSuffix(" ");
    mix.setTextValueSuffix(" %");
    tail.setTextValueSuffix(" s");

    std::array<juce::ToggleButton*, 5> toggles { &preDelayOn, &toneOn, &widthOn, &mixOn, &tailOn };
    for (std::size_t i = 0; i < toggles.size(); ++i)
    {
        toggles[i]->setColour(juce::ToggleButton::textColourId, text);
        toggles[i]->setColour(juce::ToggleButton::tickColourId, amber);
        addAndMakeVisible(*toggles[i]);
    }
    tapButton.setColour(juce::TextButton::buttonColourId, panel);
    tapButton.setColour(juce::TextButton::textColourOffId, amber);
    tapButton.onClick = [this] { pluginProcessor.tapTempoNow(); };
    addAndMakeVisible(tapButton);

    addLabel(title, "DRM-SMRTVRB");
    title.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, text);
    addLabel(brand, "yeahwaah audio  /  DRUM REVERB");
    addLabel(presetLabel, "PRESET");
    addLabel(presetStatus, "");
    addLabel(targetLabel, "SOURCE");
    addLabel(characterLabel, "CHARACTER");
    addLabel(spaceLabel, "SPACE");
    addLabel(syncLabel, "TEMPO SYNC");
    addLabel(bpmLabel, "BPM");
    addLabel(divisionLabel, "DIVISION");
    addLabel(advancedLabel, "ADVANCED PROFILE TRIMS");
    advancedLabel.setColour(juce::Label::textColourId, amber);

    auto& state = pluginProcessor.parameters;
    comboAttachments[0] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "target", target);
    comboAttachments[1] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "engine", character);
    comboAttachments[2] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "sync", sync);
    comboAttachments[3] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "division", division);
    sliderAttachments[0] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "space", space);
    sliderAttachments[1] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "bpm", bpm);
    sliderAttachments[2] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "freeDecay", freeDecay);
    sliderAttachments[3] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advPreDelay", preDelay);
    sliderAttachments[4] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advTone", tone);
    sliderAttachments[5] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advWidth", width);
    sliderAttachments[6] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advMix", mix);
    // Tail uses the same APVTS attachment type and is retained separately to keep the fixed arrays small.
    buttonAttachments[0] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advPreDelayOn", preDelayOn);
    buttonAttachments[1] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advToneOn", toneOn);
    buttonAttachments[2] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advWidthOn", widthOn);
    buttonAttachments[3] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advMixOn", mixOn);
    buttonAttachments[4] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advTailOn", tailOn);
    tailAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advTail", tail);

    setSize(760, 500);
    startTimerHz(24);
}

void DRMsmrtvrbAudioProcessorEditor::styleSlider(juce::Slider& slider, bool rotary)
{
    slider.setWantsKeyboardFocus(true);
    slider.setSliderStyle(rotary ? juce::Slider::RotaryHorizontalVerticalDrag : juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(rotary ? juce::Slider::TextBoxBelow : juce::Slider::TextBoxRight, false,
                           rotary ? 82 : 70, rotary ? 20 : 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, amber);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, steel);
    slider.setColour(juce::Slider::thumbColourId, amber);
    slider.setColour(juce::Slider::trackColourId, steel);
    slider.setColour(juce::Slider::backgroundColourId, display);
    slider.setColour(juce::Slider::textBoxTextColourId, text);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, display);
    slider.setColour(juce::Slider::textBoxOutlineColourId, steel);
    slider.setDoubleClickReturnValue(true, slider.getValue());
    addAndMakeVisible(slider);
}

void DRMsmrtvrbAudioProcessorEditor::addLabel(juce::Label& label, const juce::String& value)
{
    label.setText(value, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, muted);
    label.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(label);
}

void DRMsmrtvrbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(face);
    auto area = getLocalBounds().toFloat().reduced(12.0f);
    g.setColour(panel);
    g.fillRoundedRectangle(area, 7.0f);
    g.setColour(steel);
    g.drawRoundedRectangle(area, 7.0f, 1.0f);

    const float meterTop = 144.0f;
    const float meterHeight = 174.0f;
    const float meterStart = static_cast<float>(getWidth() - 112);
    for (int i = 0; i < 4; ++i)
    {
        const float x = meterStart + static_cast<float>(i) * 18.0f;
        g.setColour(display);
        const juce::Rectangle<float> meter { x, meterTop, 12.0f, meterHeight };
        g.fillRoundedRectangle(meter, 2.0f);
        const auto level = juce::jlimit(0.0f, 1.0f, meterLevels[static_cast<std::size_t>(i)]);
        const auto fill = juce::Rectangle<float>(x, meter.getBottom() - meterHeight * level,
                                                 12.0f, meterHeight * level);
        g.setColour(i < 2 ? juce::Colour(0xff71836b) : amber);
        g.fillRoundedRectangle(fill, 2.0f);
        const float markerY = meter.getBottom() - meterHeight * meterPeakMarkers[static_cast<std::size_t>(i)];
        g.setColour(text);
        g.drawLine(x - 1.0f, markerY, x + 13.0f, markerY, 1.5f);
    }
    g.setColour(muted);
    g.setFont(9.0f);
    g.drawFittedText("INPUT", getWidth() - 112, 124, 30, 14, juce::Justification::centred, 1);
    g.drawFittedText("OUTPUT", getWidth() - 76, 124, 42, 14, juce::Justification::centred, 1);
    g.drawText("L", static_cast<int>(meterStart), 321, 12, 12, juce::Justification::centred);
    g.drawText("R", static_cast<int>(meterStart + 18.0f), 321, 12, 12, juce::Justification::centred);
    g.drawText("L", static_cast<int>(meterStart + 36.0f), 321, 12, 12, juce::Justification::centred);
    g.drawText("R", static_cast<int>(meterStart + 54.0f), 321, 12, 12, juce::Justification::centred);

    const juce::Rectangle<float> activityArea { static_cast<float>(getWidth() - 126), 350.0f, 92.0f, 34.0f };
    g.setColour(display);
    g.fillRoundedRectangle(activityArea, 2.0f);
    g.setColour(amber.withAlpha(0.85f));
    const float barWidth = activityArea.getWidth() / static_cast<float>(activitySnapshot.size());
    for (std::size_t i = 0; i < activitySnapshot.size(); ++i)
    {
        const float height = activityArea.getHeight() * juce::jlimit(0.0f, 1.0f, activitySnapshot[i]);
        g.fillRect(activityArea.getX() + static_cast<float>(i) * barWidth,
                   activityArea.getBottom() - height, juce::jmax(1.0f, barWidth - 0.5f), height);
    }
    g.setColour(muted);
    g.drawText("WET ENVELOPE", getWidth() - 126, 386, 92, 13, juce::Justification::centred);
}

void DRMsmrtvrbAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(24);
    title.setBounds(32, 22, 260, 30);
    brand.setBounds(32, 51, 260, 18);
    presetLabel.setBounds(322, 20, 68, 18);
    presetStatus.setBounds(getWidth() - 194, 20, 142, 18);
    presetStatus.setJustificationType(juce::Justification::centredRight);
    preset.setBounds(322, 39, juce::jmax(190, getWidth() - 354), 28);
    targetLabel.setBounds(32, 84, 100, 18);
    target.setBounds(32, 103, 150, 30);
    characterLabel.setBounds(198, 84, 130, 18);
    character.setBounds(198, 103, 150, 30);
    const int knob = juce::jmin(190, getHeight() - 292);
    const int spaceX = juce::jmax(356, 420 - knob / 2);
    spaceLabel.setBounds(spaceX + knob / 2 - 60, 82, 120, 20);
    space.setBounds(spaceX, 98, knob, knob);

    syncLabel.setBounds(32, 158, 120, 18);
    sync.setBounds(32, 178, 116, 28);
    bpmLabel.setBounds(162, 158, 70, 18);
    bpm.setBounds(162, 178, 116, 28);
    tapButton.setBounds(32, 214, 74, 27);
    divisionLabel.setBounds(122, 214, 78, 18);
    division.setBounds(122, 232, 156, 28);
    freeDecay.setBounds(32, 268, 246, 26);

    advancedLabel.setBounds(32, 318, 300, 18);
    const int rowY = 344;
    const int colW = juce::jmax(82, (getWidth() - 184) / 5);
    std::array<juce::ToggleButton*, 5> toggles { &preDelayOn, &toneOn, &widthOn, &mixOn, &tailOn };
    std::array<juce::Slider*, 5> sliders { &preDelay, &tone, &width, &mix, &tail };
    for (int i = 0; i < 5; ++i)
    {
        const int x = 32 + i * colW;
        toggles[static_cast<std::size_t>(i)]->setBounds(x, rowY, colW - 8, 22);
        sliders[static_cast<std::size_t>(i)]->setBounds(x, rowY + 23, colW - 4, 35);
    }
    juce::ignoreUnused(bounds);
}

void DRMsmrtvrbAudioProcessorEditor::timerCallback()
{
    const auto syncMode = static_cast<int>(std::lround(pluginProcessor.parameters.getRawParameterValue("sync")->load()));
    bpm.setEnabled(syncMode == 2);
    tapButton.setEnabled(syncMode == 2);
    division.setEnabled(syncMode != 0);
    freeDecay.setEnabled(syncMode == 0);
    if (syncMode != 2)
        bpm.setValue(syncMode == 1 ? pluginProcessor.displayedBpm() : 120.0,
                     juce::dontSendNotification);
    bpmLabel.setText(syncMode == 2 ? "TAP BPM"
        : syncMode == 1 ? (pluginProcessor.hostTempoAvailable() ? "HOST BPM" : "FALLBACK BPM")
                        : "BPM",
        juce::dontSendNotification);

    preDelay.setEnabled(preDelayOn.getToggleState());
    tone.setEnabled(toneOn.getToggleState());
    width.setEnabled(widthOn.getToggleState());
    mix.setEnabled(mixOn.getToggleState());
    tailOn.setEnabled(syncMode == 0);
    tail.setEnabled(syncMode == 0 && tailOn.getToggleState());
    tailOn.setTooltip(syncMode == 0 ? "Override the profile tail length" : "Tail timing follows the selected tempo division");
    tail.setTooltip(syncMode == 0 ? "Tail length in seconds" : "Tail timing follows the selected tempo division");
    const auto currentProgram = pluginProcessor.presetIndexForDisplay();
    if (currentProgram >= 0)
        preset.setSelectedId(currentProgram + 2, juce::dontSendNotification);
    else
        preset.setSelectedId(1, juce::dontSendNotification);
    const bool modified = pluginProcessor.isCurrentPresetModified();
    presetStatus.setText(modified ? "MODIFIED" : "FACTORY", juce::dontSendNotification);
    presetStatus.setColour(juce::Label::textColourId, modified ? amber : muted);

    for (int channel = 0; channel < 2; ++channel)
    {
        for (int direction = 0; direction < 2; ++direction)
        {
            const auto meterIndex = static_cast<std::size_t>(direction * 2 + channel);
            const auto current = direction == 0 ? pluginProcessor.inputPeak(channel) : pluginProcessor.outputPeak(channel);
            auto& level = meterLevels[meterIndex];
            auto& marker = meterPeakMarkers[meterIndex];
            auto& hold = meterPeakHoldTicks[meterIndex];
            level = juce::jmax(current, level * 0.90f);
            if (current >= marker)
            {
                marker = current;
                hold = 18;
            }
            else if (hold > 0)
                --hold;
            else
                marker *= 0.96f;
        }
    }
    pluginProcessor.copyActivityHistory(activitySnapshot);
    repaint();
}

void DRMsmrtvrbAudioProcessorEditor::selectPreset()
{
    const int selection = preset.getSelectedId();
    if (selection >= 2) pluginProcessor.loadFactoryPreset(selection - 2);
}
