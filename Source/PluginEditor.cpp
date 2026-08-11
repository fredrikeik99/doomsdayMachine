/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DoomsdayMachineAudioProcessorEditor::DoomsdayMachineAudioProcessorEditor (DoomsdayMachineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    setSize(800, 600);

	// GAIN SLIDER
	gain_slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	gain_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
	gain_slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::red.withAlpha(0.5f));
	gain_slider.setRange(-24.0, 12.0, 0.01);
	gain_slider.setValue(-1.0);
	gain_slider.addListener(this);
	gain_slider.setColour(juce::Slider::thumbColourId, juce::Colours::black.withAlpha(0.8f));
	gain_slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred.withAlpha(0.5f).withBrightness(1.f));
	gain_slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey.withAlpha(0.4f));
	addAndMakeVisible(gain_slider);

	gain_label.setText("GAIN", juce::dontSendNotification);
	gain_label.setFont(juce::Font(22.0f, juce::Font::bold));
	gain_label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.7f));
	addAndMakeVisible(gain_label);
	gain_label.setInterceptsMouseClicks(false, false);

	// mix SLIDER
	mix_slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	mix_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 50);
	mix_slider.setRange(0.0, 100.0, 0.01);
	mix_slider.setValue(0.0);
	mix_slider.addListener(this);
	mix_slider.setColour(juce::Slider::thumbColourId, juce::Colours::black.withAlpha(0.8f));
	mix_slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred.withAlpha(0.5f).withBrightness(1.f));
	mix_slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey.withAlpha(0.4f));
	addAndMakeVisible(mix_slider);

	// bitcrush slider
	bitcrush_slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	bitcrush_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
	bitcrush_slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::red.withAlpha(0.5f));
	bitcrush_slider.setRange(0.0, 1.0, 0.01);
	bitcrush_slider.setValue(0.0);
	bitcrush_slider.addListener(this);
	bitcrush_slider.setColour(juce::Slider::thumbColourId, juce::Colours::black.withAlpha(0.8f));
	bitcrush_slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred.withAlpha(0.5f).withBrightness(1.f));
	bitcrush_slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey.withAlpha(0.4f));
	addAndMakeVisible(bitcrush_slider);

	bitcrush_label.setText("CRUSH", juce::dontSendNotification);
	bitcrush_label.setFont(juce::Font(22.0f, juce::Font::bold));
	bitcrush_label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.7f));
	addAndMakeVisible(bitcrush_label);
	bitcrush_label.setInterceptsMouseClicks(false, false);
	
	version = "1.0.0";
	version_label.setText(version, juce::dontSendNotification);
	version_label.setFont(juce::Font(12.0f, juce::Font::bold));
	version_label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.7f));
	addAndMakeVisible(version_label);
	version_label.setInterceptsMouseClicks(false, false);

	// background
	background_image = juce::ImageCache::getFromMemory(BinaryData::doomsday_jpeg, BinaryData::doomsday_jpegSize);
	gain_image = juce::ImageCache::getFromMemory(BinaryData::doomsday_2jpeg, BinaryData::doomsday_2jpegSize);
	label = juce::ImageCache::getFromMemory(BinaryData::label_png, BinaryData::label_pngSize);
}

DoomsdayMachineAudioProcessorEditor::~DoomsdayMachineAudioProcessorEditor()
{
}

//==============================================================================
void DoomsdayMachineAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
	if (!background_image.isNull())
	{
		float mix = static_cast<float>(mix_slider.getValue() / mix_slider.getMaximum()); // 0.0 to 1.0

		if (!background_image.isNull())
		{
			g.setOpacity(1.0f - mix);
			g.drawImageWithin(background_image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
		}
		if (!gain_image.isNull())
		{
			g.setOpacity(mix);
			g.drawImageWithin(gain_image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
		}
		if (!label.isNull())
		{
			g.setOpacity(1.0f);
			g.drawImageWithin(label, 250, 330, label.getWidth() / 5, label.getHeight() / 5, juce::RectanglePlacement::stretchToFit);
		}

		g.setOpacity(1.0f); // Reset opacity for other drawing
	}
	else
	{
		g.fillAll(juce::Colours::darkred);
	}
}

void DoomsdayMachineAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

	// Set the position and size of the mix slider
	{
		mix_slider.setSize(400, 400);
		// get center of the editor and position the mix slider there
		int width = mix_slider.getWidth();
		int height = mix_slider.getHeight();
		int x = (getWidth() - width) / 2;
		int y = (getHeight() - height) / 2;
		mix_slider.setBounds(getLocalBounds().withX(x).withY(y - 70).withWidth(width).withHeight(height));
	}
	// Set the position and size of the gain slider
	{
		gain_slider.setSize(150, 150);
		// offset to bottom right corner of the editor
		int width = gain_slider.getWidth();
		int height = gain_slider.getHeight();
		int x = (getWidth() - (width + (width / 2)));
		int y = (getHeight() - (height + (height / 2)));
		gain_slider.setBounds(getLocalBounds().withX(x + 50).withY(y + 30).withWidth(width).withHeight(height));
		
		// Center the label inside the slider
		int labelWidth = 100;
		int labelHeight = 100;
		int labelX = gain_slider.getX() + (width - labelWidth) / 2;
		int labelY = gain_slider.getY() + (height - labelHeight) / 2;
		gain_label.setBounds(labelX + 20, labelY - 15, labelWidth, labelHeight);
	}
	// bitcrush
	{
		bitcrush_slider.setSize(150, 150);
		// offset to bottom left corner of the editor
		int width = bitcrush_slider.getWidth();
		int height = bitcrush_slider.getHeight();
		int x = width / 2;
		int y = (getHeight() - (height + (height / 2)));
		bitcrush_slider.setBounds(getLocalBounds().withX(x - 50).withY(y + 30).withWidth(width).withHeight(height));

		// Center the label inside the slider
		int labelWidth = 100;
		int labelHeight = 100;
		int labelX = bitcrush_slider.getX() + (width - labelWidth) / 2;
		int labelY = bitcrush_slider.getY() + (height - labelHeight) / 2;
		bitcrush_label.setBounds(labelX + 10, labelY - 15, labelWidth, labelHeight);
	}
	// version
	{
		version_label.setBounds(20, 580, 70, 70);
	}
}

void DoomsdayMachineAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &gain_slider) 
    {
		audioProcessor.gain_amount = pow(10.0, gain_slider.getValue() / 20.0);
    }
    if (slider == &mix_slider)
    {
		audioProcessor.mix_amount = mix_slider.getValue();
		repaint(); // Trigger a repaint to update the background image based on the new mix value
    }
	if (slider == &bitcrush_slider)
	{
		audioProcessor.bitcrush_amount = bitcrush_slider.getValue();
	}
}