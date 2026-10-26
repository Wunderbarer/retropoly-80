#include "PluginEditor.h"

JunoCloneAudioProcessorEditor::JunoCloneAudioProcessorEditor (JunoCloneAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);
    addAndMakeVisible (presetBar);

    auto& lfoSec   = addSection ("LFO");
    addFader (lfoSec, "lfoRate",  "Rate");
    addFader (lfoSec, "lfoDelay", "Delay");
    addKnob  (lfoSec, "lfoToDco", "DCO Mod");
    addKnob  (lfoSec, "lfoToVcf", "VCF Mod");

    auto& dcoSec = addSection ("DCO");
    addKnob (dcoSec, "sawLevel",     "Saw");
    addKnob (dcoSec, "pulseLevel",   "Pulse");
    addKnob (dcoSec, "subLevel",     "Sub");
    addKnob (dcoSec, "noiseLevel",   "Noise");
    addKnob (dcoSec, "pwmAmount",    "PWM");
    addKnob (dcoSec, "pwmLfoAmount", "PWM LFO");

    auto& hpfSec = addSection ("HPF");
    addKnob (hpfSec, "hpfStep", "Cutoff");

    auto& vcfSec = addSection ("VCF");
    addKnob (vcfSec, "vcfCutoff",    "Cutoff");
    addKnob (vcfSec, "vcfResonance", "Reso");
    addKnob (vcfSec, "vcfEnvAmount", "Env Amt");
    addKnob (vcfSec, "vcfKeyFollow", "Kybd");

    auto& envSec = addSection ("ENV / VCA");
    addFader (envSec, "attack",  "A");
    addFader (envSec, "decay",   "D");
    addFader (envSec, "sustain", "S");
    addFader (envSec, "release", "R");
    addCombo (envSec, "vcaGateMode", "Mode");

    auto& fxSec = addSection ("Chorus / Unisono");
    addCombo (fxSec, "chorusMode",    "Chorus");
    addCombo (fxSec, "unisonVoices",  "Voices");
    addKnob  (fxSec, "unisonDetune",  "Detune");
    addKnob  (fxSec, "glideTime",     "Glide");
    addCombo (fxSec, "oversampling",  "Quality");

    setSize (1040, 440);
}

JunoCloneAudioProcessorEditor::~JunoCloneAudioProcessorEditor() { setLookAndFeel (nullptr); }

JunoCloneAudioProcessorEditor::Section& JunoCloneAudioProcessorEditor::addSection (const juce::String& title)
{
    auto sec = std::make_unique<Section>();
    sec->title = title;
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
    c->label.setFont (juce::Font (11.0f));
    addAndMakeVisible (*c->slider);
    addAndMakeVisible (c->label);
    c->sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, paramId, *c->slider);
    sec.controls.push_back (c.get());
    allControls.push_back (std::move (c));
    return *allControls.back();
}

JunoCloneAudioProcessorEditor::Control& JunoCloneAudioProcessorEditor::addFader (Section& sec, const juce::String& paramId, const juce::String& text)
{
    auto c = std::make_unique<Control>();
    c->slider = std::make_unique<juce::Slider> (juce::Slider::LinearVertical, juce::Slider::NoTextBox);
    c->label.setText (text, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setFont (juce::Font (11.0f));
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
    c->label.setFont (juce::Font (11.0f));
    addAndMakeVisible (*c->combo);
    addAndMakeVisible (c->label);
    c->comboAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (proc.apvts, paramId, *c->combo);
    sec.controls.push_back (c.get());
    allControls.push_back (std::move (c));
    return *allControls.back();
}

void JunoCloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (lnf.panelColour);
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawText ("RetroPoly-80", getLocalBounds().removeFromTop (34).reduced (10, 0), juce::Justification::centredLeft);
}

void JunoCloneAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop (34); // Platz fuer Titel
    presetBar.setBounds (area.removeFromTop (32).reduced (200, 0));
    area.reduce (10, 10);

    int sectionWidth = area.getWidth() / (int) sections.size();
    int x = area.getX();

    for (auto& secPtr : sections)
    {
        auto& sec = *secPtr;
        auto secBounds = juce::Rectangle<int> (x, area.getY(), sectionWidth - 8, area.getHeight());
        sec.group.setBounds (secBounds);

        auto inner = secBounds.reduced (10, 22);
        int controlW = inner.getWidth() / juce::jmax (1, (int) sec.controls.size());

        int cx = inner.getX();
        for (auto* ctrl : sec.controls)
        {
            auto cBounds = juce::Rectangle<int> (cx, inner.getY(), controlW - 4, inner.getHeight() - 16);
            if (ctrl->combo != nullptr)
            {
                ctrl->combo->setBounds (cBounds.removeFromTop (24).withSizeKeepingCentre (controlW - 8, 22));
            }
            else
            {
                ctrl->slider->setBounds (cBounds);
            }
            ctrl->label.setBounds (cx, inner.getBottom() - 14, controlW - 4, 14);
            cx += controlW;
        }

        x += sectionWidth;
    }
}
