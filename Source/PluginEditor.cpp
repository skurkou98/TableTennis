/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TableTennisAudioProcessorEditor::TableTennisAudioProcessorEditor(TableTennisAudioProcessor& p)
    : GenericAudioProcessorEditor(&p), audioProcessor(p) 
{
    setSize(400, 250);
}

TableTennisAudioProcessorEditor::~TableTennisAudioProcessorEditor()
{
}

//==============================================================================
void TableTennisAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Component is opaque, so background must be filled with a solid colour
    g.fillAll(juce::Colours::blue);
    g.setColour(juce::Colours::white);
    g.setFont(30);
    g.drawFittedText("Table Tennis", 50, 20, 130, 30, juce::Justification::centredLeft, 1, 0.0f);
}

void TableTennisAudioProcessorEditor::resized()
{
    // Lay out the positions of subcomponents here
}