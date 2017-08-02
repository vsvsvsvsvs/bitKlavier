/*
  ==============================================================================

    NostalgicViewController.cpp
    Created: 24 May 2017 7:13:23pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "NostalgicViewController.h"

NostalgicViewController::NostalgicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::nostalgic_icon_png, BinaryData::nostalgic_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Nostalgic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    lengthModeSelectCB.setName("Length Mode");
    lengthModeSelectCB.addSeparator();
    lengthModeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    lengthModeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(lengthModeSelectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    addAndMakeVisible(transpositionSlider);
    
    lengthMultiplierSlider = new BKSingleSlider("note length multiplier", 0, 10, 1, 0.01);
    lengthMultiplierSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(lengthMultiplierSlider);
    
    beatsToSkipSlider = new BKSingleSlider("beats to skip", 0, 10, 0, 1);
    addAndMakeVisible(beatsToSkipSlider);
    beatsToSkipSlider->setVisible(false);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(gainSlider);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.setButtonText(" X ");
    
    addAndMakeVisible(nDisplaySlider);
}

void NostalgicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void NostalgicViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    Rectangle<int> nDisplayRow = area.removeFromBottom(100 + 80 * paddingScalarY);
    nDisplayRow.reduce(0, 4);
    nDisplayRow.removeFromLeft(gXSpacing + gPaddingConst * paddingScalarX * 0.5);
    nDisplayRow.removeFromRight(gXSpacing + gPaddingConst * paddingScalarX * 0.5);
    nDisplaySlider.setBounds(nDisplayRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    clearModsButton.setBounds(comboBoxSlice.removeFromLeft(90));
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    //modeSlice.reduce(4 + 2.*gPaddingConst * paddingScalarX, 0);
    //lengthModeSelectCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    lengthModeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
    
    Rectangle<int> sliderSlice = area;
    sliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    //sliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    /*
     sliderSlice.reduce(4 + 2.*gPaddingConst * paddingScalarX,
     4 + 2.*gPaddingConst * paddingScalarY);
     */
    
    int nextCenter = sliderSlice.getY() + sliderSlice.getHeight() / 4.;
    lengthMultiplierSlider->setBounds(sliderSlice.getX(),
                                      nextCenter - gComponentSingleSliderHeight/2 + 8,
                                      sliderSlice.getWidth(),
                                      gComponentSingleSliderHeight);
    beatsToSkipSlider->setBounds(lengthMultiplierSlider->getBounds());
    
    nextCenter = sliderSlice.getY() + 3. * sliderSlice.getHeight() / 4.;
    gainSlider->setBounds(sliderSlice.getX(),
                          nextCenter - gComponentSingleSliderHeight/2 + 4,
                          sliderSlice.getWidth(),
                          gComponentSingleSliderHeight);
    
    //leftColumn.reduce(4, 0);
    leftColumn.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    leftColumn.removeFromLeft(gXSpacing);
    transpositionSlider->setBounds(leftColumn.getX(),
                                   lengthMultiplierSlider->getY(),
                                   leftColumn.getWidth(),
                                   gComponentStackedSliderHeight + paddingScalarY * 30);
}

void NostalgicViewController::fillModeSelectCB(void)
{
    
    lengthModeSelectCB.clear(dontSendNotification);
    for (int i = 0; i < cNostalgicSyncModes.size(); i++)
    {
        String name = cNostalgicSyncModes[i];
        if (name != String::empty)  lengthModeSelectCB.addItem(name, i+1);
        else                        lengthModeSelectCB.addItem(String(i+1), i+1);
    }
    
    lengthModeSelectCB.setSelectedItemIndex(0, NotificationType::dontSendNotification);
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NostalgicPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

NostalgicPreparationEditor::NostalgicPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
NostalgicViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    nDisplaySlider.addMyListener(this);
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    lengthModeSelectCB.addListener(this);
    transpositionSlider->addMyListener(this);
    lengthMultiplierSlider->addMyListener(this);
    beatsToSkipSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    hideOrShow.addListener(this);
    
    startTimer(20);
}

void NostalgicPreparationEditor::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
    Nostalgic::Ptr nostalgic = processor.gallery->getNostalgic(processor.updateState->currentNostalgicId);
    nostalgic->editted = true;
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    prep    ->setWaveDistance(wavedist);
    active  ->setWaveDistance(wavedist);
    prep    ->setUndertow(undertow);
    active  ->setUndertow(undertow);
    
}

void NostalgicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Nostalgic::Ptr nostalgic = processor.gallery->getNostalgic(processor.updateState->currentNostalgicId);
    nostalgic->editted = true;
    
    nostalgic->setName(name);
}

void NostalgicPreparationEditor::update(void)
{
    if (processor.updateState->currentNostalgicId < 0) return;
    
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if (prep != nullptr)
    {
        nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
        nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);
        
        selectCB.setSelectedItemIndex(processor.updateState->currentNostalgicId, dontSendNotification);
        lengthModeSelectCB.setSelectedItemIndex(prep->getMode(), dontSendNotification);
        
        //transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
        transpositionSlider->setTo(prep->getTransposition(), dontSendNotification);
        lengthMultiplierSlider->setValue(prep->getLengthMultiplier(), dontSendNotification);
        beatsToSkipSlider->setValue(prep->getBeatsToSkip(), dontSendNotification);
        gainSlider->setValue(prep->getGain(), dontSendNotification);
        
        if(prep->getMode() == NoteLengthSync)
        {
            lengthMultiplierSlider->setVisible(true);
            beatsToSkipSlider->setVisible(false);
        }
        else
        {
            lengthMultiplierSlider->setVisible(false);
            beatsToSkipSlider->setVisible(true);
        }

    }
    
}

void NostalgicPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        update();
    }
}

void NostalgicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    int index = box->getSelectedItemIndex();
    
    if (name == "Nostalgic")
    {
        if (Id == selectCB.getNumItems()-1)
        {
            processor.gallery->addNostalgic();
            
            Id = processor.gallery->getAllNostalgic().getLast()->getId();
        }
        
        processor.updateState->currentNostalgicId = Id;
        
        processor.updateState->idDidChange = true;
        
        update();
        
        fillSelectCB(lastId, Id);
        
        lastId = Id;
    }
    else if (name == "Length Mode")
    {
        Nostalgic::Ptr nostalgic = processor.gallery->getNostalgic(processor.updateState->currentNostalgicId);
        nostalgic->editted = true;
        
        NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
        NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
        
        prep    ->setMode((NostalgicSyncMode) index);
        active  ->setMode((NostalgicSyncMode) index);
        
        if(prep->getMode() == NoteLengthSync)
        {
            lengthMultiplierSlider->setVisible(true);
            beatsToSkipSlider->setVisible(false);
        }
        else
        {
            lengthMultiplierSlider->setVisible(false);
            beatsToSkipSlider->setVisible(true);
        }

    }
}

void NostalgicPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    Nostalgic::Ptr nostalgic = processor.gallery->getNostalgic(processor.updateState->currentNostalgicId);
    nostalgic->editted = true;
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if(name == "note length multiplier")
    {
        //DBG("note length multiplier " + String(val));
        prep->setLengthMultiplier(val);
        active->setLengthMultiplier(val);
    }
    else if(name == "beats to skip")
    {
        //DBG("beats to skip " + String(val));
        prep->setBeatsToSkip(val);
        active->setBeatsToSkip(val);
    }
    else if(name == "gain")
    {
        //DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void NostalgicPreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    Nostalgic::Ptr nostalgic = processor.gallery->getNostalgic(processor.updateState->currentNostalgicId);
    nostalgic->editted = true;
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);

    prep->setTransposition(val);
    active->setTransposition(val);
}

void NostalgicPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllNostalgic())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Nostalgic"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeNostalgic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentNostalgicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    selectCB.addSeparator();
    selectCB.addItem("New Nostalgic...", -1);
    
    lastId = selectedId;
    
}

void NostalgicPreparationEditor::timerCallback()
{
    NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
    
    Array<int> currentPlayPositions = nProcessor->getPlayPositions();
    Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
    currentPlayPositions.addArray(currentUndertowPositions);
    
    nDisplaySlider.updateSliderPositions(currentPlayPositions);
    
}


void NostalgicPreparationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    
    Nostalgic::Ptr nostalgic = processor.gallery->getNostalgic(processor.updateState->currentNostalgicId);
    nostalgic->editted = true;
}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NostalgicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

NostalgicModificationEditor::NostalgicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
NostalgicViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    clearModsButton.setButtonText("clear mods");
    addAndMakeVisible(clearModsButton);
    clearModsButton.addListener(this);
    
    nDisplaySlider.addMyListener(this);
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    lengthModeSelectCB.addListener(this);
    transpositionSlider->addMyListener(this);
    lengthMultiplierSlider->addMyListener(this);
    beatsToSkipSlider->addMyListener(this);
    hideOrShow.addListener(this);
    
    gainSlider->addMyListener(this);
    
    //startTimer(20);
}

void NostalgicModificationEditor::greyOutAllComponents()
{
    nDisplaySlider.setDim(gModAlpha);
    lengthModeSelectCB.setAlpha(gModAlpha);
    transpositionSlider->setDim(gModAlpha);
    lengthMultiplierSlider->setDim(gModAlpha);
    beatsToSkipSlider->setDim(gModAlpha);
    gainSlider->setDim(gModAlpha);
}

void NostalgicModificationEditor::highlightModedComponents()
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);

    if(mod->getParam(NostalgicWaveDistance) != "")      nDisplaySlider.setBright();
    if(mod->getParam(NostalgicUndertow) != "")          nDisplaySlider.setBright();
    if(mod->getParam(NostalgicTransposition) != "")     transpositionSlider->setBright();
    if(mod->getParam(NostalgicLengthMultiplier) != "")  lengthMultiplierSlider->setBright();
    if(mod->getParam(NostalgicBeatsToSkip) != "")       beatsToSkipSlider->setBright();
    if(mod->getParam(NostalgicMode) != "")              lengthModeSelectCB.setAlpha(1.);
    if(mod->getParam(NostalgicGain) != "")              gainSlider->setBright();
}

void NostalgicModificationEditor::update(void)
{
    if (processor.updateState->currentModNostalgicId < 0) return;
    
    
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    if (mod != nullptr)
    {
        fillSelectCB(-1,-1);
        
        greyOutAllComponents();
        highlightModedComponents();
        
        selectCB.setSelectedItemIndex(processor.updateState->currentModNostalgicId, dontSendNotification);
        
        String val = mod->getParam(NostalgicWaveDistance);
        nDisplaySlider.setWaveDistance(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(NostalgicUndertow);
        nDisplaySlider.setUndertow(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(NostalgicMode);
        NostalgicSyncMode mode = (NostalgicSyncMode) val.getIntValue();
        lengthModeSelectCB.setSelectedItemIndex(mode, dontSendNotification);
        
        if(mode == NoteLengthSync)
        {
            lengthMultiplierSlider->setVisible(true);
            beatsToSkipSlider->setVisible(false);
        }
        else
        {
            lengthMultiplierSlider->setVisible(false);
            beatsToSkipSlider->setVisible(true);
        }
        
        val = mod->getParam(NostalgicTransposition);
        //transpositionSlider->setValue(stringToFloatArray(val), dontSendNotification);
        transpositionSlider->setTo(stringToFloatArray(val), dontSendNotification);
        
        val = mod->getParam(NostalgicLengthMultiplier);
        lengthMultiplierSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(NostalgicBeatsToSkip);
        beatsToSkipSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(NostalgicGain);
        gainSlider->setValue(val.getFloatValue(), dontSendNotification);
    }
    
    
}


void NostalgicModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getNostalgicModPreparations())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("NostalgicMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeNostalgic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentNostalgicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    selectCB.addSeparator();
    selectCB.addItem("New Nostalgic Mod...", -1);
    
    lastId = selectedId;
}

void NostalgicModificationEditor::timerCallback()
{
    /*
     NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
     
     Array<int> currentPlayPositions = nProcessor->getPlayPositions();
     Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
     currentPlayPositions.addArray(currentUndertowPositions);
     
     nDisplaySlider.updateSliderPositions(currentPlayPositions);
     */
    
}

void NostalgicModificationEditor::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    mod     ->setParam(NostalgicWaveDistance, String(wavedist));
    mod     ->setParam(NostalgicUndertow, String(undertow));
    
    nDisplaySlider.setBright();
    
    updateModification();
}

void NostalgicModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    mod->setName(name);
    
    updateModification();
}

void NostalgicModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        update();
    }
}

void NostalgicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    int index = box->getSelectedItemIndex();
    
    if (name == "Nostalgic")
    {
        if (Id == -1)
        {
            processor.gallery->addNostalgicMod();
            
            Id = processor.gallery->getNostalgicModPreparations().getLast()->getId();
        }
        
        processor.updateState->currentModNostalgicId = Id;
        
        processor.updateState->idDidChange = true;
        
        update();
        
        fillSelectCB(lastId, Id);
        
        lastId = Id;
    }
    else if (name == "Length Mode")
    {
        NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
        
        NostalgicSyncMode mode = (NostalgicSyncMode) index;
        
        mod->setParam(NostalgicMode, String(mode));
        
        if(mode == NoteLengthSync)
        {
            lengthMultiplierSlider->setVisible(true);
            beatsToSkipSlider->setVisible(false);
        }
        else
        {
            lengthMultiplierSlider->setVisible(false);
            beatsToSkipSlider->setVisible(true);
        }
        
        updateModification();
        lengthModeSelectCB.setAlpha(1.);
        
    }
    
    
}

void NostalgicModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    if(name == "note length multiplier")
    {
        mod->setParam(NostalgicLengthMultiplier, String(val));
        lengthMultiplierSlider->setBright();
    }
    else if(name == "beats to skip")
    {
        mod->setParam(NostalgicBeatsToSkip, String(val));
        beatsToSkipSlider->setBright();
    }
    else if(name == "gain")
    {
        mod->setParam(NostalgicGain, String(val));
        gainSlider->setBright();
    }
    
    updateModification();
}

void NostalgicModificationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    mod->setParam(NostalgicTransposition, floatArrayToString(val));
    transpositionSlider->setBright();
    
    updateModification();
}

void NostalgicModificationEditor::updateModification(void)
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    mod->editted = true;
    
    processor.updateState->modificationDidChange = true;
}

void NostalgicModificationEditor::buttonClicked (Button* b)
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    mod->editted = true;
    
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &clearModsButton)
    {
        NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
        mod->clearAll();
        update();
    }
}
