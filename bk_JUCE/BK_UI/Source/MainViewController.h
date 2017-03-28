/*
  ==============================================================================

    MainViewController.h
    Created: 27 Mar 2017 2:04:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef MAINVIEWCONTROLLER_H_INCLUDED
#define MAINVIEWCONTROLLER_H_INCLUDED

#include "PluginProcessor.h"

#include "SynchronicViewController.h"
#include "NostalgicViewController.h"
#include "DirectViewController.h"
#include "TuningViewController.h"
#include "TempoViewController.h"

#include "GeneralViewController.h"

#include "GalleryViewController.h"

#include "KeymapViewController.h"

class MainViewController :  public Component, private Timer, public BKListener
{
    
public:
    MainViewController (BKAudioProcessor&);
    ~MainViewController();
    
    
    
    
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it
    
    BKAudioProcessor& processor;
    
    GalleryViewController galvc;
    
    KeymapViewController kvc;
    TuningViewController tvc;
    GeneralViewController gvc;
    SynchronicViewController svc;
    NostalgicViewController nvc;
    DirectViewController dvc;
    TempoViewController ovc;
    
    OwnedArray<TextButton> buttons;
    
    void timerCallback() override;

    int timerCallbackCount;
    
    BKPreparationDisplay currentDisplay;
    
    void bkTextFieldDidChange       (TextEditor&)           override{};
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override{};
    void bkButtonClicked            (Button* b)             override;
    
    void setCurrentDisplay(BKPreparationDisplay type);
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewController)
};

#endif  // MAINVIEWCONTROLLER_H_INCLUDED