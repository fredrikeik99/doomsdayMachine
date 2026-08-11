/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DoomsdayMachineAudioProcessor::DoomsdayMachineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

DoomsdayMachineAudioProcessor::~DoomsdayMachineAudioProcessor()
{
}

//==============================================================================
const juce::String DoomsdayMachineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DoomsdayMachineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DoomsdayMachineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DoomsdayMachineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DoomsdayMachineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DoomsdayMachineAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DoomsdayMachineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DoomsdayMachineAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DoomsdayMachineAudioProcessor::getProgramName (int index)
{
    return {};
}

void DoomsdayMachineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DoomsdayMachineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void DoomsdayMachineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DoomsdayMachineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DoomsdayMachineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    // Bitcrusher: downsample by holding the sample value for N samples
    double downsampleFactor = juce::jlimit(1.0, 40.0, bitcrush_amount * mix_amount);
    if (downsampleFactor < 1.0)
    {
        downsampleFactor = 1.0;
    }
    int downsampleInt = static_cast<int>(downsampleFactor);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        double heldSample = 0.0f;
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            if (sample % downsampleInt == 0)
            {
                heldSample = buffer.getSample(channel, sample);
            }

            if (mix_amount > 0.0)
            {
				cube_distortion(heldSample, mix_amount);
            }

			double rand = random_gen.nextDouble(); // Advance the random generator to create a new random value for each sample
			double output = juce::jlimit(-1.0, 1.0, heldSample * gain_amount);
            channelData[sample] = output;
        }
    }
}

void DoomsdayMachineAudioProcessor::cube_distortion(double& input, double& mix)
{
    float RC = 1.0f / (2.0f * 3.1415f * cutoff_freq);
    float alpha = 1.0f / (1.0f + (RC * getSampleRate()));

    double rand = (random_gen.nextDouble() * 2.0 - 1.0); // Range: -1.0 to 1.0
    double noiseAmount = 0.05 * mix * std::abs(input); // Adjust 0.01 for desired noise level
    double distortion = (input * input * input) * mix + rand * noiseAmount;
    input = juce::jlimit(-1.0, 1.0, distortion + alpha * input);
}

//==============================================================================
bool DoomsdayMachineAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DoomsdayMachineAudioProcessor::createEditor()
{
    return new DoomsdayMachineAudioProcessorEditor (*this);
}

//==============================================================================
void DoomsdayMachineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DoomsdayMachineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DoomsdayMachineAudioProcessor();
}
