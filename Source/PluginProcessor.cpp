/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TableTennisAudioProcessor::TableTennisAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    delayTimeL = new juce::AudioParameterFloat("delayTimeL", "L Delay (secs)", 0.0f, 3.0f, 0.3f); // Delay Time (0-3000 milliseconds) (Left Channel)
    delayTimeR = new juce::AudioParameterFloat("delayTimeR", "R Delay (secs)", 0.0f, 3.0f, 0.3f); // Delay Time (0-3000 milliseconds) (Right Channel)
    feedback = new juce::AudioParameterFloat("feedback", "Feedback", 0.0f, 0.99f, 0.0f); // Feedback (0-0.99)
    delayMix = new juce::AudioParameterFloat("delayMix", "Delay Level", 0.0f, 1.0f, 0.5f); // Wet-Dry Mix (0-1.0)

    addParameter(delayTimeL);
    addParameter(delayTimeR);
    addParameter(feedback);
    addParameter(delayMix);
}

TableTennisAudioProcessor::~TableTennisAudioProcessor()
{
}

//==============================================================================
const juce::String TableTennisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TableTennisAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TableTennisAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TableTennisAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TableTennisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TableTennisAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TableTennisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TableTennisAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String TableTennisAudioProcessor::getProgramName(int index)
{
    return {};
}

void TableTennisAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void TableTennisAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    delayBufferLength = (int)(3 * sampleRate); // Sample rate represents 1secs of playback (set to leave enough headroom for 3sec delay, else crash occurs)
    delayBuffer.setSize(2, delayBufferLength); // 2 represents the number of channels (LEFT and RIGHT)
    delayBuffer.clear();
    readIndexL = (int)(writeIndex - (delayTimeL->get() * getSampleRate()) + delayBufferLength) % delayBufferLength;
    readIndexR = (int)(writeIndex - (delayTimeR->get() * getSampleRate()) + delayBufferLength) % delayBufferLength;
}

void TableTennisAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any space, memory etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TableTennisAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported
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

void TableTennisAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (buffer.getNumChannels() < 2) // Buffer must have 2 channels to work correctly
        return;

    int tempReadIndexL = 0;
    int tempReadIndexR = 0;
    int tempWriteIndex = 0;
    float tempDelayTimeL = 0.3f;
    float tempDelayTimeR = 0.3f;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* leftChannel = buffer.getWritePointer(0); // LEFT
        auto* rightChannel = buffer.getWritePointer(1); // RIGHT
        auto* leftDelay = delayBuffer.getWritePointer(0); // LEFT
        auto* rightDelay = delayBuffer.getWritePointer(1); // RIGHT

        tempReadIndexL = readIndexL;
        tempReadIndexR = readIndexR;
        tempWriteIndex = writeIndex;

        if (tempDelayTimeL != delayTimeL->get())
            readIndexL = (int)(writeIndex - (delayTimeL->get() * getSampleRate()) + delayBufferLength) % delayBufferLength;

        if (tempDelayTimeR != delayTimeR->get())
            readIndexR = (int)(writeIndex - (delayTimeR->get() * getSampleRate()) + delayBufferLength) % delayBufferLength;

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto inL = leftChannel[i]; // LEFT
            auto inR = rightChannel[i]; // RIGHT

            auto outL = inL + (delayMix->get() * leftDelay[tempReadIndexL]);
            auto outR = inR + (delayMix->get() * rightDelay[tempReadIndexR]);

            leftDelay[tempWriteIndex] = inL + (rightDelay[tempReadIndexR] * feedback->get()); // LEFT channel swaps with RIGHT for ping-pong effect
            rightDelay[tempWriteIndex] = inR + (leftDelay[tempReadIndexL] * feedback->get()); // RIGHT channel swaps with LEFT for ping-pong effect

            if (++tempReadIndexL >= delayBufferLength)
                tempReadIndexL = 0;
            if (++tempReadIndexR >= delayBufferLength)
                tempReadIndexR = 0;
            if (++tempWriteIndex >= delayBufferLength)
                tempWriteIndex = 0;

            leftChannel[i] = outL;
            rightChannel[i] = outR;
        }
    }
    readIndexL = tempReadIndexL;
    readIndexR = tempReadIndexR;
    writeIndex = tempWriteIndex;
    tempDelayTimeL = delayTimeL->get();
    tempDelayTimeR = delayTimeR->get();
}

//==============================================================================
bool TableTennisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TableTennisAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void TableTennisAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state here
    juce::MemoryOutputStream stream(destData, true);
    // Store a float into memory
    stream.writeFloat(*delayTimeL);
    stream.writeFloat(*delayTimeR);
    stream.writeFloat(*feedback);
    stream.writeFloat(*delayMix);
}

void TableTennisAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state here from getStateInformation block
    juce::MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);
    // Read a float from memory i.e. retrieve the parameter value
    *delayTimeL = stream.readFloat();
    *delayTimeR = stream.readFloat();
    *feedback = stream.readFloat();
    *delayMix = stream.readFloat();
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TableTennisAudioProcessor();
}
