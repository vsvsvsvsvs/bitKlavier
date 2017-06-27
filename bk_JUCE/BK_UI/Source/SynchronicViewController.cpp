/*
  ==============================================================================

    SynchronicViewController.cpp
    Created: 21 Apr 2017 11:17:47pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "SynchronicViewController.h"
#include "BKUtilities.h"
#include "Keymap.h"
#include "Preparation.h"

SynchronicViewController::SynchronicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::synchronic_icon_png, BinaryData::synchronic_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    // MultSliders
    paramSliders = OwnedArray<BKMultiSlider>();
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        if (cSynchronicDataTypes[i] == BKFloatArr || cSynchronicDataTypes[i] == BKArrFloatArr)
        {
            paramSliders.insert(0, new BKMultiSlider(HorizontalMultiBarSlider));
            addAndMakeVisible(paramSliders[0]);
            paramSliders[0]->addMyListener(this);
            paramSliders[0]->setName(cSynchronicParameterTypes[i]);
            paramSliders[0]->setMinMaxDefaultInc(cSynchronicDefaultRangeValuesAndInc[i]);
        }
    }
    
    selectCB.setName("Synchronic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    modeSelectCB.setName("Mode");
    modeSelectCB.addSeparator();
    modeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    modeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(modeSelectCB);
    
    //offsetParamStartToggle = new BKSingleSlider("skip first", 0, 1, 0, 1);
    offsetParamStartToggle.setButtonText ("skip first");
    buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
    offsetParamStartToggle.setToggleState (true, dontSendNotification);
    addAndMakeVisible(offsetParamStartToggle);
    
    releaseVelocitySetsSynchronicToggle.addListener(this);
    releaseVelocitySetsSynchronicToggle.setLookAndFeel(&buttonsAndMenusLAF2); //need different one so toggle text can be on other side
    releaseVelocitySetsSynchronicToggle.setButtonText ("release loudness");
    buttonsAndMenusLAF2.setToggleBoxTextToRightBool(true);
    releaseVelocitySetsSynchronicToggle.setToggleState (false, dontSendNotification);
    addAndMakeVisible(releaseVelocitySetsSynchronicToggle);
    
    howManySlider = new BKSingleSlider("how many", 1, 100, 1, 1);
    howManySlider->setJustifyRight(false);
    addAndMakeVisible(howManySlider);
    
    clusterThreshSlider = new BKSingleSlider("cluster threshold", 20, 2000, 200, 10);
    clusterThreshSlider->setJustifyRight(false);
    addAndMakeVisible(clusterThreshSlider);
    
    clusterMinMaxSlider = new BKRangeSlider("cluster min/max", 1, 10, 3, 4, 1);
    clusterMinMaxSlider->setJustifyRight(false);
    addAndMakeVisible(clusterMinMaxSlider);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setJustifyRight(false);
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(gainSlider);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.setButtonText(" X ");

    
}

void SynchronicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void SynchronicViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    releaseVelocitySetsSynchronicToggle.setBounds(comboBoxSlice);
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    modeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
    offsetParamStartToggle.setBounds(modeSlice);
    
    int tempHeight = (area.getHeight() - paramSliders.size() * (gYSpacing + gPaddingConst * paddingScalarY)) / paramSliders.size();
    area.removeFromLeft(4 + 2.*gPaddingConst * paddingScalarX);
    area.removeFromRight(gXSpacing);
    for(int i = 0; i < paramSliders.size(); i++)
    {
        area.removeFromTop(gYSpacing + gPaddingConst * paddingScalarY);
        paramSliders[i]->setBounds(area.removeFromTop(tempHeight));
    }
    
    //leftColumn.reduce(4 + 2.*gPaddingConst * paddingScalarX, 0);
    leftColumn.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX - gComponentSingleSliderXOffset);
    //leftColumn.removeFromLeft(gXSpacing);
    
    int nextCenter = paramSliders[0]->getY() + paramSliders[0]->getHeight() / 2 + gPaddingConst * (1. - paddingScalarY) ;
    howManySlider->setBounds(leftColumn.getX(),
                             nextCenter - gComponentSingleSliderHeight/2.,
                             leftColumn.getWidth(),
                             gComponentSingleSliderHeight);
    
    nextCenter = paramSliders[1]->getY() + paramSliders[1]->getHeight() / 2 + gPaddingConst * (1. - paddingScalarY);
    clusterThreshSlider->setBounds(leftColumn.getX(),
                                   nextCenter - gComponentSingleSliderHeight/2.,
                                   leftColumn.getWidth(),
                                   gComponentSingleSliderHeight);
    
    nextCenter = paramSliders[2]->getY() + paramSliders[2]->getHeight() / 2 + gPaddingConst * (1. - paddingScalarY);
    clusterMinMaxSlider->setBounds(leftColumn.getX(),
                                   nextCenter - gComponentRangeSliderHeight/2.,
                                   leftColumn.getWidth(),
                                   gComponentRangeSliderHeight);
    
    nextCenter = paramSliders[3]->getY() + paramSliders[3]->getHeight() / 2 + gPaddingConst * (1. - paddingScalarY);
    gainSlider->setBounds(leftColumn.getX(),
                          nextCenter - gComponentSingleSliderHeight/2.,
                          leftColumn.getWidth(),
                          gComponentSingleSliderHeight);
    
    repaint();
    
}

//SynchronicSyncMode
void SynchronicViewController::fillModeSelectCB(void)
{
    
    modeSelectCB.clear(dontSendNotification);
    for (int i = 0; i < cSynchronicSyncModes.size(); i++)
    {
        String name = cSynchronicSyncModes[i];
        if (name != String::empty)  modeSelectCB.addItem(name, i+1);
        else                        modeSelectCB.addItem(String(i+1), i+1);
    }
    
    modeSelectCB.setSelectedItemIndex(0, NotificationType::dontSendNotification);
}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SynchronicPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

SynchronicPreparationEditor::SynchronicPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
SynchronicViewController(p, theGraph)
{
    fillSelectCB();
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    modeSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);
    hideOrShow.addListener(this);

    gainSlider->addMyListener(this);
    
    startTimer(20);
    
}

void SynchronicPreparationEditor::timerCallback()
{
    SynchronicProcessor::Ptr sProcessor = processor.gallery->getSynchronicProcessor(processor.updateState->currentSynchronicId);

    for (int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == "beat length multipliers")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getBeatMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "sustain length multipliers")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getLengthMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "accents")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getAccentMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "transpositions")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getTranspCounter());
        }
    }
}


void SynchronicPreparationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
  
    //DBG("multiSliderValueChanged called");
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        prep    ->setAccentMultiplier(whichSlider, values[0]);
        active  ->setAccentMultiplier(whichSlider, values[0]);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        prep    ->setBeatMultiplier(whichSlider, values[0]);
        active  ->setBeatMultiplier(whichSlider, values[0]);
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        prep    ->setLengthMultiplier(whichSlider, values[0]);
        active  ->setLengthMultiplier(whichSlider, values[0]);
    }
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        prep    ->setSingleTransposition(whichSlider, values);
        active  ->setSingleTransposition(whichSlider, values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;

}

void SynchronicPreparationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{

    //DBG("multiSliderALLValueChanged called");
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        prep    ->setAccentMultipliers(newvals);
        active  ->setAccentMultipliers(newvals);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        prep    ->setBeatMultipliers(newvals);
        active  ->setBeatMultipliers(newvals);
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        prep    ->setLengthMultipliers(newvals);
        active  ->setLengthMultipliers(newvals);
    }
    //pass original 2D array for transpositions
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        prep    ->setTransposition(values);
        active  ->setTransposition(values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;
}

void SynchronicPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "how many") {
        DBG("got new how many " + String(val));
        prep->setNumBeats(val);
        active->setNumBeats(val);
    }
    else if(name == "cluster threshold")
    {
        DBG("got new cluster threshold " + String(val));
        prep->setClusterThresh(val);
        active->setClusterThresh(val);
    }
    else if(name == "gain")
    {
        DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void SynchronicPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "cluster min/max") {
        DBG("got new cluster min/max " + String(minval) + " " + String(maxval));
        prep->setClusterMin(minval);
        prep->setClusterMax(maxval);
        active->setClusterMin(minval);
        active->setClusterMax(maxval);
    }
}

void SynchronicPreparationEditor::update(NotificationType notify)
{
    if (processor.updateState->currentSynchronicId < 0) return;
    
    
    fillSelectCB();
    
    SynchronicPreparation::Ptr prep   = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentSynchronicId, notify);
    modeSelectCB.setSelectedItemIndex(prep->getMode(), notify);
    offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
    releaseVelocitySetsSynchronicToggle.setToggleState(prep->getReleaseVelocitySetsSynchronic(), notify);
    howManySlider->setValue(prep->getNumBeats(), notify);
    clusterThreshSlider->setValue(prep->getClusterThreshMS(), notify);
    clusterMinMaxSlider->setMinValue(prep->getClusterMin(), notify);
    clusterMinMaxSlider->setMaxValue(prep->getClusterMax(), notify);
    gainSlider->setValue(prep->getGain(), notify);
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicAccentMultipliers])
        {
            paramSliders[i]->setTo(prep->getAccentMultipliers(), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicBeatMultipliers])
        {
            paramSliders[i]->setTo(prep->getBeatMultipliers(), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicLengthMultipliers])
        {
            paramSliders[i]->setTo(prep->getLengthMultipliers(), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicTranspOffsets])
        {
            paramSliders[i]->setTo(prep->getTransposition(), notify);
        }
    }
    
}

void SynchronicPreparationEditor::update()
{
    update(dontSendNotification);
}

void SynchronicPreparationEditor::fillSelectCB(void)
{
    // Direct menu
    Synchronic::PtrArr newpreps = processor.gallery->getAllSynchronic();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
        
        selectCB.setItemEnabled(i+1, true);
        Array<int> active = processor.updateState->active.getUnchecked(PreparationTypeSynchronic);
        if (active.contains(i) && i != processor.updateState->currentSynchronicId)
        {
            selectCB.setItemEnabled(i+1, false);
        }
    }
    
    selectCB.addItem("New synchronic...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentSynchronicId, NotificationType::dontSendNotification);
    
}


void SynchronicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Synchronic")
    {
        processor.updateState->currentSynchronicId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentSynchronicId == selectCB.getNumItems()-1)
        {
            processor.gallery->addSynchronic();
            
            fillSelectCB();
        }
        
        //update(sendNotification);
        update();
        
    }
    else if (name == "Mode")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep    ->setMode((SynchronicSyncMode) box->getSelectedItemIndex());
        active  ->setMode((SynchronicSyncMode) box->getSelectedItemIndex());
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(prep->getMode() == FirstNoteOnSync || prep->getMode() == AnyNoteOnSync)
        {
            prep->setBeatsToSkip(toggleVal - 1);
            active->setBeatsToSkip(toggleVal - 1);
        }
        else
        {
            prep->setBeatsToSkip(toggleVal);
            active->setBeatsToSkip(toggleVal);
        }
    }
}

void SynchronicPreparationEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    //SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == "Name")
    {
        processor.gallery->getSynchronic(processor.updateState->currentSynchronicId)->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
}

void SynchronicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getSynchronic(processor.updateState->currentSynchronicId)->setName(name);
}

void SynchronicPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        update();
    }
}

void SynchronicPreparationEditor::buttonClicked (Button* b)
{
    if (b == &offsetParamStartToggle)
    {
        
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(prep->getMode() == FirstNoteOnSync || prep->getMode() == AnyNoteOnSync)
        {
            prep->setBeatsToSkip(toggleVal - 1);
            active->setBeatsToSkip(toggleVal - 1);
        }
        else
        {
            prep->setBeatsToSkip(toggleVal);
            active->setBeatsToSkip(toggleVal);
        }

    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep->setReleaseVelocitySetsSynchronic(releaseVelocitySetsSynchronicToggle.getToggleState());
        active->setReleaseVelocitySetsSynchronic(releaseVelocitySetsSynchronicToggle.getToggleState());
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}



// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SynchronicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
SynchronicModificationEditor::SynchronicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
SynchronicViewController(p, theGraph)
{
    fillSelectCB();
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    modeSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);
    hideOrShow.addListener(this);
    gainSlider->addMyListener(this);
    
    startTimer(20);
    
}

void SynchronicModificationEditor::timerCallback()
{
    /*
    SynchronicProcessor::Ptr sProcessor = processor.gallery->getSynchronicProcessor(processor.updateState->currentSynchronicId);
    
    for (int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == "beat length multipliers")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getBeatMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "sustain length multipliers")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getLengthMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "accents")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getAccentMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "transpositions")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getTranspCounter());
        }
    }
     */
}

void SynchronicModificationEditor::update(NotificationType notify)
{
    if (processor.updateState->currentModSynchronicId < 0) return;
    
    fillSelectCB();
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModSynchronicId, notify);
    
    SynchronicPreparation::Ptr prep   = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    
    String val = mod->getParam(SynchronicMode);
    modeSelectCB.setSelectedItemIndex(val.getIntValue(), notify);
    
    //FIXIT offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
    
    val = mod->getParam(SynchronicNumPulses);
    howManySlider->setValue(val.getIntValue(), notify);
    
    val = mod->getParam(SynchronicClusterThresh);
    clusterThreshSlider->setValue(val.getFloatValue(), notify);
    
    val = mod->getParam(SynchronicClusterMin);
    clusterMinMaxSlider->setMinValue(val.getIntValue(), notify);
    
    val = mod->getParam(SynchronicClusterMax);
    clusterMinMaxSlider->setMaxValue(val.getIntValue(), notify);
    
    

    // FIXIT gainSlider->setValue(prep->getGain(), notify);
    
    for (int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicAccentMultipliers])
        {
            val = mod->getParam(SynchronicAccentMultipliers);
            paramSliders[i]->setTo(stringToFloatArray(val), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicBeatMultipliers])
        {
            val = mod->getParam(SynchronicBeatMultipliers);
            paramSliders[i]->setTo(stringToFloatArray(val), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicLengthMultipliers])
        {
            val = mod->getParam(SynchronicLengthMultipliers);
            paramSliders[i]->setTo(stringToFloatArray(val), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicTranspOffsets])
        {
            val = mod->getParam(SynchronicTranspOffsets);
            paramSliders[i]->setTo(stringToArrayFloatArray(val), notify);
        }
    }
    
}

void SynchronicModificationEditor::update()
{
    update(dontSendNotification);
}


void SynchronicModificationEditor::fillSelectCB(void)
{
    // Direct menu
    StringArray mods = processor.gallery->getAllSynchronicModNames();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < mods.size(); i++)
    {
        String name = mods[i];
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New synchronic modification...", mods.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModSynchronicId, NotificationType::dontSendNotification);
    
}

void SynchronicModificationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        mod->setParam(SynchronicAccentMultipliers, String(values[0]));
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        mod->setParam(SynchronicBeatMultipliers, String(values[0]));
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        mod->setParam(SynchronicLengthMultipliers, String(values[0]));
    }
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        mod->setParam(SynchronicTranspOffsets, floatArrayToString(values));
    }
    
    updateModification();
    
}

void SynchronicModificationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        mod->setParam(SynchronicAccentMultipliers, floatArrayToString(newvals));
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        mod->setParam(SynchronicBeatMultipliers, floatArrayToString(newvals));
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        mod->setParam(SynchronicLengthMultipliers, floatArrayToString(newvals));
    }
    //pass original 2D array for transpositions
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        mod->setParam(SynchronicLengthMultipliers, arrayFloatArrayToString(values));
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    GeneralSettings::Ptr genmod = processor.gallery->getGeneralSettings();
    
    if(name == "how many")
    {
        mod->setParam(SynchronicNumPulses, String(val));
    }
    else if(name == "cluster threshold")
    {
        mod->setParam(SynchronicClusterThresh, String(val));
    }
    else if(name == "gain")
    {
        // gain mod
        //genmod->setSynchronicGain(val);
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    if(name == "cluster min/max")
    {
        mod->setParam(SynchronicClusterMin, String(minval));
        mod->setParam(SynchronicClusterMax, String(maxval));
    }
    
    updateModification();
}



void SynchronicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Synchronic")
    {
        processor.updateState->currentModSynchronicId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentModSynchronicId == selectCB.getNumItems()-1)
        {
            processor.gallery->addSynchronicMod();
            
            fillSelectCB();
        }

        update();
        
        return;
        
    }
    
    if (name == "Mode")
    {
        SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
        
        mod->setParam(SynchronicMode, String(box->getSelectedItemIndex()));
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue()) == FirstNoteOnSync ||
           ((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue())  == AnyNoteOnSync)
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal - 1));
        }
        else
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal));
        }
    }
    
    updateModification();
}

void SynchronicModificationEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    if (name == "Name")
    {
        processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId)->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getSynchronicModPreparation(processor.updateState->currentSynchronicId)->setName(name);
    updateModification();
}

void SynchronicModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        update();
    }
    
    
}

void SynchronicModificationEditor::buttonClicked (Button* b)
{
    if (b == &offsetParamStartToggle)
    {
        SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue()) == FirstNoteOnSync ||
           ((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue())  == AnyNoteOnSync)
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal - 1));
        }
        else
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal));
        }
        
    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        // this for MODS
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    
    updateModification();
}

void SynchronicModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}



