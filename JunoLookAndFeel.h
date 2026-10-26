#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class JunoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    JunoLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, accentColour);
        setColour (juce::Slider::trackColourId, juce::Colours::black);
        setColour (juce::Slider::backgroundColourId, panelColour.darker (0.3f));
        setColour (juce::ResizableWindow::backgroundColourId, panelColour);
        setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.92f));
        setColour (juce::GroupComponent::textColourId, accentColour);
        setColour (juce::GroupComponent::outlineColourId, juce::Colours::white.withAlpha (0.25f));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff0e1a2c));
        setColour (juce::ComboBox::outlineColourId, juce::Colours::white.withAlpha (0.3f));
        setColour (juce::ComboBox::textColourId, juce::Colours::white);
        setColour (juce::TextButton::buttonColourId, juce::Colour (0xff0e1a2c));
        setColour (juce::TextButton::buttonOnColourId, accentColour.darker (0.1f));
        setColour (juce::TextButton::textColourOffId, juce::Colours::white.withAlpha (0.85f));
        setColour (juce::TextButton::textColourOnId, juce::Colours::black);
        setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff0e1a2c));
        setColour (juce::TextEditor::textColourId, juce::Colours::white);
        setColour (juce::TextEditor::outlineColourId, juce::Colours::white.withAlpha (0.3f));
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                            float pos, float startAngle, float endAngle, juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();
        auto angle = startAngle + pos * (endAngle - startAngle);

        // --- Schritt: garantiert kreisrunde Potis ---
        auto circleBounds = juce::Rectangle<float> (radius * 2.0f, radius * 2.0f).withCentre (centre);

        g.setColour (juce::Colour (0xff10151c));
        g.fillEllipse (circleBounds);

        g.setColour (juce::Colours::white.withAlpha (0.7f));
        for (int i = 0; i <= 10; ++i)
        {
            float a = startAngle + (endAngle - startAngle) * (i / 10.0f);
            auto p1 = centre.getPointOnCircumference (radius + 1.0f, a - juce::MathConstants<float>::halfPi);
            auto p2 = centre.getPointOnCircumference (radius - 2.0f, a - juce::MathConstants<float>::halfPi);
            g.drawLine ({ p1, p2 }, 1.2f);
        }

        auto capBounds = circleBounds.reduced (radius * 0.22f);
        juce::ColourGradient grad (juce::Colour (0xfffafaf6), centre.x, centre.y - radius,
                                    juce::Colour (0xffd6d6cd), centre.x, centre.y + radius, false);
        g.setGradientFill (grad);
        g.fillEllipse (capBounds);
        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.drawEllipse (capBounds, 1.0f);

        // --- Schritt: deutlich laengerer, dickerer Zeiger ---
        auto capRadius = capBounds.getWidth() * 0.5f;
        juce::Point<float> tip (centre.x + capRadius * 0.94f * std::cos (angle - juce::MathConstants<float>::halfPi),
                                 centre.y + capRadius * 0.94f * std::sin (angle - juce::MathConstants<float>::halfPi));
        g.setColour (juce::Colours::black);
        g.drawLine ({ centre, tip }, 3.4f);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
        g.setColour (juce::Colour (0xff0e1a2c));
        g.fillRoundedRectangle (bounds, 3.0f);
        g.setColour (juce::Colours::white.withAlpha (0.3f));
        g.drawRoundedRectangle (bounds, 3.0f, 1.0f);

        auto led = juce::Rectangle<float> (bounds.getX() + 6.0f, bounds.getCentreY() - 4.0f, 8.0f, 8.0f);
        g.setColour (button.getToggleState() ? accentColour : juce::Colours::black.withAlpha (0.6f));
        g.fillEllipse (led);

        g.setColour (juce::Colours::white);
        g.setFont (juce::Font (13.5f));
        g.drawText (button.getButtonText(), bounds.withTrimmedLeft (20.0f), juce::Justification::centredLeft);
        juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
        auto baseColour = button.getToggleState()
                         ? accentColour
                         : juce::Colour (0xff0e1a2c).brighter (shouldDrawButtonAsHighlighted ? 0.15f : 0.0f);
        if (shouldDrawButtonAsDown) baseColour = baseColour.darker (0.2f);

        g.setColour (baseColour);
        g.fillRoundedRectangle (bounds, 4.0f);
        g.setColour (juce::Colours::white.withAlpha (0.3f));
        g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
    }

    void drawGroupComponentOutline (juce::Graphics& g, int w, int h, const juce::String& text,
                                     const juce::Justification&, juce::GroupComponent&) override
    {
        auto bounds = juce::Rectangle<float> (0, 0, (float) w, (float) h).reduced (2.0f);
        g.setColour (juce::Colour (0xff17283f));
        g.fillRoundedRectangle (bounds, 5.0f);
        g.setColour (findColour (juce::GroupComponent::outlineColourId));
        g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

        g.setColour (accentColour);
        g.setFont (juce::Font (14.5f, juce::Font::bold));
        g.drawText (text.toUpperCase(), bounds.removeFromTop (19.0f).reduced (6.0f, 0.0f),
                    juce::Justification::centredLeft);
    }

    juce::Colour panelColour  { 0xff1c3253 };
    juce::Colour accentColour { 0xfff4a93c };
};
