/*
  ==============================================================================

    TuningViewController.h
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

#define TOP 70

//std::vector<std::string> intervalNames = {"U", "m2", "M2", "m3", "M3", "P4", "d5", "P5", "m6", "M6", "m7", "M7", "O"};

class TuningViewController :
public BKViewController,
public Timer
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    
    TuningViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningViewController()
    {
        setLookAndFeel(nullptr);
    };
    
    void timerCallback(void) override;
    
    void paint (Graphics&) override;
    void resized() override;
    
    virtual void update(void) {};
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
protected:
    //basics
    BKEditableComboBox selectCB;
    
    BKLabel scaleLabel;
    BKComboBox scaleCB;
    
    BKLabel fundamentalLabel;
    BKComboBox fundamentalCB;
    
    ScopedPointer<BKSingleSlider> offsetSlider;
    
    //adaptive tuning 1 stuff
    BKLabel A1IntervalScaleLabel;
    BKComboBox A1IntervalScaleCB;
    
    ToggleButton A1Inversional;
    
    BKLabel A1AnchorScaleLabel;
    BKComboBox A1AnchorScaleCB;
    
    BKLabel A1FundamentalLabel;
    BKComboBox A1FundamentalCB;
    
    ScopedPointer<BKSingleSlider> A1ClusterThresh;
    ScopedPointer<BKSingleSlider> A1ClusterMax;
    
    BKTextButton A1reset;
    
    BKTextButton showSpringsButton;
    bool showSprings;

    Array<float> absoluteOffsets;   //for entire keyboard; up to 128 vals
    Array<float> customOffsets;     //for custom tuning; 12 vals
    
    BKLabel lastNote;
    BKLabel lastInterval;
    float lastNoteTuningSave = -.1;
    
    BKLabel currentFundamental;
    
    BKAbsoluteKeyboardSlider absoluteKeyboard;
    BKCircularKeyboardSlider customKeyboard; 
    
    ImageComponent iconImageComponent;
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    
    BKComboBox nToneRootCB;
    BKComboBox nToneRootOctaveCB;
    ScopedPointer<BKSingleSlider> nToneSemitoneWidthSlider;
    
    OwnedArray<Slider> springSliders;
    OwnedArray<Slider> tetherSliders;
    OwnedArray<Label>  springLabels;
    OwnedArray<Label>  tetherLabels;
    
    BKComboBox springScaleCB;
    
    Label rateSliderLabel;
    ScopedPointer<BKSingleSlider> rateSlider;
    
    Label dragSliderLabel;
    ScopedPointer<BKSingleSlider> dragSlider;
    
    Label tetherStiffnessLabel;
    ScopedPointer<BKSingleSlider> tetherStiffnessSlider;
    
    Label intervalStiffnessLabel;
    ScopedPointer<BKSingleSlider> intervalStiffnessSlider;
    
    BKTextButton springTuningToggle;
    Label springTuningLabel;
    
    
    void fillTuningCB(void);
    void fillFundamentalCB(void);
    
    void updateComponentVisibility();
    
    int customIndex; //index of Custom tuning in combobox
private:
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningViewController)
    
};

class TuningPreparationEditor :
public TuningViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
//public BKKeyboardSlider::Listener,
public BKCircularKeyboardSlider::Listener,
public BKAbsoluteKeyboardSlider::Listener,
public Slider::Listener
{
public:
    
    TuningPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningPreparationEditor() {setLookAndFeel(nullptr);};
    
    void update(void) override;
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, TuningPreparationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    void bkComboBoxDidChange (ComboBox* box) override;
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void keyboardSliderChanged(String name, Array<float> values) override;
    void sliderValueChanged (Slider* slider) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningPreparationEditor)
    
};

class TuningModificationEditor :
public TuningViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
//public BKKeyboardSlider::Listener,
public BKCircularKeyboardSlider::Listener,
public Slider::Listener,
public BKAbsoluteKeyboardSlider::Listener
{
public:
    
    TuningModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningModificationEditor()
    {
        setLookAndFeel(nullptr);
    };
    
    void update(void) override;
    void updateModification(void);
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, TuningModificationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
    void greyOutAllComponents();
    void highlightModedComponents();
    
private:
    
    void bkComboBoxDidChange (ComboBox* box) override;
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void keyboardSliderChanged(String name, Array<float> values) override;
    void sliderValueChanged (Slider* slider) override;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningModificationEditor)
    
};
