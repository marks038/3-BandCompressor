/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
/*
DSP Roadmap
1) figure out how to split the audio into 3 bands
2) create parameters to control where this split happens
3) prove that splitting into 3 bands produces no audible artifacts
4) create audio parameters for the 3 compressor bands. These must lice on each band instance
5) add 2 listening compressors
6) add ability to mute/solo/bypass individual compressors
8) clean up anything that needs to be cleaned up
*/


#include <JuceHeader.h>

namespace Params
{
    enum Names
    {
        Low_Mid_Crossover_Freq,
        Mid_High_Crossover_Freq,

        Threshold_Low_Band,
        Threshold_Mid_Band,
        Threshold_High_Band,

        Attack_Low_Band,
        Attack_Mid_Band,
        Attack_High_Band,

        Release_Low_Band,
        Release_Mid_Band,
        Release_High_Band,

        Ratio_Low_Band,
        Ratio_Mid_Band,
        Ratio_High_Band,

        Bypassed_Low_Band,
        Bypassed_Mid_Band,
        Bypassed_High_Band,
    };
    
    inline const std::map<Names, juce::String>& GetParams()
    {
        static std::map<Names, juce::String> params =
        {
            {Low_Mid_Crossover_Freq, "Low_Mid_Crossover_Freq"},
            {Mid_High_Crossover_Freq, "Mid_High_Crossover_Freq"},
            {Threshold_Low_Band, "Threshold_Low_Band"},
            {Threshold_Mid_Band, "Threshold_Mid_Band"},
            {Threshold_High_Band, "Threshold_High_Band"},
            {Attack_Low_Band, "Attack_Low_Band"},
            {Attack_Mid_Band, "Attack_Mid_Band"},
            {Attack_High_Band, "Attack_High_Band"},
            {Release_Low_Band, "Release_Low_Band"},
            {Release_Mid_Band, "Release_Mid_Band"},
            {Release_High_Band, "Release_High_Band"},
            {Ratio_Low_Band, "Ratio_Low_Band"},
            {Ratio_Mid_Band, "Ratio_Mid_Band"},
            {Ratio_High_Band, "Ratio_High_Band"},
            {Bypassed_Low_Band, "Bypassed_Low_Band"},
            {Bypassed_Mid_Band, "Bypassed_Mid_Band"},
            {Bypassed_High_Band, "Bypassed_High_Band"},
        };
        return params;
    }
}

struct CompressorBand
{
public:
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }

    void updateCompressorSettings()
    {
        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = bypassed->get();

        compressor.process(context);
    }

private:
    juce::dsp::Compressor<float> compressor;
};

//==============================================================================
/**
*/
class SimpleMBCompAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleMBCompAudioProcessor();
    ~SimpleMBCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts { *this, nullptr, "Parameters", createParameterLayout() };

private:

    //juce::dsp::Compressor<float> compressor;

    //juce::AudioParameterFloat* attack{ nullptr };
    //juce::AudioParameterFloat* release{ nullptr };
    //juce::AudioParameterFloat* threshold{ nullptr };
    //juce::AudioParameterChoice* ratio{ nullptr };
    //juce::AudioParameterBool* bypassed{ nullptr };

    CompressorBand compressor;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
