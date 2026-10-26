#include "PluginEditor.h"

JunoCloneAudioProcessorEditor::JunoCloneAudioProcessorEditor (JunoCloneAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);
    addAndMakeVisible (presetBar);
    addAndMakeVisible (voiceLedBar);

    auto& lfo1Sec = addSection ("LFO 1", 0);
    addCombo (lfo1Sec, "lfo1Wave", "Wave");
    addKnob  (lfo1Sec, "lfo1Rate", "Rate");

    auto& lfo2Sec = addSection ("LFO 2", 0);
    addCombo (lfo2Sec, "lfo2Wave", "Wave");
    addKnob  (lfo2Sec, "lfo2Rate", "Rate");

    auto& lfo3Sec = addSection ("LFO 3 (Vibrato)", 0);
    addKnob (lfo3Sec, "lfo3Rate", "Rate");
    addKnob (lfo3Sec, "lfo3ModWheelAmount", "ModWheel");

    auto& osc1Sec = addSection ("OSC 1", 0);
    addCombo (osc1Sec, "osc1Wave", "Wave");
    addKnob  (osc1Sec, "osc1Tune", "Tune");
    addKnob  (osc1Sec, "osc1Lfo1Mod", "LFO1 Mod");
    addKnob  (osc1Sec, "osc1Lfo2Mod", "LFO2 Mod");
    addKnob  (osc1Sec, "osc1Eg1Pwm", "EG1 PWM");
    addKnob  (osc1Sec, "osc1Lfo1Pwm", "LFO1 PWM");

    auto& osc2Sec = addSection ("OSC 2", 0);
    addCombo  (osc2Sec, "osc2Wave", "Wave");
    addKnob   (osc2Sec, "osc2Tune", "Tune");
    addKnob   (osc2Sec, "osc2Lfo1Mod", "LFO1 Mod");
    addKnob   (osc2Sec, "osc2Lfo2Mod", "LFO2 Mod");
    addKnob   (osc2Sec, "osc2Eg1Pwm", "EG1 PWM");
    addKnob   (osc2Sec, "osc2Lfo1Pwm", "LFO1 PWM");
    addToggle (osc2Sec, "osc2Sync", "Sync");

    auto& mixSec = addSection ("Mixer", 0);
    addKnob (mixSec, "mixOsc1", "OSC1");
    addKnob (mixSec, "mixSub1", "Sub1");
    addKnob (mixSec, "mixOsc2", "OSC2");
    addKnob (mixSec, "mixSub2", "Sub2");
    addKnob (mixSec, "mixNoise", "Noise");

    auto& modeSec = addSection ("Mode", 0);
    addCombo (modeSec, "unisonVoices", "Voices");
    addKnob  (modeSec, "unisonDetune", "Detune");
    addKnob  (modeSec, "glideTime", "Portamento");

    auto& filterSec = addSection ("Filter", 1);
    addKnob (filterSec, "hpfStep", "HPF");
    addKnob (filterSec, "vcfCutoff", "Cutoff");
    addKnob (filterSec, "vcfResonance", "Reso");
    addKnob (filterSec, "vcfKeyFollow", "Kybd");
    addKnob (filterSec, "vcfEg1Mod", "EG1 Mod");
    addKnob (filterSec, "vcfLfo1Mod", "LFO1 Mod");
    addKnob (filterSec, "vcfLfo2Mod", "LFO2 Mod");

    auto& ampSec = addSection ("Amplifier", 1);
    addKnob  (ampSec, "ampLfo1Mod", "LFO1 Mod");
    addKnob  (ampSec, "ampLfo2Mod", "LFO2 Mod");
    addKnob  (ampSec, "ampKeyTrack", "Kybd Trk");
    addKnob  (ampSec, "ampVelocityMod", "Vel Mod");
    addCombo (ampSec, "vcaGateMode", "Mode");

    auto& eg1Sec = addSection ("Envelope 1 (Filter)", 1);
    addKnob (eg1Sec, "eg1Attack", "A");
    addKnob (eg1Sec, "eg1Decay", "D");
    addKnob (eg1Sec, "eg1Sustain", "S");
    addKnob (eg1Sec, "eg1Release", "R");
    addKnob (eg1Sec, "eg1VelocityMod", "Vel Mod");
    addKnob (eg1Sec, "eg1KeyTrack", "Kybd Trk");

    auto& eg2Sec = addSection ("Envelope 2 (Amp)", 1);
    addKnob (eg2Sec, "eg2Attack", "A");
    addKnob (eg2Sec, "eg2Decay", "D");
    addKnob (eg2Sec, "eg2Sustain", "S");
    addKnob (eg2Sec, "eg2Release", "R");
    addKnob (eg2Sec, "eg2VelocityMod", "Vel Mod");
    addKnob (eg2Sec, "eg2KeyTrack", "Kybd Trk");

    auto& chorusSec = addSection ("Chorus", 1);
    addKnob (chorusSec, "chorusRate", "Rate");
    addKnob (chorusSec, "chorusDepth", "Depth");
    addKnob (chorusSec, "chorusMix", "Mix");

    auto& delaySec = addSection ("Delay", 1);
    addKnob (delaySec, "delayTime", "Time");
    addKnob (delaySec, "delayFeedback", "Feedback");
    addKnob (delaySec, "delayMix", "Mix");

    auto& outSec = addSection ("Output", 1);
    addKnob  (outSec, "masterVolume", "Volume");
    addCombo (outSec, "oversampling", "Quality");

    addChildComponent (saveOverlay);
    presetBar.onSaveRequested = [this] (const juce::String& currentName)
    {
        saveOverlay.show (currentName);
    };
    saveOverlay.onSave = [this] (const juce::String& newName)
    {
        proc.presetManager.savePresetAs (newName);
        presetBar.refreshLabel();
    };
    saveOverlay.onCancel = [] {};

    setResizable (true, true);
    setResizeLimits (1100, 620, 2000, 1100);
    setSize (1480, 760);
}

JunoCloneAudioProcessorEditor::~JunoCloneAudioProcessorEditor() { setLookAndFeel (nullptr); }

JunoCloneAudioProcessorEditor::Section& JunoCloneAudioProcessorEditor::addSection (const juce::String& title, int row)
{
    auto sec = std::make_unique<Section>();
    sec->title = title;
    sec->row = row;
    sec->group.setText (title);
    addAndMakeVisible (sec->group);
    sections.push_back (std::move (sec));
    return *sections.back();
}

JunoCloneAudioProcessorEditor::Control& JunoCloneAudioProcessorEditor::addKnob (Section& sec, const juce::String& paramId, const juce::String& text)
{
    auto c = std::make_unique<Control>();
    c->slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox);
    c->label.setText (text, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setFont (juce::Font (13.5f, juce::Font::bold));
    addAndMakeVisible (*c->slider);
    addAndMakeVisible (c->label);
    c->sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, paramId, *c->slider);
    sec.controls.push_back (c.get());
    allControls.push_back (std::move (c));
    return *allControls.back();
}

JunoCloneAudioProcessorEditor::Control& JunoCloneAudioProcessorEditor::addCombo (Section& sec, const juce::String& paramId, const juce::String& text)
{
    auto c = std::make_unique<Control>();
    c->combo = std::make_unique<juce::ComboBox>();
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (proc.apvts.getParameter (paramId)))
    {
        int idx = 1;
        for (auto& choiceText : choice->choices)
            c->combo->addItem (choiceText, idx++);
    }
    c->label.setText (text, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setFont (juce::Font (13.5f, juce::Font::bold));
    addAndMakeVisible (*c->combo);
    addAndMakeVisible (c->label);
    c->comboAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (proc.apvts, paramId, *c->combo);
    sec.controls.push_back (c.get());
    allControls.push_back (std::move (c));
    return *allControls.back();
}

JunoCloneAudioProcessorEditor::Control& JunoCloneAudioProcessorEditor::addToggle (Section& sec, const juce::String& paramId, const juce::String& text)
{
    auto c = std::make_unique<Control>();
    c->toggle = std::make_unique<juce::TextButton> (text);
    c->toggle->setClickingTogglesState (true);
    c->label.setText (text, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setFont (juce::Font (13.5f, juce::Font::bold));
    addAndMakeVisible (*c->toggle);
    addAndMakeVisible (c->label);
    c->buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, paramId, *c->toggle);
    sec.controls.push_back (c.get());
    allControls.push_back (std::move (c));
    return *allControls.back();
}

void JunoCloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (lnf.panelColour);
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawText ("RetroPoly-80", getLocalBounds().removeFromTop (36).reduced (12, 0), juce::Justification::centredLeft);

    g.setColour (juce::Colours::white.withAlpha (0.6f));
    g.setFont (juce::Font (12.0f));
    g.drawText ("Voices", juce::Rectangle<int> (getWidth() - 160, 10, 60, 16), juce::Justification::centredRight);
}

void JunoCloneAudioProcessorEditor::layoutRow (const std::vector<Section*>& rowSections, juce::Rectangle<int> rowBounds)
{
    if (rowSections.empty()) return;
    int sectionWidth = rowBounds.getWidth() / (int) rowSections.size();
    int x = rowBounds.getX();

    for (auto* secPtr : rowSections)
    {
        auto& sec = *secPtr;
        auto secBounds = juce::Rectangle<int> (x, rowBounds.getY(), sectionWidth - 6, rowBounds.getHeight());
        sec.group.setBounds (secBounds);

        auto inner = secBounds.reduced (8, 22);
        int n = juce::jmax (1, (int) sec.controls.size());
        int controlW = inner.getWidth() / n;

        int cx = inner.getX();
        for (auto* ctrl : sec.controls)
        {
            auto cBounds = juce::Rectangle<int> (cx, inner.getY(), controlW - 4, inner.getHeight() - 22);
            if (ctrl->combo != nullptr)
            {
                ctrl->combo->setBounds (cBounds.removeFromTop (24).withSizeKeepingCentre (controlW - 8, 22));
            }
            else if (ctrl->toggle != nullptr)
            {
                ctrl->toggle->setBounds (cBounds.withSizeKeepingCentre (controlW - 8, 26));
            }
            else
            {
                ctrl->slider->setBounds (cBounds);
            }
            ctrl->label.setBounds (cx, inner.getBottom() - 20, controlW - 4, 20);
            cx += controlW;
        }
        x += sectionWidth;
    }
}

void JunoCloneAudioProcessorEditor::resized()
{
    saveOverlay.setBounds (getLocalBounds());

    auto area = getLocalBounds();
    area.removeFromTop (36);

    auto topBar = area.removeFromTop (32);
    voiceLedBar.setBounds (topBar.removeFromRight (160).reduced (4));
    presetBar.setBounds (topBar.reduced (200, 0));

    area.reduce (8, 8);

    std::vector<Section*> row0, row1;
    for (auto& s : sections)
        (s->row == 0 ? row0 : row1).push_back (s.get());

    int rowHeight = area.getHeight() / 2;
    auto row0Bounds = area.removeFromTop (rowHeight - 4);
    area.removeFromTop (8);
    auto row1Bounds = area;

    layoutRow (row0, row0Bounds);
    layoutRow (row1, row1Bounds);
}
