/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DoomsdayMachineAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    DoomsdayMachineAudioProcessorEditor (DoomsdayMachineAudioProcessor&);
    ~DoomsdayMachineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
	void sliderValueChanged(juce::Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DoomsdayMachineAudioProcessor& audioProcessor;

	juce::Slider gain_slider;
	juce::Slider mix_slider;
    juce::Slider bitcrush_slider;

	juce::Label gain_label;
	juce::Label mix_label;
    juce::Label bitcrush_label;
    juce::Label version_label;

    juce::Image background_image;
	juce::Image gain_image;
    juce::Image label;

    juce::String version;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DoomsdayMachineAudioProcessorEditor)
};
