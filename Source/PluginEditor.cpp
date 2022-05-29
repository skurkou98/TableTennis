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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 250);
}

TableTennisAudioProcessorEditor::~TableTennisAudioProcessorEditor()
{
}

//==============================================================================
void TableTennisAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::blue);
    g.setColour(juce::Colours::white);
    g.setFont(30);
    g.drawFittedText("Table Tennis", 50, 20, 130, 30, juce::Justification::centredLeft, 1, 0.0f);
}

void TableTennisAudioProcessorEditor::resized()
{
    //Lay out the positions of subcomponents here
}
