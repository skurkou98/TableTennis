/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class TableTennisAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    TableTennisAudioProcessor();
    ~TableTennisAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:
    juce::AudioBuffer<float> delayBuffer;

    int delayBufferLength = 1;
    int readIndexL = 0; // LEFT
    int readIndexR = 0; // RIGHT
    int writeIndex = 0; // Write

    juce::AudioParameterFloat* delayTimeL;  // LEFT
    juce::AudioParameterFloat* delayTimeR; // RIGHT
    juce::AudioParameterFloat* feedback; // Feedback
    juce::AudioParameterFloat* delayMix; // Wet-Dry Mix
    //==============================================================================
};
