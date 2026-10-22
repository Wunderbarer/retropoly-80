#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// --- Schritt: Optik wie das Original-Bedienfeld ---
// Eigenstaendiges, an das Original angelehntes Design: dunkles Metallic-Chassis,
// orange/rote Akzentfarbe, Drehpotis MIT Zeigerlinie + Skalenstrichen und
// Schieberegler als laengliche "Kappen" auf einem Schlitz - exakt die beiden
// Bedienelement-Typen, die auch die Original-Hardware verwendet. Kein Logo,
// kein Markenname, kein 1:1 kopiertes Bild - nur die Bedienelement-Typen und
// das allgemeine Farbschema sind angelehnt.
class JunoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    JunoLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, accentColour);
        setColour (juce::Slider::trackColourId, juce::Colours::black);
        setColour (juce::Slider::backgroundColourId, panelColour.darker (0.2f));
        setColour (juce::ResizableWindow::backgroundColourId, panelColour);
        setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.9f));
        setColour (juce::GroupComponent::textColourId, accentColour);
        setColour (juce::GroupComponent::outlineColourId, juce::Colours::grey.withAlpha (0.5f));
        setColour (juce::ComboBox::backgroundColourId, juce::Colours::black);
        setColour (juce::ComboBox::outlineColourId, juce::Colours::grey);
        setColour (juce::TextButton::buttonColourId, juce::Colours::black);
        setColour (juce::TextButton::textColourOffId, accentColour);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                            float pos, float startAngle, float endAngle, juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();
        auto angle = startAngle + pos * (endAngle - startAngle);

        // Poti-Koerper mit leichtem Radial-Verlauf, wirkt wie gebuerstetes Metall/Kunststoff
        juce::ColourGradient grad (juce::Colour (0xff4a4a4e), centre.x, centre.y - radius,
                                    juce::Colour (0xff1c1c1e), centre.x, centre.y + radius, false);
        g.setGradientFill (grad);
        g.fillEllipse (bounds);
        g.setColour (juce::Colours::black);
        g.drawEllipse (bounds, 1.5f);

        // Skalenstriche wie beim Original (11 Raststriche: min, 9 Zwischenwerte, max)
        g.setColour (juce::Colours::grey);
        for (int i = 0; i <= 10; ++i)
        {
            float a = startAngle + (endAngle - startAngle) * (i / 10.0f);
            auto p1 = centre.getPointOnCircumference (radius + 2.0f, a - juce::MathConstants<float>::halfPi);
            auto p2 = centre.getPointOnCircumference (radius + 6.0f, a - juce::MathConstants<float>::halfPi);
            g.drawLine ({ p1, p2 }, 1.0f);
        }

        // Zeiger
        juce::Point<float> tip (centre.x + radius * 0.75f * std::cos (angle - juce::MathConstants<float>::halfPi),
                                 centre.y + radius * 0.75f * std::sin (angle - juce::MathConstants<float>::halfPi));
        g.setColour (accentColour);
        g.drawLine ({ centre, tip }, 2.5f);
        g.fillEllipse (juce::Rectangle<float> (5.0f, 5.0f).withCentre (centre));
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                            float sliderPos, float, float, const juce::Slider::SliderStyle style,
                            juce::Slider&) override
    {
        // Schlitz
        g.setColour (juce::Colours::black);
        if (style == juce::Slider::LinearVertical)
            g.fillRoundedRectangle ((float) (x + w / 2 - 3), (float) y, 6.0f, (float) h, 2.0f);
        else
            g.fillRoundedRectangle ((float) x, (float) (y + h / 2 - 3), (float) w, 6.0f, 2.0f);

        g.setColour (juce::Colours::grey.withAlpha (0.4f));
        if (style == juce::Slider::LinearVertical)
            g.drawRoundedRectangle ((float) (x + w / 2 - 3), (float) y, 6.0f, (float) h, 2.0f, 1.0f);
        else
            g.drawRoundedRectangle ((float) x, (float) (y + h / 2 - 3), (float) w, 6.0f, 2.0f, 1.0f);

        // Laengliche Reglerkappe, wie bei den original Schiebereglern
        g.setColour (accentColour);
        if (style == juce::Slider::LinearVertical)
        {
            auto capRect = juce::Rectangle<float> ((float) x + 2.0f, sliderPos - 7.0f, (float) w - 4.0f, 14.0f);
            g.fillRoundedRectangle (capRect, 3.0f);
            g.setColour (juce::Colours::black.withAlpha (0.6f));
            g.drawLine (capRect.getX() + 3, capRect.getCentreY(), capRect.getRight() - 3, capRect.getCentreY(), 1.5f);
        }
        else
        {
            auto capRect = juce::Rectangle<float> (sliderPos - 7.0f, (float) y + 2.0f, 14.0f, (float) h - 4.0f);
            g.fillRoundedRectangle (capRect, 3.0f);
        }
    }

    void drawGroupComponentOutline (juce::Graphics& g, int w, int h, const juce::String& text,
                                     const juce::Justification&, juce::GroupComponent&) override
    {
        auto bounds = juce::Rectangle<float> (0, 0, (float) w, (float) h).reduced (2.0f);
        g.setColour (findColour (juce::GroupComponent::outlineColourId));
        g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

        g.setColour (accentColour);
        g.setFont (juce::Font (13.0f, juce::Font::bold));
        g.drawText (text.toUpperCase(), bounds.removeFromTop (16.0f).reduced (6.0f, 0.0f),
                    juce::Justification::centredLeft);
    }

    juce::Colour panelColour  { 0xff26262a };
    juce::Colour accentColour { 0xffff7a1a };
};
