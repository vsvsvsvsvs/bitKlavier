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
    selectCB.setTooltip("Select from available saved preparation settings");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    lengthModeSelectCB.setName("Length Mode");
    lengthModeSelectCB.setTooltip("Indicates how Nostalgic calculates length of reverse wave");
    lengthModeSelectCB.addSeparator();
    lengthModeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    lengthModeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(lengthModeSelectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    transpositionSlider->setTooltip("Determines pitch (in semitones) of Nostalgic notes; control-click to add another voice, double-click to edit all");
    addAndMakeVisible(transpositionSlider);
    
    lengthMultiplierSlider = new BKSingleSlider("note length multiplier", 0, 10, 1, 0.01);
    lengthMultiplierSlider->setToolTipString("Changes length of Nostalgic wave as a factor of note duration");
    lengthMultiplierSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(lengthMultiplierSlider);
    
    beatsToSkipSlider = new BKSingleSlider("beats to skip", 0, 10, 0, 1);
    beatsToSkipSlider->setToolTipString("Indicates how long Nostalgic wave lasts with respect to linked Synchronic sequence");
    addAndMakeVisible(beatsToSkipSlider);
    beatsToSkipSlider->setVisible(false);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setToolTipString("Volume multiplier for Nostalgic notes");
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(gainSlider);
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    nDisplaySlider.setWaveDistanceTooltip("Determines endpoint of Nostalgic wave with respect to waveform");
    nDisplaySlider.setUndertowTooltip("Determines total length of Undertow beginning at wave endpoint");
    addAndMakeVisible(nDisplaySlider);
    
    reverseADSRSlider = new BKADSRSlider("reverseEnvelope");
    reverseADSRSlider->setButtonText("edit reverse envelope");
    reverseADSRSlider->setToolTip("ADSR settings for Nostalgic wave");
    addAndMakeVisible(reverseADSRSlider);
    
    undertowADSRSlider = new BKADSRSlider("undertowEnvelope");
    undertowADSRSlider->setButtonText("edit undertow envelope");
    undertowADSRSlider->setToolTip("ADSR settings for Undertow");
    addAndMakeVisible(undertowADSRSlider);
    
#if JUCE_IOS
    beatsToSkipSlider->addWantsBigOneListener(this);
    gainSlider->addWantsBigOneListener(this);
    lengthMultiplierSlider->addWantsBigOneListener(this);
    transpositionSlider->addWantsBigOneListener(this);
    nDisplaySlider.addWantsBigOneListener(this);
#endif
    
    showADSR = false;
    showReverseADSR = false;
    showUndertowADSR = false;
    
}

void NostalgicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void NostalgicViewController::setShowADSR(String name, bool newval)
{
    showADSR = newval;
    
    if(showADSR)
    {
        beatsToSkipSlider->setVisible(false);
        gainSlider->setVisible(false);
        lengthMultiplierSlider->setVisible(false);
        transpositionSlider->setVisible(false);
        nDisplaySlider.setVisible(false);
        lengthModeSelectCB.setVisible(false);
        
        if(name == reverseADSRSlider->getName()) {
            reverseADSRSlider->setButtonText("close  reverse envelope");
            undertowADSRSlider->setVisible(false);
            
            showReverseADSR = true;
            showUndertowADSR = false;
        }
        else if(name == undertowADSRSlider->getName()) {
            undertowADSRSlider->setButtonText("close undertow envelope");
            reverseADSRSlider->setVisible(false);
            
            showReverseADSR = false;
            showUndertowADSR = true;
        }
    }
    else
    {
        if(lengthModeSelectCB.getSelectedId() == 0) lengthMultiplierSlider->setVisible(true);
        else beatsToSkipSlider->setVisible(true);
        
        gainSlider->setVisible(true);
        transpositionSlider->setVisible(true);
        nDisplaySlider.setVisible(true);
        lengthModeSelectCB.setVisible(true);
        
        reverseADSRSlider->setVisible(true);
        undertowADSRSlider->setVisible(true);
        
        if(name == reverseADSRSlider->getName()) reverseADSRSlider->setButtonText("edit reverse envelope");
        else if(name == undertowADSRSlider->getName()) undertowADSRSlider->setButtonText("edit undertow envelope");
    }
    
    resized();
}

void NostalgicViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> areaSave = area;
    
    Rectangle<int> nDisplayRow = area.removeFromBottom(100 + 80 * processor.paddingScalarY);
    nDisplayRow.reduce(0, 4);
    nDisplayRow.removeFromLeft(gXSpacing + gPaddingConst * processor.paddingScalarX * 0.5);
    nDisplayRow.removeFromRight(gXSpacing + gPaddingConst * processor.paddingScalarX * 0.5);
    nDisplaySlider.setBounds(nDisplayRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);

    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    if(!showADSR)
    {
        Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
        modeSlice.removeFromRight(gXSpacing);
        //modeSlice.reduce(4 + 2.*gPaddingConst * processor.paddingScalarX, 0);
        //lengthModeSelectCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
        lengthModeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
        
        Rectangle<int> sliderSlice = area;
        sliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        //sliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
        /*
         sliderSlice.reduce(4 + 2.*gPaddingConst * processor.paddingScalarX,
         4 + 2.*gPaddingConst * processor.paddingScalarY);
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
        leftColumn.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        leftColumn.removeFromLeft(gXSpacing);
        transpositionSlider->setBounds(leftColumn.getX(),
                                       lengthMultiplierSlider->getY(),
                                       leftColumn.getWidth(),
                                       gComponentStackedSliderHeight + processor.paddingScalarY * 30);
        
        reverseADSRSlider->setBounds(leftColumn.getX(),
                                      gainSlider->getY() + gComponentComboBoxHeight * 0.5,
                                      leftColumn.getWidth() * 0.5 - gYSpacing,
                                      gComponentComboBoxHeight);
        
        undertowADSRSlider->setBounds(leftColumn.getX() + leftColumn.getWidth() * 0.5 + gYSpacing,
                                     gainSlider->getY() + gComponentComboBoxHeight * 0.5,
                                     leftColumn.getWidth() * 0.5 - gYSpacing,
                                     gComponentComboBoxHeight);
    }
    else
    {
        areaSave.removeFromTop(gYSpacing * 2 + gYSpacing + 8.*gPaddingConst * processor.paddingScalarY);
         Rectangle<int> adsrSliderSlice = areaSave.removeFromTop(gComponentComboBoxHeight * 2 + gComponentSingleSliderHeight * 2 + gYSpacing * 3);
        if(showReverseADSR) reverseADSRSlider->setBounds(adsrSliderSlice);
        else undertowADSRSlider->setBounds(adsrSliderSlice);
        
        selectCB.toFront(false);
    }
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

#if JUCE_IOS
void NostalgicViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

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
    
    reverseADSRSlider->addMyListener(this);
    undertowADSRSlider->addMyListener(this);
    
    startTimer(20);
}

void NostalgicPreparationEditor::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
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
    
    nostalgic->setName(name);
}

void NostalgicPreparationEditor::update(void)
{
    if (processor.updateState->currentNostalgicId < 0) return;
    setShowADSR(reverseADSRSlider->getName(), false);
    reverseADSRSlider->setIsButtonOnly(true);
    setShowADSR(undertowADSRSlider->getName(), false);
    undertowADSRSlider->setIsButtonOnly(true);
    setSubWindowInFront(false);
    
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if (prep != nullptr)
    {
        nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
        nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);
        
        selectCB.setSelectedId(processor.updateState->currentNostalgicId, dontSendNotification);
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
        
        reverseADSRSlider->setAttackValue(prep->getReverseAttack(), dontSendNotification);
        reverseADSRSlider->setDecayValue(prep->getReverseDecay(), dontSendNotification);
        reverseADSRSlider->setSustainValue(prep->getReverseSustain(), dontSendNotification);
        reverseADSRSlider->setReleaseValue(prep->getReverseRelease(), dontSendNotification);
        
        undertowADSRSlider->setAttackValue(prep->getUndertowAttack(), dontSendNotification);
        undertowADSRSlider->setDecayValue(prep->getUndertowDecay(), dontSendNotification);
        undertowADSRSlider->setSustainValue(prep->getUndertowSustain(), dontSendNotification);
        undertowADSRSlider->setReleaseValue(prep->getUndertowRelease(), dontSendNotification);
       
    }
    
}

void NostalgicPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        update();
    }
}

int NostalgicPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeNostalgic);
    
    return processor.gallery->getAllNostalgic().getLast()->getId();
}

int NostalgicPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeNostalgic, processor.updateState->currentNostalgicId);
    
    return processor.gallery->getAllNostalgic().getLast()->getId();
}

void NostalgicPreparationEditor::deleteCurrent(void)
{
    int NostalgicId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeNostalgic, NostalgicId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentNostalgicId = -1;
}

void NostalgicPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentNostalgicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void NostalgicPreparationEditor::actionButtonCallback(int action, NostalgicPreparationEditor* vc)
{
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
    }
    else if (action == 4)
    {
        processor.reset(PreparationTypeNostalgic, processor.updateState->currentNostalgicId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeNostalgic, processor.updateState->currentNostalgicId);
        vc->update();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentNostalgicId;
        Nostalgic::Ptr prep = processor.gallery->getNostalgic(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
        }
        
        vc->update();
    }
}

void NostalgicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    int index = box->getSelectedItemIndex();
    
    if (name == "Nostalgic")
    {
        setCurrentId(Id);
    }
    else if (name == "Length Mode")
    {
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

void NostalgicPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
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

void NostalgicPreparationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    DBG("received ADSR slider " + name);
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if(name == reverseADSRSlider->getName())
    {
        prep->setReverseAttack(attack);
        prep->setReverseDecay(decay);
        prep->setReverseSustain(sustain);
        prep->setReverseRelease(release);
        active->setReverseAttack(attack);
        active->setReverseDecay(decay);
        active->setReverseSustain(sustain);
        active->setReverseRelease(release);
    }
    else if(name == undertowADSRSlider->getName())
    {
        prep->setUndertowAttack(attack);
        prep->setUndertowDecay(decay);
        prep->setUndertowSustain(sustain);
        prep->setUndertowRelease(release);
        active->setUndertowAttack(attack);
        active->setUndertowDecay(decay);
        active->setUndertowSustain(sustain);
        active->setUndertowRelease(release);
    }

}

void NostalgicPreparationEditor::closeSubWindow()
{
    reverseADSRSlider->setIsButtonOnly(true);
    undertowADSRSlider->setIsButtonOnly(true);
    setShowADSR(reverseADSRSlider->getName(), false);
    setShowADSR(undertowADSRSlider->getName(), false);
    setSubWindowInFront(false);
}


void NostalgicPreparationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    //DBG("BKADSRButtonStateChanged " + name);
    setShowADSR(name, !state);
    setSubWindowInFront(!state);
}

void NostalgicPreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
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
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
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
    
    lastId = selectedId;
    
}

void NostalgicPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayNostalgic)
    {
        NostalgicProcessor::Ptr nProcessor = processor.currentPiano->getNostalgicProcessor(processor.updateState->currentNostalgicId);
        
        if (nProcessor != nullptr)
        {
            Array<int> currentPlayPositions = nProcessor->getPlayPositions();
            Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
            currentPlayPositions.addArray(currentUndertowPositions);
            
            nDisplaySlider.updateSliderPositions(currentPlayPositions);
        }
    }
}


void NostalgicPreparationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
        
        reverseADSRSlider->setIsButtonOnly(true);
        undertowADSRSlider->setIsButtonOnly(true);
        setShowADSR(reverseADSRSlider->getName(), false);
        setShowADSR(undertowADSRSlider->getName(), false);
        setSubWindowInFront(false);
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NostalgicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

NostalgicModificationEditor::NostalgicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
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
    
    reverseADSRSlider->addMyListener(this);
    undertowADSRSlider->addMyListener(this);
    
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
    reverseADSRSlider->setDim(gModAlpha);
    undertowADSRSlider->setDim(gModAlpha);
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
    if(mod->getParam(NostalgicReverseADSR) != "")       reverseADSRSlider->setBright();
    if(mod->getParam(NostalgicUndertowADSR) != "")      undertowADSRSlider->setBright();
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
        
        selectCB.setSelectedId(processor.updateState->currentModNostalgicId, dontSendNotification);
        
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
        transpositionSlider->setTo(stringToFloatArray(val), dontSendNotification);
        
        val = mod->getParam(NostalgicLengthMultiplier);
        lengthMultiplierSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(NostalgicBeatsToSkip);
        beatsToSkipSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(NostalgicGain);
        gainSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(NostalgicReverseADSR);
        reverseADSRSlider->setValue(stringToFloatArray(val), dontSendNotification);
        
        val = mod->getParam(NostalgicUndertowADSR);
        undertowADSRSlider->setValue(stringToFloatArray(val), dontSendNotification);
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

    lastId = selectedId;
}

void NostalgicModificationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayNostalgicMod)
    {
    /*
     NostalgicProcessor::Ptr nProcessor = processor.currentPiano->getNostalgicProcessor(processor.updateState->currentNostalgicId);
     
     Array<int> currentPlayPositions = nProcessor->getPlayPositions();
     Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
     currentPlayPositions.addArray(currentUndertowPositions);
     
     nDisplaySlider.updateSliderPositions(currentPlayPositions);
     */
    }
    
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

int NostalgicModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeNostalgicMod);
    
    return processor.gallery->getNostalgicModPreparations().getLast()->getId();
}

int NostalgicModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeNostalgicMod, processor.updateState->currentModNostalgicId);
    
    return processor.gallery->getNostalgicModPreparations().getLast()->getId();
}

void NostalgicModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeNostalgicMod, oldId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentModNostalgicId = -1;
}

void NostalgicModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModNostalgicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void NostalgicModificationEditor::actionButtonCallback(int action, NostalgicModificationEditor* vc)
{
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeNostalgicMod, processor.updateState->currentModNostalgicId);
        vc->update();
        vc->updateModification();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModNostalgicId;
        NostalgicModPreparation::Ptr prep = processor.gallery->getNostalgicModPreparation(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
        }
        
        vc->update();
    }
}

void NostalgicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    int index = box->getSelectedItemIndex();
    
    if (name == "Nostalgic")
    {
        setCurrentId(Id);
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

void NostalgicModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
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
    processor.updateState->modificationDidChange = true;
}

void NostalgicModificationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getModOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
}

void NostalgicModificationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    Array<float> newvals = {(float)attack, (float)decay, sustain, (float)release};
    if(name == reverseADSRSlider->getName())
    {
        mod->setParam(NostalgicReverseADSR, floatArrayToString(newvals));
        reverseADSRSlider->setBright();
    }
    else if(name == undertowADSRSlider->getName())
    {
        mod->setParam(NostalgicUndertowADSR, floatArrayToString(newvals));
        undertowADSRSlider->setBright();
    }

    
    updateModification();
}

void NostalgicModificationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    setShowADSR(name, !state);
    setSubWindowInFront(!state);
}
