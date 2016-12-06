
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "BKUtilities.h"

#include "BKSampleLoader.h"

#include "BKSynthesiser.h"

#include "Synchronic.h"

#include "Nostalgic.h"

#include "Keymap.h"

#include "Tuning.h"

#define USE_SYNCHRONIC_TWO 0

//==============================================================================
/**
*/
class BKAudioProcessor  : public AudioProcessor,
                           public ChangeListener,
                            public BKSampleLoader
{
    
public:
    //==============================================================================
    BKAudioProcessor();
    ~BKAudioProcessor();
    
    
    int numSynchronicLayers;
    int currentSynchronicLayer;
    
    int numNostalgicLayers;
    int currentNostalgicLayer;
    
    // Synthesisers.
    BKSynthesiser mainPianoSynth;
    BKSynthesiser hammerReleaseSynth;
    BKSynthesiser resonanceReleaseSynth;

    // Processors.
    SynchronicProcessor::CSArr      sProcessor;
    NostalgicProcessor::Arr         nProcessor;
    
    // Preparations.
    SynchronicPreparation::CSArr    sPreparation;
    NostalgicPreparation::CSArr     nPreparation;
    
    // Keymaps.
    Keymap::CSArr                   bkKeymaps;
    
    // Tuning.
    TuningProcessor tuner;
    
    int channel;
    
    // Change listener callback implementation
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    

private:
    void loadMainPianoSamples(BKSynthesiser *synth, int numLayers);
    void loadResonanceRelaseSamples(BKSynthesiser *synth);
    void loadHammerReleaseSamples(BKSynthesiser *synth);
    
    TuningSystem mainTuning = PartialTuning;
    int tuningBasePitch = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED