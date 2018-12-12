/*
  ==============================================================================

    TuningViewController.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController.h"

TuningViewController::TuningViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p,theGraph),
showSprings(false)
#if JUCE_IOS
, absoluteKeyboard(true)
#endif
{
    setLookAndFeel(&buttonsAndMenusLAF);

    //setRepaintsOnMouseActivity(false);
    
    for (int i = 0; i < 128; i++)
    {
        Slider* s = new Slider("t" + String(i));
        
        s->setSliderStyle(Slider::SliderStyle::LinearBar);
        s->setRange(0.0, 1.0);
        addChildComponent(s);
        tetherSliders.add(s);
        
        Label* l = new Label(s->getName(), Utilities::getNoteString(i));
        l->setColour(juce::Label::ColourIds::textColourId, Colours::white);
        addChildComponent(l);
        tetherLabels.add(l);
    }
    
    for (int i = 0; i < 12; i++)
    {
        Slider* s = new Slider(intervalNames[i+1]);
        
        s->setSliderStyle(Slider::SliderStyle::LinearBar);
        s->setRange(0.0, 1.0);
        addChildComponent(s);
        springSliders.add(s);
        
        Label* l = new Label(s->getName(), s->getName());
        l->setColour(juce::Label::ColourIds::textColourId, Colours::white);
        addChildComponent(l);
        springLabels.add(l);
    
    }
    
    
    springTuningToggle.setColour(ToggleButton::textColourId, Colours::antiquewhite);
    springTuningToggle.setColour(TextButton::buttonOnColourId, Colours::red.withMultipliedAlpha(0.5));
    springTuningToggle.setColour(ToggleButton::tickDisabledColourId, Colours::antiquewhite);
    springTuningToggle.setClickingTogglesState(true);
    springTuningToggle.setTooltip("Turns on/off Spring Tuning.");
    springTuningToggle.setAlwaysOnTop(true);
    springTuningToggle.setButtonText("Springs");
    addChildComponent(springTuningToggle);
    
    springTuningLabel.setText("Springs", dontSendNotification);
    springTuningLabel.setJustificationType(Justification::left);
    springTuningLabel.setColour(Label::ColourIds::textColourId, Colours::antiquewhite);
    
    rateSliderLabel.setText("rate", dontSendNotification);
    rateSliderLabel.setColour(Label::ColourIds::textColourId, Colours::antiquewhite);
    
    rateSlider = new BKSingleSlider("rate", 5., 400., 100., 1);
    rateSlider->displaySliderVisible(false);
    rateSlider->setJustifyRight(false);
    addChildComponent(rateSlider);
    
    dragSliderLabel.setText("drag", dontSendNotification);
    dragSliderLabel.setColour(Label::ColourIds::textColourId, Colours::antiquewhite);
    
    dragSlider = new BKSingleSlider("drag", 0., 1., 0.5, 0.0001);
    dragSlider->displaySliderVisible(false);
    addChildComponent(dragSlider);
    
    tetherStiffnessSlider = new BKSingleSlider("anchor stiff", 0., 1., 0.5, 0.0001);
    tetherStiffnessSlider->displaySliderVisible(false);
    addChildComponent(tetherStiffnessSlider);
    
    intervalStiffnessSlider = new BKSingleSlider("interval stiff", 0., 1., 0.5, 0.0001);
    intervalStiffnessSlider->displaySliderVisible(false);
    intervalStiffnessSlider->setJustifyRight(false);
    addChildComponent(intervalStiffnessSlider);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::tuning_icon_png, BinaryData::tuning_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Tuning");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);
    
    scaleCB.setName("Scale");
    scaleCB.setTooltip("Select from a range of preset temperaments; open the Adaptive Tuning settings");
    addAndMakeVisible(scaleCB);
    
    springScaleCB.setName("SpringTuning Scale");
    springScaleCB.setTooltip("Select a temperament for the spring intervals.");
    addChildComponent(springScaleCB);
    
    scaleLabel.setText("Scale", dontSendNotification);
    
    fundamentalCB.setName("Fundamental");
    fundamentalCB.setTooltip("Select root around which your temperament will be generated");
    addAndMakeVisible(fundamentalCB);
    
    fundamentalLabel.setText("Fundamental", dontSendNotification);
    
    A1IntervalScaleCB.setName("A1IntervalScale");
    A1IntervalScaleCB.addListener(this);
    A1IntervalScaleCB.setTooltip("scale the sets how successive intervals are tuned");
    addAndMakeVisible(A1IntervalScaleCB);
    
    A1IntervalScaleLabel.setText("Adaptive:", dontSendNotification);
    
    A1Inversional.setButtonText ("invert");
    A1Inversional.setToggleState (true, dontSendNotification);
    A1Inversional.setColour(ToggleButton::textColourId, Colours::antiquewhite);
    A1Inversional.setColour(ToggleButton::tickColourId, Colours::antiquewhite);
    A1Inversional.setColour(ToggleButton::tickDisabledColourId, Colours::antiquewhite);
    A1Inversional.setTooltip("when selected, intervals will be tuned the same whether they ascend or descend; e.g. C-D will always be the same interval as C-Bb");
    addAndMakeVisible(A1Inversional);
    
    
    
    A1AnchorScaleCB.setName("A1AnchorScale");
    A1AnchorScaleCB.setTooltip("determines where the moving fundamental will be tuned to");
    addAndMakeVisible(A1AnchorScaleCB);
    
    A1AnchorScaleLabel.setText("Anchor:", dontSendNotification);
    addAndMakeVisible(A1AnchorScaleLabel);
    
    A1FundamentalCB.setName("A1Fundamental");
    A1FundamentalCB.setTooltip("sets fundamental for the anchor scale");
    addAndMakeVisible(A1FundamentalCB);
    
    A1ClusterThresh = new BKSingleSlider("Threshold", 1, 1000, 0, 1);
    A1ClusterThresh->setJustifyRight(false);
    A1ClusterThresh->setToolTipString("if this time (ms) is exceeded, the fundamental will reset");
    addAndMakeVisible(A1ClusterThresh);
    
    A1ClusterMax = new BKSingleSlider("Maximum", 1, 8, 1, 1);
    A1ClusterMax->setJustifyRight(false);
    A1ClusterMax->setToolTipString("after these many notes are played, the fundamental will reset");
    addAndMakeVisible(A1ClusterMax);
    
    A1reset.setButtonText("reset");
    addAndMakeVisible(A1reset);
    
    showSpringsButton.setButtonText("Spiral");
    showSpringsButton.setClickingTogglesState(true);
    showSpringsButton.setColour(TextButton::buttonOnColourId, Colours::red.withMultipliedAlpha(0.5));
    addAndMakeVisible(showSpringsButton);
    
    nToneRootCB.setName("nToneRoot");
    nToneRootCB.setTooltip("set root note, when semitone width is not 100");
    addAndMakeVisible(nToneRootCB);
    
    nToneRootOctaveCB.setName("nToneRootOctave");
    nToneRootOctaveCB.setTooltip("set octave for root note, when semitone width is not 100");
    addAndMakeVisible(nToneRootOctaveCB);
    
    nToneSemitoneWidthSlider = new BKSingleSlider("semitone width and root", 1, 200, 100, 0.001);
    nToneSemitoneWidthSlider->setJustifyRight(false);
    nToneSemitoneWidthSlider->displaySliderVisible(false);
    nToneSemitoneWidthSlider->setToolTipString("Adjusts half step distance. For example, 50 cents is a quarter-tone keyboard, and -100 cents is an inverted keyboard");
    addAndMakeVisible(nToneSemitoneWidthSlider);
    
    fillTuningCB();
    fillFundamentalCB();
    
    // Absolute Tuning Keyboard
    absoluteOffsets.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) absoluteOffsets.add(0.);
    absoluteKeyboard.setName("absolute");
    absoluteKeyboard.setAlpha(1);
    addAndMakeVisible(absoluteKeyboard);
    
    //Custom Tuning Keyboard
    customOffsets.ensureStorageAllocated(12);
    for(int i=0; i<12; i++) customOffsets.add(0.);

    customKeyboard.setName("offset from ET");
    customKeyboard.setDimensionRatio(2.0);
    customKeyboard.setFundamental(0);
    addAndMakeVisible(customKeyboard);
    
    offsetSlider = new BKSingleSlider("offset: ", -100, 100, 0, 0.1);
    offsetSlider->displaySliderVisible(false);
    offsetSlider->setToolTipString("Raise or lower the entire temperament in cents");
    addAndMakeVisible(offsetSlider);
    
    lastNote.setText("note: ", dontSendNotification);
    lastNote.setTooltip("last note played as MIDI value");
    lastInterval.setText("interval: ", dontSendNotification);
    lastInterval.setJustificationType(juce::Justification::centredRight);
    lastInterval.setTooltip("Distance between last two notes played");
    addAndMakeVisible(lastNote);
    addAndMakeVisible(lastInterval);
    
    currentFundamental.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(currentFundamental);
    
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    addAndMakeVisible(actionButton);
    
    updateComponentVisibility();
    
#if JUCE_IOS
    offsetSlider->addWantsBigOneListener(this);
    A1ClusterMax->addWantsBigOneListener(this);
    A1ClusterThresh->addWantsBigOneListener(this);
    
    absoluteKeyboard.addWantsBigOneListener(this);
    customKeyboard.addWantsBigOneListener(this);
    
    nToneSemitoneWidthSlider->addWantsBigOneListener(this);
#endif
    
    startTimerHz(30);
}

void TuningViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX+3, 10 * processor.paddingScalarY+3);
    
    float keyboardHeight = 100 + 50 * processor.paddingScalarY;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight);
    absoluteKeymapRow.reduce(gXSpacing, 0);
    
    absoluteKeyboard.setBounds(absoluteKeymapRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    actionButton.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    showSpringsButton.setBounds(comboBoxSlice);

    /* *** above here should be generic (mostly) to all prep layouts *** */
    /* ***         below here will be specific to each prep          *** */
    
    // ********* right column
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromLeft(gXSpacing);
    modeSlice.removeFromRight(gXSpacing);
    springTuningToggle.setBounds(modeSlice.removeFromLeft(showSpringsButton.getWidth()));
    modeSlice.removeFromLeft(gXSpacing);
    scaleCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    
    modeSlice.removeFromLeft(gXSpacing);
    fundamentalCB.setBounds(modeSlice);
    
    int customKeyboardHeight = 80 + 70. * processor.paddingScalarY;
    int extraY = (area.getHeight() - (customKeyboardHeight + gComponentSingleSliderHeight + gYSpacing * 3)) * 0.25;
    
    area.removeFromTop(extraY);
    Rectangle<int> customKeyboardSlice = area.removeFromTop(customKeyboardHeight);
    customKeyboardSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    customKeyboardSlice.removeFromRight(gXSpacing);
    customKeyboard.setBounds(customKeyboardSlice);
    
    area.removeFromTop(extraY);
    Rectangle<int> offsetSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
    offsetSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    offsetSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    offsetSlider->setBounds(offsetSliderSlice);
    
    area.removeFromTop(extraY);
    Rectangle<int> currentFundamentalSlice = area.removeFromTop(gComponentTextFieldHeight);
    currentFundamental.setBounds(currentFundamentalSlice);
    
    // ********* left column
    
    extraY = (leftColumn.getHeight() -
              (gComponentComboBoxHeight * 2 +
               gComponentSingleSliderHeight * 2 +
               gYSpacing * 5)) * 0.25;
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1IntervalScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    A1IntervalScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    A1IntervalScaleCBSlice.removeFromLeft(gXSpacing);
    int tempwidth = A1IntervalScaleCBSlice.getWidth() / 3.;
    A1Inversional.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleCB.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleCBSlice.removeFromRight(gXSpacing);
    A1reset.setBounds(A1IntervalScaleCBSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1ClusterMaxSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterMaxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    A1ClusterMax->setBounds(A1ClusterMaxSlice);
    nToneSemitoneWidthSlider->setBounds(A1ClusterMaxSlice);
    
    leftColumn.removeFromTop(gYSpacing);
    Rectangle<int> A1ClusterThreshSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterThreshSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    A1ClusterThresh->setBounds(A1ClusterThreshSlice);
    
    Rectangle<int> nToneRootCBSlice = A1ClusterThreshSlice.removeFromLeft(tempwidth);
    nToneRootCBSlice = nToneRootCBSlice.removeFromTop(gComponentComboBoxHeight);
    nToneRootCBSlice.removeFromLeft(gXSpacing * 2);
    nToneRootCB.setBounds(nToneRootCBSlice);
    
    Rectangle<int> nToneRootOctaveCBSlice = A1ClusterThreshSlice.removeFromLeft(tempwidth);
    nToneRootOctaveCBSlice = nToneRootOctaveCBSlice.removeFromTop(gComponentComboBoxHeight);
    nToneRootOctaveCBSlice.removeFromLeft(gXSpacing * 2);
    nToneRootOctaveCB.setBounds(nToneRootOctaveCBSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1AnchorScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    A1AnchorScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
    tempwidth = A1AnchorScaleCBSlice.getWidth() / 3.;
    A1AnchorScaleLabel.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
    A1AnchorScaleCB.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
    A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
    A1FundamentalCB.setBounds(A1AnchorScaleCBSlice);
    
    Rectangle<float> editAllBounds = absoluteKeyboard.getEditAllBounds();
    editAllBounds.translate(absoluteKeyboard.getX(), absoluteKeyboard.getY());
    lastNote.setBounds(editAllBounds.getRight() + gXSpacing, editAllBounds.getY(),editAllBounds.getWidth() * 2, editAllBounds.getHeight());
    lastInterval.setBounds(lastNote.getRight() + gXSpacing, lastNote.getY(),lastNote.getWidth(), lastNote.getHeight());
    

    rateSlider->setBounds(selectCB.getX()-gComponentSingleSliderXOffset, selectCB.getBottom() + gYSpacing, selectCB.getWidth()+gComponentSingleSliderXOffset*2., gComponentSingleSliderHeight);
    dragSlider->setBounds(fundamentalCB.getX()-gComponentSingleSliderXOffset, rateSlider->getY(), fundamentalCB.getWidth()+gComponentSingleSliderXOffset*2., gComponentSingleSliderHeight);
    
    springScaleCB.setBounds(scaleCB.getX(), dragSlider->getY(), scaleCB.getWidth(), gComponentComboBoxHeight);
    
    float sliderHeight = (absoluteKeyboard.getBottom() - (rateSlider->getBottom() + gYSpacing)) / 13.;
    
    intervalStiffnessSlider->setBounds(selectCB.getX() - gComponentSingleSliderXOffset,
                                       rateSlider->getBottom() + gYSpacing,
                                       rateSlider->getWidth(),
                                       gComponentSingleSliderHeight);
    
    tetherStiffnessSlider->setBounds(fundamentalCB.getX() - gComponentSingleSliderXOffset,
                                     intervalStiffnessSlider->getY(),
                                     fundamentalCB.getWidth() + 2.*gComponentSingleSliderXOffset,
                                     gComponentSingleSliderHeight);
    
    for (int i = 0; i < 12; i++)
    {
        springLabels[i]->setBounds(hideOrShow.getX(),
                                   intervalStiffnessSlider->getBottom() + (sliderHeight) * (11 - i),
                                   hideOrShow.getWidth(),
                                   sliderHeight);
        springSliders[i]->setBounds(selectCB.getX(),
                                    springLabels[i]->getY(),
                                    intervalStiffnessSlider->getWidth() * 0.75,
                                    sliderHeight);
    }
    
    updateComponentVisibility();
}

void TuningViewController::paint (Graphics& g)
{
    {
        g.fillAll(Colours::black);
        
        if (!showSprings) return;
        
        TuningProcessor::Ptr tuning;
        TuningPreparation::Ptr active;
        TuningModPreparation::Ptr mod;
        
        if (processor.updateState->currentDisplay == DisplayTuningMod)
        {
            mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);

            int tuningId = mod->getPreps().size() ? mod->getPreps().getUnchecked(0) : 0;
            
            tuning = processor.currentPiano->getTuningProcessor(tuningId);
            active = processor.gallery->getActiveTuningPreparation(tuningId);
        }
        else
        {
            tuning = processor.currentPiano->getTuningProcessor(processor.updateState->currentTuningId);
            active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
        }
        
    
        
        bool springsOn = active->getSpringsActive();
        
        Rectangle<int> b = getLocalBounds();
        b.removeFromTop(selectCB.getBottom());
        
        float midi,scalex,posx,radians,cx,cy;
        float centerx = b.getWidth() * 0.5f;
        float centery = b.getCentreY();
        
        float radius_scale = 0.25;
        float radius = jmin(b.getHeight() * radius_scale, b.getWidth() * radius_scale);
        float dimc_scale = 0.05;
        float dimc = jmin(b.getHeight() * dimc_scale, b.getWidth() * dimc_scale);
        int x_offset = 0.075 * b.getWidth();
        
        float midiScale;
        
        Particle::PtrArr particles = active->getTetherParticles();

        for (auto s : active->getSprings())
        {
            if (s->getEnabled())
            {
                Particle* a = s->getA();
                if(springsOn) midi = Utilities::ftom(Utilities::centsToFreq(a->getX() - (1200.0 * a->getOctave())));
                else
                {
                    midi = tuning->getOffset(a->getNote(), false);
                    midi += a->getNote();
                }
                
                scalex = ((midi - 60.0f) / 12.0f);
                
                float midiSave = midi;
                
                midiScale = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(a->getX() - (1200.0 * a->getOctave()))), 128);
                midiScale += ((a->getOctave() - 5) * 12.0);
                midiScale /= 60.;
                
                radians = scalex * Utilities::twopi - Utilities::pi * 0.5;
                
                float cxa = centerx + cosf(radians) * radius * midiScale;
                float cya = centery + sinf(radians) * radius * midiScale;
                
                Particle* b = s->getB();
                if(springsOn) midi = Utilities::ftom(Utilities::centsToFreq(b->getX() - (1200.0 * b->getOctave())));
                else {
                    midi = tuning->getOffset(b->getNote(), false);
                    midi += b->getNote();
                }
                
                scalex = ((midi - 60.0f) / 12.0f);
                
                midiScale = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(b->getX() - (1200.0 * b->getOctave()))), 128);
                midiScale += ((b->getOctave() - 5) * 12.0);
                midiScale /= 60.;
                
                radians = scalex * Utilities::twopi - Utilities::pi * 0.5;
                
                float cxb = centerx + cosf(radians) * radius * midiScale;
                float cyb = centery + sinf(radians) * radius * midiScale;
                
                double strength = s->getStrength();
                
                float hue = fmod((midi + midiSave)/2., 12.) / 12.;
                Colour colour (hue, 0.5f, 0.5f, 0.25f);
                
                g.setColour(colour);
                g.drawLine(cxa, cya, cxb, cyb,  (strength > 0.0) ? strength * 5.0 + 1.0 : 0.0);
                
                int h = 10, w = 35;
                
                int midX = (cxa + cxb) / 2.0; //+ xoff;
                int midY = (cya + cyb) / 2.0; //+ yoff;
            
                g.setColour(Colours::ghostwhite);
                g.setFont(12.0f);
                g.setOpacity(0.7);
                if(springsOn)
                {
                    g.drawText(String((int)round(s->getLength())), midX-dimc*0.25, midY, w, h, Justification::topLeft);
                }
                else
                {
                    g.drawText(String((int)round(100.*(midi - midiSave))), midX-dimc*0.25, midY, w, h, Justification::topLeft);
                }
            
            }
            
        }
        
        for (auto p : active->getParticles())
        {
            if (p->getEnabled())
            {
                // DRAW PARTICLE IN MOTION
                if(springsOn) {
                    midi = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(p->getX() - (1200.0 * p->getOctave()))), 128);
                    midi += ((p->getOctave() - 5) * 12.0);
                }
                else {
                    midi = tuning->getOffset(p->getNote(), false);
                    //DBG("midiOffset = " + String(midi) + " for note: " + String(p->getNote() % 12));
                    midi += p->getNote();
                }
                
                //DBG("midi = " + String(midi));
                midiScale = midi / 60.;
                
                //int cents = roundToInt(((midi - (float)p->getNote())) * 100.0);
                int cents = round(((midi - (float)p->getNote())) * 100.0);
                
                scalex = ((midi - 60.0f) / 12.0f);
                
                posx = scalex *  (b.getWidth() - tetherSliders[0]->getRight());
                
                radians = scalex * Utilities::twopi - Utilities::pi * 0.5;

                cx = centerx + cosf(radians) * radius * midiScale - dimc * 0.5f;
                cy = centery + sinf(radians) * radius * midiScale - dimc * 0.5f;
                
                float hue = fmod(midi, 12.) / 12.;
                Colour colour (hue, 0.5f, 0.5f, 0.9f);
                
                //g.setColour (Colours::black);
                g.setColour (colour);
                g.fillEllipse(cx, cy, dimc, dimc);
                
                g.setColour(Colours::white);
                g.setFont(14.0f);
                //g.drawText(String(round(cents)), cx + dimc * 0.25, cy-dimc*0.7, 40, 10, Justification::topLeft);
                g.drawText(String(round(cents)), cx-dimc*0.25, cy+dimc*0.25, dimc * 1.5, dimc * 0.5, Justification::centred);
            }
            
            //DRAW REST PARTICLE
            midi = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(p->getRestX() - (1200.0 * p->getOctave()))), 128);
            midi += ((p->getOctave() - 5) * 12.0);
            
            if(midi > 20 && midi < 109) {
                midiScale = midi / 60.;
                scalex = ((midi - 60.0f) / 12.0f);
                //posx = scalex *  (b.getWidth() - tetherSliders[0]->getRight());
                radians = scalex * Utilities::twopi - Utilities::pi * 0.5;
                cx = centerx + cosf(radians) * radius * midiScale - dimc * 0.5f;
                cy = centery + sinf(radians) * radius * midiScale - dimc * 0.5f;
                g.setColour (Colours::dimgrey);
                g.setOpacity(0.25);
                g.fillEllipse(cx, cy, dimc, dimc);
            }
        }
    }
}

void TuningViewController::fillTuningCB(void)
{
    scaleCB.clear(dontSendNotification);
    springScaleCB.clear(dontSendNotification);
    A1IntervalScaleCB.clear(dontSendNotification);
    A1AnchorScaleCB.clear(dontSendNotification);
    
    //create submenu of historical temperaments
    PopupMenu* additionalTuningsPopUp = scaleCB.getRootMenu();
    OwnedArray<PopupMenu> submenus;
    submenus.add(new PopupMenu());
    submenus.add(new PopupMenu());
    
    int count =0;
    for (int i = 0; i < cTuningSystemNames.size(); i++) //&& if(i<=8), for original systems; otherwise add to submenu of historical temperaments
    {
        String name = cTuningSystemNames[i];
        
        if(i<=8) //original bK scales
        {
            scaleCB.addItem(name, i+1);
            A1IntervalScaleCB.addItem(name, i+1);
            A1AnchorScaleCB.addItem(name, i+1);
        }

        if(name == "Adaptive Tuning 1" || name == "Adaptive Anchored Tuning 1")
        {
            A1IntervalScaleCB.setItemEnabled(i+1, false);
            A1AnchorScaleCB.setItemEnabled(i+1, false);
        }
        else
        {
            springScaleCB.addItem(name, ++count);
        }
                
        if(name == "Custom") {
            customIndex = i;
        }
        
        if(i>8 && i<=35) //historical
        {
            //add to Historical Temperaments popup
            DBG("adding historical temperament: " + name);
            PopupMenu* historicalMenu = submenus.getUnchecked(0);
            historicalMenu->addItem(i+1, name);
        }
        else if (i>35) //various
        {
            //add to Various popup
            DBG("adding various tunings: " + name);
            PopupMenu* variousMenu = submenus.getUnchecked(1);
            variousMenu->addItem(i+1, name);
        }
    }
    scaleCB.addSeparator();
    additionalTuningsPopUp->addSubMenu("Historical", *submenus.getUnchecked(0));
    additionalTuningsPopUp->addSubMenu("Various", *submenus.getUnchecked(1));
    
}

void TuningViewController::fillFundamentalCB(void)
{
    fundamentalCB.clear(dontSendNotification);
    A1FundamentalCB.clear(dontSendNotification);
    nToneRootCB.clear(dontSendNotification);
    nToneRootOctaveCB.clear(dontSendNotification);
    
    for (int i = 0; i < cFundamentalNames.size(); i++)
    {
        String name = cFundamentalNames[i];
        fundamentalCB.addItem(name, i+1);
        A1FundamentalCB.addItem(name, i+1);
        nToneRootCB.addItem(name, i+1);
    }
    
    for (int i = 0; i < 9; i++)
    {
        nToneRootOctaveCB.addItem(String(i), i+1);
    }
}

void TuningViewController::updateComponentVisibility()
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    bool set = true;
    if (processor.updateState->currentDisplay == DisplayTuningMod) set = false;
    
    for (auto s : tetherSliders)    s->setVisible(false);
    for (auto s : springSliders)    s->setVisible(false);
    for (auto l : springLabels)     l->setVisible(false);
    
    rateSlider->setVisible(false);
    dragSlider->setVisible(false);
    springScaleCB.setVisible(false);
    
    tetherStiffnessSlider->setVisible(false);
    intervalStiffnessSlider->setVisible(false);
    
    rateSliderLabel.setVisible(false);
    dragSliderLabel.setVisible(false);
    
    absoluteKeyboard.setVisible(true);
    customKeyboard.setVisible(true);
    lastInterval.setVisible(true);
    lastNote.setVisible(true);
    offsetSlider->setVisible(true);
    springTuningToggle.setVisible(true);
    springTuningLabel.setVisible(true);
    showSpringsButton.setVisible(true);
    
    if(scaleCB.getText() == "Adaptive Tuning 1")
    {
        A1IntervalScaleCB.setVisible(true);
        A1Inversional.setVisible(true);
        A1AnchorScaleCB.setVisible(false);
        A1FundamentalCB.setVisible(false);
        A1ClusterThresh->setVisible(true);
        A1ClusterMax->setVisible(true);
        A1IntervalScaleLabel.setVisible(true);
        A1AnchorScaleLabel.setVisible(false);
        A1FundamentalLabel.setVisible(false);
        A1reset.setVisible(true);
        currentFundamental.setVisible(true);
        nToneRootCB.setVisible(false);
        nToneRootOctaveCB.setVisible(false);
        nToneSemitoneWidthSlider->setVisible(false);

    }
    else if(scaleCB.getText() == "Adaptive Anchored Tuning 1")
    {
        A1IntervalScaleCB.setVisible(true);
        A1Inversional.setVisible(true);
        A1AnchorScaleCB.setVisible(true);
        A1FundamentalCB.setVisible(true);
        A1ClusterThresh->setVisible(true);
        A1ClusterMax->setVisible(true);
        A1IntervalScaleLabel.setVisible(true);
        A1AnchorScaleLabel.setVisible(true);
        A1FundamentalLabel.setVisible(true);
        A1reset.setVisible(true);
        currentFundamental.setVisible(true);
        nToneRootCB.setVisible(false);
        nToneRootOctaveCB.setVisible(false);
        nToneSemitoneWidthSlider->setVisible(false);
    }
    else
    {
        A1IntervalScaleCB.setVisible(false);
        A1Inversional.setVisible(false);
        A1AnchorScaleCB.setVisible(false);
        A1FundamentalCB.setVisible(false);
        A1ClusterThresh->setVisible(false);
        A1ClusterMax->setVisible(false);
        A1IntervalScaleLabel.setVisible(false);
        A1AnchorScaleLabel.setVisible(false);
        A1FundamentalLabel.setVisible(false);
        A1reset.setVisible(false);
        currentFundamental.setVisible(false);
        nToneRootCB.setVisible(true);
        nToneRootOctaveCB.setVisible(true);
        nToneSemitoneWidthSlider->setVisible(true);
    }
    
    bool tuningMod = (processor.updateState->currentDisplay == DisplayTuningMod);
    if (showSprings)
    {
        lastInterval.setVisible(false);
        lastNote.setVisible(false);
        
        Spring::PtrArr springs = active->getSprings();
        Spring::PtrArr tetherSprings = active->getTetherSprings();
        
        nToneRootCB.setVisible(false);
        nToneRootOctaveCB.setVisible(false);
        nToneSemitoneWidthSlider->setVisible(false);
        
        absoluteKeyboard.setVisible(false);
        customKeyboard.setVisible(false);
        offsetSlider->setVisible(false);
        

        if(active->getSpringsActive() || tuningMod)
        {
            rateSlider->setVisible(true);
            rateSlider->toFront(true);
            if (set) rateSlider->setValue(active->getSpringRate(), dontSendNotification);
            
            
            dragSlider->setVisible(true);
            dragSlider->toFront(true);
            if (set)
            {
                double newval = dt_asymwarp_inverse(1.0f - active->getSpringTuning()->getDrag(), 100.);
                dragSlider->setValue(newval, dontSendNotification);
            }
            
            tetherStiffnessSlider->setVisible(true);
            tetherStiffnessSlider->toFront(true);
            if (set) tetherStiffnessSlider->setValue(active->getSpringTuning()->getTetherStiffness(), dontSendNotification);
            
            //intervalStiffnessLabel.setVisible(true);
            intervalStiffnessSlider->setVisible(true);
            intervalStiffnessSlider->toFront(true);
            if (set) intervalStiffnessSlider->setValue(active->getSpringTuning()->getIntervalStiffness(), dontSendNotification);
            
            springScaleCB.setVisible(true);
            springScaleCB.toFront(true);
            
            for (int i = 0; i < 12; i++)
            {
                springSliders[i]->setVisible(true);
                springSliders[i]->toFront(true);
                if (set) springSliders[i]->setValue(springs[i]->getStrength(), dontSendNotification);
                
                springLabels[i]->setVisible(true);
            }
            
            for (int i = 0; i < 128; i++)
            {
                tetherSliders[i]->toFront(true);
                tetherSliders[i]->setValue(tetherSprings[i]->getStrength(), dontSendNotification);
            }
        }
        
    }
    
}

#if JUCE_IOS
void TuningViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

void TuningViewController::timerCallback(void)
{
    //if (processor.updateState->currentDisplay == DisplayTuning)
    {
        TuningProcessor::Ptr tProcessor;
        TuningPreparation::Ptr active;
        Tuning::Ptr tuning;
        TuningModPreparation::Ptr mod;
        
        int tuningId = processor.updateState->currentTuningId;
        bool isMod = false;
        if (processor.updateState->currentDisplay == DisplayTuningMod)
        {
            isMod = true;
            mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
            
            tuningId = mod->getPreps().size() ? mod->getPreps().getUnchecked(0) : 0;
        }
            
        tProcessor = processor.currentPiano->getTuningProcessor(tuningId);
        active = processor.gallery->getActiveTuningPreparation(tuningId);
        tuning = processor.gallery->getTuning(tuningId);
        
        if (tProcessor != nullptr)
        {
            if (tProcessor->getLastNoteTuning() != lastNoteTuningSave)
            {
                lastNoteTuningSave = tProcessor->getLastNoteTuning();
                lastNote.setText("note: " + String(lastNoteTuningSave, 3), dontSendNotification);
                lastInterval.setText("interval: "  + String(tProcessor->getLastIntervalTuning(), 3), dontSendNotification);
                
                currentFundamental.setText("current fundamental: " + String(ftom(tProcessor->getAdaptiveFundamentalFreq()), 3), dontSendNotification);
            }
            
            if(active->getScale() == AdaptiveTuning || active->getScale() == AdaptiveAnchoredTuning )
            {
                A1ClusterMax->setDisplayValue(tProcessor->getAdaptiveHistoryCounter() + 1);
                
                if(tProcessor->getAdaptiveClusterTimer() < active->getAdaptiveClusterThresh())
                    A1ClusterThresh->setDisplayValue(tProcessor->getAdaptiveClusterTimer());
                else
                {
                    A1ClusterThresh->setDisplayValue(0);
                    A1ClusterMax->setDisplayValue(0);
                }
            }
        }
        
        
        
        if (showSprings)
        {
            float sliderHeight = springSliders.getUnchecked(0)->getHeight();
            
            Tuning::Ptr tuning = tProcessor->getTuning();
            Spring::PtrArr tetherSprings =  active->getTetherSprings();
            Array<bool> locked = active->getSpringTuning()->getTethersLocked();
    
            int count = 0;
            for (int i = 0; i < 128; i++)
            {
                int pc = i % 12;
            
                if (tetherSprings[i]->getEnabled())
                {
                    tetherSliders[i]->setBounds(fundamentalCB.getX(),
                                                tetherStiffnessSlider->getBottom() + sliderHeight * (count),
                                                fundamentalCB.getWidth() * 0.75,
                                                sliderHeight);
                    
                    if (!isMod) tetherSliders[i]->setValue(tetherSprings[i]->getStrength(), dontSendNotification);
                    if(active->getSpringsActive())tetherSliders[i]->setVisible(true);
                    
                    tetherLabels[i]->setBounds(tetherSliders[i]->getRight() + gXSpacing,
                                               tetherSliders[i]->getY(),
                                               30,
                                               sliderHeight);
                    
                    tetherLabels[i]->setText(Utilities::getNoteString(i), dontSendNotification);
                    if(active->getSpringsActive()) tetherLabels[i]->setVisible(true);
                    count++;
                    
                }
                else
                {
                    tetherSliders[i]->setVisible(false);
                    tetherLabels[i]->setVisible(false);
                }
                
            }
            
            repaint();
        }
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TuningPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

TuningPreparationEditor::TuningPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TuningViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    for (int i = 0; i < 12; i++)
    {
        springSliders[i]->addListener(this);
    }
    
    for (int i = 0; i < 128; i++)
    {
        tetherSliders[i]->addListener(this);
    }
    
    springTuningToggle.addListener(this);
    
    rateSlider->addMyListener(this);
    
    dragSlider->addMyListener(this);
    
    intervalStiffnessSlider->addMyListener(this);
    
    tetherStiffnessSlider->addMyListener(this);
    
    springScaleCB.addListener(this);
    
    showSpringsButton.addListener(this);
    
    selectCB.addMyListener(this);
    
    selectCB.addListener(this);

    scaleCB.addListener(this);
    
    fundamentalCB.addListener(this);

    A1IntervalScaleCB.addListener(this);
    
    A1Inversional.addListener(this);
    
    A1AnchorScaleCB.addListener(this);
    
    A1FundamentalCB.addListener(this);
    
    A1ClusterThresh->addMyListener(this);
    
    A1ClusterMax->addMyListener(this);
    
    A1reset.addListener(this);
    
    
    absoluteKeyboard.addMyListener(this);
    
    customKeyboard.addMyListener(this);
    
    offsetSlider->addMyListener(this);
    
    nToneRootCB.addListener(this);
    
    nToneRootOctaveCB.addListener(this);
    
    nToneSemitoneWidthSlider->addMyListener(this);
    
    update();
}

int TuningPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeTuning);
    
    return processor.gallery->getAllTuning().getLast()->getId();
}

int TuningPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeTuning, processor.updateState->currentTuningId);
    
    return processor.gallery->getAllTuning().getLast()->getId();
}

void TuningPreparationEditor::deleteCurrent(void)
{
    int TuningId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeTuning, TuningId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentTuningId = -1;
}

void TuningPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentTuningId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void TuningPreparationEditor::actionButtonCallback(int action, TuningPreparationEditor* vc)
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
        processor.reset(PreparationTypeTuning, processor.updateState->currentTuningId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeTuning, processor.updateState->currentTuningId);
        vc->update();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentTuningId;
        Tuning::Ptr prep = processor.gallery->getTuning(Id);
        
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


void TuningPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    if (box == &selectCB)
    {
        setCurrentId(Id);
    }
    else if (box == &scaleCB)
    {

        DBG("name of scale chosen: " + box->getItemText(index));
        //prep->setScale((TuningSystem) index);
        //active->setScale((TuningSystem) index);
        
        //redoing this so we index by tuning name, rather than index, so we don't lock the menu structure down
        prep->setScaleByName(box->getItemText(index));
        active->setScaleByName(box->getItemText(index));
        
        //DBG("current tuning from processor = " + String(processor.updateState->currentTuningId));
        //DBG("current TuningSystem " + cTuningSystemNames[index]);
        DBG("current TuningSystem " + prep->getScaleName());
        customKeyboard.setValues(tuning->getCurrentScaleCents());
        
        //just for printing offsets out; temporary
        Array<float> tuningOffsets = tuning->getCurrentScale();
        String offsetString = " ";
        for(int i=0; i<tuningOffsets.size(); i++)
        {
            offsetString += (String(tuningOffsets.getUnchecked(i)) + ", ");
        }
        DBG("scale offsets: " + offsetString);

        
        if(active->getSpringsActive())
        {
            prep->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
            active->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
        }
        else
        {
            prep->getSpringTuning()->setTetherTuning(EqualTemperament); //use ET as background when not in Spring Tuning
            active->getSpringTuning()->setTetherTuning(EqualTemperament);
        }
        
        updateComponentVisibility();
        
    }
    else if (box == &fundamentalCB)
    {
        prep->setFundamental((PitchClass) index);
        active->setFundamental((PitchClass) index);
        
        customKeyboard.setFundamental(index);
        
        updateComponentVisibility();
        
    }
    else if (box == &A1IntervalScaleCB)
    {
        prep->setAdaptiveIntervalScale((TuningSystem) index);
        active->setAdaptiveIntervalScale((TuningSystem) index);
        
        updateComponentVisibility();
    }
    else if (box == &A1AnchorScaleCB)
    {
        prep->setAdaptiveAnchorScale((TuningSystem) index);
        active->setAdaptiveAnchorScale((TuningSystem) index);
        
        updateComponentVisibility();
    }
    else if (box == &A1FundamentalCB)
    {
        prep->setAdaptiveAnchorFundamental((PitchClass) index);
        active->setAdaptiveAnchorFundamental((PitchClass) index);
        
        updateComponentVisibility();
        
    }
    else if (box == &nToneRootCB)
    {
        prep->setNToneRootPC(index);
        active->setNToneRootPC(index);
        
        updateComponentVisibility();
    }
    else if (box == &nToneRootOctaveCB)
    {
        prep->setNToneRootOctave(index);
        active->setNToneRootOctave(index);
        
        updateComponentVisibility();
    }
    else if (box == &springScaleCB)
    {
        TuningSystem springScaleId = (TuningSystem) index;
        
        if (springScaleId >= AdaptiveTuning) springScaleId = (TuningSystem)((int)springScaleId + 2);
        
        prep->getSpringTuning()->setScaleId(springScaleId);
        active->getSpringTuning()->setScaleId(springScaleId);
        
        //TuningSystem springScaleId = prep->getSpringTuning()->getScaleId();
        
        Array<float> scale = tuning->getScaleCents(springScaleId);
        
        prep->getSpringTuning()->setIntervalTuning(scale);
        active->getSpringTuning()->setIntervalTuning(scale);
    }
    
    processor.gallery->setGalleryDirty(true);
}


void TuningPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllTuning())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Tuning"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeTuning, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentTuningId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);

    lastId = selectedId;
}


void TuningPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);

    tuning->setName(name);
    
    processor.gallery->setGalleryDirty(true);
}


void TuningPreparationEditor::update(void)
{
    if (processor.updateState->currentTuningId < 0) return;
    
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentTuningId, dontSendNotification);
        scaleCB.setSelectedItemIndex(prep->getScale(), dontSendNotification);
        
        int springScaleId = prep->getCurrentSpringScaleId();
        if (springScaleId >= AdaptiveTuning) springScaleId = (TuningSystem)((int)springScaleId - 2);
        springScaleCB.setSelectedItemIndex(springScaleId, dontSendNotification);
        
        fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
        offsetSlider->setValue(prep->getFundamentalOffset() * 100., dontSendNotification);

        absoluteKeyboard.setValues(prep->getAbsoluteOffsetsCents());
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
        
        A1IntervalScaleCB.setSelectedItemIndex(prep->getAdaptiveIntervalScale(), dontSendNotification);
        A1Inversional.setToggleState(prep->getAdaptiveInversional(), dontSendNotification);
        A1AnchorScaleCB.setSelectedItemIndex(prep->getAdaptiveAnchorScale(), dontSendNotification);
        A1FundamentalCB.setSelectedItemIndex(prep->getAdaptiveAnchorFundamental(), dontSendNotification);
        A1ClusterThresh->setValue(prep->getAdaptiveClusterThresh(), dontSendNotification);
        A1ClusterMax->setValue(prep->getAdaptiveHistory(), dontSendNotification);
        
        nToneRootCB.setSelectedItemIndex(prep->getNToneRootPC(), dontSendNotification);
        nToneRootOctaveCB.setSelectedItemIndex(prep->getNToneRootOctave(), dontSendNotification);
        nToneSemitoneWidthSlider->setValue(prep->getNToneSemitoneWidth(), dontSendNotification);
        
        rateSlider->setValue(prep->getSpringTuning()->getRate(), dontSendNotification);
        
        tetherStiffnessSlider->setValue(prep->getSpringTuning()->getTetherStiffness(), dontSendNotification);
        intervalStiffnessSlider->setValue(prep->getSpringTuning()->getIntervalStiffness(), dontSendNotification);

        springTuningToggle.setToggleState(prep->getSpringsActive(), dontSendNotification);

        //dragSlider->setValue(  //must remember to use dt_asym_inversion on 1 - val)
    }
    
    updateComponentVisibility();
}

void TuningPreparationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
 
    if(name == absoluteKeyboard.getName())
    {
        //DBG("updating absolute tuning vals");
        prep->setAbsoluteOffsetCents(values);
        active->setAbsoluteOffsetCents(values);
    }
    else if(name == customKeyboard.getName())
    {
        //DBG("updating custom tuning vals");
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);
        
        prep->setScale((TuningSystem)customIndex);
        active->setScale((TuningSystem)customIndex);
        
        //DBG("keyboardSliderChanged values.size() = " + String(values.size()));
        prep->setCustomScaleCents(values);
        active->setCustomScaleCents(values);
        
        prep->getSpringTuning()->setIntervalTuning(values);
        active->getSpringTuning()->setIntervalTuning(values);
        
    }
    processor.gallery->setGalleryDirty(true);
}

void TuningPreparationEditor::sliderValueChanged (Slider* slider)
{
    
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    double value = slider->getValue();
    
    String name = slider->getName();
    
    Spring::PtrArr tetherSprings = active->getTetherSprings();
    Spring::PtrArr springs = active->getSprings();

    for (int i = 0; i < 128; i++)
    {
        if (slider == tetherSliders[i])
        {
            prep->getSpringTuning()->setTetherWeight(i, value);
            active->getSpringTuning()->setTetherWeight(i, value);
            break;
        }
        else if (slider == springSliders[i])
        {
            prep->getSpringTuning()->setSpringWeight((i+1), value);
            active->getSpringTuning()->setSpringWeight((i+1), value);
            break;
        }
    }
}

void TuningPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    if(slider == offsetSlider) {
        //DBG("got offset " + String(val));
        prep->setFundamentalOffset(val * 0.01);
        active->setFundamentalOffset(val * 0.01);
    }
    else if(slider == A1ClusterThresh) {
        //DBG("got A1ClusterThresh " + String(val));
        prep->setAdaptiveClusterThresh(val);
        active->setAdaptiveClusterThresh(val);
    }
    else if(slider == A1ClusterMax) {
        //DBG("got A1ClusterMax " + String(val));
        prep->setAdaptiveHistory(val);
        active->setAdaptiveHistory(val);
    }
    else if(slider == nToneSemitoneWidthSlider) {
        //DBG("got nToneSemiToneSliderWidth " + String(val));
        prep->setNToneSemitoneWidth(val);
        active->setNToneSemitoneWidth(val);
    }
    else if (slider == rateSlider)
    {
        prep->getSpringTuning()->setRate(val, false);
        active->getSpringTuning()->setRate(val);
    }
    else if (slider == dragSlider)
    {
        double newval = dt_asymwarp(val, 100.);
        //DBG("warped = " + String(newval) + " inverted = " + String(dt_asymwarp_inverse(newval, 100.)));
        prep->getSpringTuning()->setDrag(1. - newval);
        active->getSpringTuning()->setDrag(1. - newval);
    }
    else if (slider == tetherStiffnessSlider)
    {
        prep->getSpringTuning()->setTetherStiffness(val);
        active->getSpringTuning()->setTetherStiffness(val);
    }
    else if (slider == intervalStiffnessSlider)
    {
        prep->getSpringTuning()->setIntervalStiffness(val);
        active->getSpringTuning()->setIntervalStiffness(val);
    }
    
    processor.gallery->setGalleryDirty(true);
}

void TuningPreparationEditor::buttonClicked (Button* b)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    if (b == &A1Inversional)
    {
        DBG("setting A1Inversional " + String((int)A1Inversional.getToggleState()));

        prep->setAdaptiveInversional(A1Inversional.getToggleState());
        active->setAdaptiveInversional(A1Inversional.getToggleState());
        
        processor.gallery->setGalleryDirty(true);
    }
    else if (b == &showSpringsButton)
    {
        showSprings = !showSprings;
        
        updateComponentVisibility();
    }
    else if (b == &A1reset)
    {
        DBG("resetting A1");
        
        TuningProcessor::Ptr tProcessor = processor.currentPiano->getTuningProcessor(processor.updateState->currentTuningId);
        tProcessor->adaptiveReset();
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &springTuningToggle)
    {
        bool state = b->getToggleState();
        DBG("springTuningToggle " + String((int)state));
        
        prep->getSpringTuning()->setActive(state);
        active->getSpringTuning()->setActive(state);
        
        if(state)
        {
            prep->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
            active->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
        }
        else
        {
            prep->getSpringTuning()->setTetherTuning(EqualTemperament);
            active->getSpringTuning()->setTetherTuning(EqualTemperament);
        }
        
        //need to make sure the interval scale is also set; i'm finding that sometimes i have to manually change away from just and back to get the system to work
        TuningSystem springScaleId = prep->getSpringTuning()->getScaleId();
        Array<float> scale = tuning->getScaleCents(springScaleId);
        prep->getSpringTuning()->setIntervalTuning(scale);
        active->getSpringTuning()->setIntervalTuning(scale);
        
        updateComponentVisibility();
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TuningModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
TuningModificationEditor::TuningModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TuningViewController(p, theGraph)
{
    
    lastNote.setVisible(false);
    lastInterval.setVisible(false);
    A1reset.setVisible(false);
    greyOutAllComponents();
    
    fillSelectCB(-1,-1);
    selectCB.addMyListener(this);
    selectCB.addListener(this);
    scaleCB.addListener(this);
    
    fundamentalCB.addListener(this);
    A1IntervalScaleCB.addListener(this);
    A1Inversional.addListener(this);
    A1AnchorScaleCB.addListener(this);
    A1FundamentalCB.addListener(this);
    A1ClusterThresh->addMyListener(this);
    A1ClusterMax->addMyListener(this);
    A1reset.addListener(this);
    absoluteKeyboard.addMyListener(this);
    customKeyboard.addMyListener(this);
    offsetSlider->addMyListener(this);
    
    nToneRootCB.addListener(this);
    nToneRootOctaveCB.addListener(this);
    nToneSemitoneWidthSlider->addMyListener(this);
    
    
    // ~ ~ ~ ~ ~ SPRING TUNING STUFF ~ ~ ~ ~ ~
    for (int i = 0; i < 12; i++)
    {
        springSliders[i]->addListener(this);
    }
    
    for (int i = 0; i < 128; i++)
    {
        tetherSliders[i]->addListener(this);
    }
    
    springTuningToggle.addListener(this);
    
    rateSlider->addMyListener(this);
    
    dragSlider->addMyListener(this);
    
    intervalStiffnessSlider->addMyListener(this);
    
    tetherStiffnessSlider->addMyListener(this);
    
    springScaleCB.addListener(this);
    
    showSpringsButton.addListener(this);
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

    update();
}

void TuningModificationEditor::greyOutAllComponents()
{
    scaleCB.setAlpha(gModAlpha);
    fundamentalCB.setAlpha(gModAlpha);
    A1IntervalScaleCB.setAlpha(gModAlpha);
    A1Inversional.setAlpha(gModAlpha);
    A1AnchorScaleCB.setAlpha(gModAlpha);
    A1FundamentalCB.setAlpha(gModAlpha);
    A1ClusterThresh->setDim(gModAlpha);
    A1ClusterMax->setDim(gModAlpha);
    absoluteKeyboard.setAlpha(gModAlpha);
    customKeyboard.setAlpha(gModAlpha);
    offsetSlider->setDim(gModAlpha);
    A1IntervalScaleLabel.setAlpha(gModAlpha);
    A1AnchorScaleLabel.setAlpha(gModAlpha);
    
    nToneRootCB.setAlpha(gModAlpha);
    nToneRootOctaveCB.setAlpha(gModAlpha);
    nToneSemitoneWidthSlider->setDim(gModAlpha);
    
    for (int i = 0; i < 12; i++)
    {
        springSliders[i]->setAlpha(gModAlpha);
    }
    
    for (int i = 0; i < 128; i++)
    {
        tetherSliders[i]->setAlpha(gModAlpha);
    }
    
    springTuningToggle.setAlpha(gModAlpha);
    
    rateSlider->setAlpha(gModAlpha);
    
    dragSlider->setAlpha(gModAlpha);
    
    intervalStiffnessSlider->setAlpha(gModAlpha);
    
    tetherStiffnessSlider->setAlpha(gModAlpha);
    
    springScaleCB.setAlpha(gModAlpha);
}

void TuningModificationEditor::highlightModedComponents()
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if(mod->getParam(TuningScale) != "")                scaleCB.setAlpha(1.);
    if(mod->getParam(TuningFundamental) != "")          fundamentalCB.setAlpha(1);
    if(mod->getParam(TuningA1IntervalScale) != "")      { A1IntervalScaleCB.setAlpha(1); A1IntervalScaleLabel.setAlpha(1); }
    if(mod->getParam(TuningA1Inversional) != "")        A1Inversional.setAlpha(1);
    if(mod->getParam(TuningA1AnchorScale) != "")        { A1AnchorScaleCB.setAlpha(1); A1AnchorScaleLabel.setAlpha(1); }
    if(mod->getParam(TuningA1AnchorFundamental) != "")  A1FundamentalCB.setAlpha(1);
    if(mod->getParam(TuningA1ClusterThresh) != "")      A1ClusterThresh->setBright();;
    if(mod->getParam(TuningA1History) != "")            A1ClusterMax->setBright();;
    if(mod->getParam(TuningAbsoluteOffsets) != "")      absoluteKeyboard.setAlpha(1);
    if(mod->getParam(TuningCustomScale) != "")          customKeyboard.setAlpha(1);
    if(mod->getParam(TuningOffset) != "")               offsetSlider->setBright();
    if(mod->getParam(TuningNToneRootCB) != "")          nToneRootCB.setAlpha(1);
    if(mod->getParam(TuningNToneRootOctaveCB) != "")    nToneRootOctaveCB.setAlpha(1);
    if(mod->getParam(TuningNToneSemitoneWidth) != "")   nToneSemitoneWidthSlider->setBright();
    if (mod->getParam(TuningSpringTetherStiffness) != "")
    {
        tetherStiffnessSlider->setAlpha(1);
        tetherStiffnessLabel.setAlpha(1);
    }
    if (mod->getParam(TuningSpringIntervalStiffness) != "")
    {
        intervalStiffnessSlider->setAlpha(1);
        intervalStiffnessLabel.setAlpha(1);
    }
    if (mod->getParam(TuningSpringRate) != "")
    {
        DBG("mod rate: " + String(mod->getParam(TuningSpringRate)));
        rateSlider->setAlpha(1);
        rateSliderLabel.setAlpha(1);
    }
    if (mod->getParam(TuningSpringDrag) != "")
    {
        dragSlider->setAlpha(1);
        dragSliderLabel.setAlpha(1);
    }
    if (mod->getParam(TuningSpringActive) != "")
    {
        springTuningToggle.setAlpha(1);
    }
    if (mod->getParam(TuningSpringTetherWeights) != "")
    {
        for (auto slider : tetherSliders) slider->setAlpha(1);
    }
    if (mod->getParam(TuningSpringIntervalWeights) != "")
    {
        for (auto slider : springSliders) slider->setAlpha(1);
    }
    if (mod->getParam(TuningSpringIntervalScale) != "")
    {
        springScaleCB.setAlpha(1);
    }
    
    repaint();
}

void TuningModificationEditor::update(void)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (mod != nullptr)
    {
        greyOutAllComponents();
        highlightModedComponents();
        
        selectCB.setSelectedId(processor.updateState->currentModTuningId, dontSendNotification);
        
        String val = mod->getParam(TuningScale);
        scaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningFundamental);
        fundamentalCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningOffset);
        offsetSlider->setValue(val.getFloatValue() * 100., dontSendNotification);
        
        val = mod->getParam(TuningAbsoluteOffsets);
        absoluteKeyboard.setValues(stringToFloatArray(val));
        
        val = mod->getParam(TuningCustomScale);
        customKeyboard.setValues(stringToFloatArray(val));
        
        val = mod->getParam(TuningA1IntervalScale);
        A1IntervalScaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningA1Inversional);
        A1Inversional.setToggleState((bool)val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningA1AnchorScale);
        A1AnchorScaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningA1AnchorFundamental);
        A1FundamentalCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningA1ClusterThresh);
        A1ClusterThresh->setValue(val.getLargeIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningA1History);
        A1ClusterMax->setValue(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningNToneRootCB);
        nToneRootCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningNToneRootOctaveCB);
        nToneRootOctaveCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        
        val = mod->getParam(TuningNToneSemitoneWidth);
        nToneSemitoneWidthSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(TuningSpringTetherStiffness);
        tetherStiffnessSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(TuningSpringIntervalStiffness);
        intervalStiffnessSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(TuningSpringRate);
        rateSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(TuningSpringDrag);
        double newval = dt_asymwarp_inverse(1.0f - val.getFloatValue(), 100.);
        dragSlider->setValue(newval, dontSendNotification);
    
        val = mod->getParam(TuningSpringActive);
        springTuningToggle.setToggleState((bool)val.getIntValue(), dontSendNotification);
        
        Array<float> vals;
        
        val = mod->getParam(TuningSpringTetherWeights);
        vals = stringToFloatArray(val);
        for (int i = 0; i < 128; i++) tetherSliders[i]->setValue(vals[i], dontSendNotification);
        
        val = mod->getParam(TuningSpringIntervalWeights);
        vals = stringToFloatArray(val);
        for (int i = 0; i < 12; i++) springSliders[i]->setValue(vals[i], dontSendNotification);
        
        val = mod->getParam(TuningSpringIntervalScale);
        
        int springScaleId = val.getIntValue();
        if (springScaleId >= AdaptiveTuning) springScaleId = (TuningSystem)((int)springScaleId - 2);
        springScaleCB.setSelectedItemIndex(springScaleId, dontSendNotification);
    
        updateComponentVisibility();
        A1reset.setVisible(false);
    }
    
}

void TuningModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getTuningModPreparations())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("TuningMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeTuning, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentTuningId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

int TuningModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeTuningMod);
    
    return processor.gallery->getTuningModPreparations().getLast()->getId();
}

int TuningModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeTuningMod, processor.updateState->currentModTuningId);
    
    return processor.gallery->getTuningModPreparations().getLast()->getId();
}

void TuningModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeTuningMod, oldId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentModTuningId = -1;
}

void TuningModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModTuningId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void TuningModificationEditor::actionButtonCallback(int action, TuningModificationEditor* vc)
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
        processor.clear(PreparationTypeTuningMod, processor.updateState->currentModTuningId);
        vc->update();
        vc->updateModification();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModTuningId;
        TuningModPreparation::Ptr prep = processor.gallery->getTuningModPreparation(Id);
        
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

void TuningModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (box == &selectCB)
    {
        setCurrentId(Id);
    }
    else if (box == &scaleCB)
    {
        mod->setParam(TuningScale, String(index));
        scaleCB.setAlpha(1.);
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
    }
    else if (box == &fundamentalCB)
    {
        mod->setParam(TuningFundamental, String(index));
        fundamentalCB.setAlpha(1.);
        
        customKeyboard.setFundamental(index);
    }
    else if (box == &A1IntervalScaleCB)
    {
        mod->setParam(TuningA1IntervalScale, String(index));
        A1IntervalScaleCB.setAlpha(1.);
        A1IntervalScaleLabel.setAlpha(1);
    }
    else if (box == &A1AnchorScaleCB)
    {
        mod->setParam(TuningA1AnchorScale, String(index));
        A1AnchorScaleCB.setAlpha(1.);
        A1AnchorScaleLabel.setAlpha(1);
    }
    else if (box == &A1FundamentalCB)
    {
        mod->setParam(TuningA1AnchorFundamental, String(index));
        A1FundamentalCB.setAlpha(1.);
    }
    else if (box == &nToneRootCB)
    {
        mod->setParam(TuningNToneRootCB, String(index));
        nToneRootCB.setAlpha(1.);
    }
    else if (box == &nToneRootOctaveCB)
    {
        mod->setParam(TuningNToneRootOctaveCB, String(index));
        nToneRootOctaveCB.setAlpha(1.);
    }
    else if (box == &springScaleCB)
    {
        TuningSystem springScaleId = (TuningSystem) index;
        
        if (springScaleId >= AdaptiveTuning) springScaleId = (TuningSystem)((int)springScaleId + 2);
        
        mod->setParam(TuningSpringIntervalScale, String(springScaleId));
    }

    if (name != selectCB.getName()) updateModification();
    
    updateComponentVisibility();
    A1reset.setVisible(false);
}

void TuningModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId)->setName(name);
    
    updateModification();
}

void TuningModificationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if(name == absoluteKeyboard.getName())
    {
        mod->setParam(TuningAbsoluteOffsets, floatArrayToString(values));
        absoluteKeyboard.setAlpha(1.);
        
    }
    else if(name == customKeyboard.getName())
    {
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);
        
        mod->setParam(TuningCustomScale, floatArrayToString(values));
        mod->setParam(TuningScale, String(customIndex));
        customKeyboard.setAlpha(1.);
    }
    
    updateModification();
}

void TuningModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if(slider == offsetSlider)
    {
        mod->setParam(TuningOffset, String(val * 0.01));
        offsetSlider->setBright();
    }
    else if(slider == A1ClusterThresh)
    {
        mod->setParam(TuningA1ClusterThresh, String(val));
        A1ClusterThresh->setBright();
    }
    else if(slider == A1ClusterMax)
    {
        mod->setParam(TuningA1History, String(val));
        A1ClusterMax->setBright();
    }
    else if(slider == nToneSemitoneWidthSlider)
    {
        mod->setParam(TuningNToneSemitoneWidth, String(val));
        nToneSemitoneWidthSlider->setBright();
    }
    else if (slider == rateSlider)
    {
        mod->setParam(TuningSpringRate, String(val));
        rateSlider->setBright();
    }
    else if (slider == dragSlider)
    {
        double newval = dt_asymwarp(val, 100.);
        mod->setParam(TuningSpringDrag, String(1.-newval));
        
        dragSlider->setBright();
    }
    else if (slider == tetherStiffnessSlider)
    {
        mod->setParam(TuningSpringTetherStiffness, String(val));
        tetherStiffnessSlider->setBright();
    }
    else if (slider == intervalStiffnessSlider)
    {
        mod->setParam(TuningSpringIntervalStiffness, String(val));
        intervalStiffnessSlider->setBright();
    }

    updateModification();
}

void TuningModificationEditor::sliderValueChanged (Slider* slider)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    double value = slider->getValue();
    
    String name = slider->getName();
    
    Array<float> tetherWeights = stringToFloatArray(mod->getParam(TuningSpringTetherWeights));
    Array<float> intervalWeights = stringToFloatArray(mod->getParam(TuningSpringIntervalWeights));
    
    if (tetherWeights.size() < 128)
    {
        tetherWeights = tuning->getStaticSpringTuning()->getTetherWeights();
    }
    
    if (intervalWeights.size() < 12)
    {
        intervalWeights = tuning->getStaticSpringTuning()->getSpringWeights();
    }
    
    
    for (int i = 0; i < 128; i++)
    {
        if (slider == tetherSliders[i])
        {
            tetherWeights.setUnchecked(i, value);
            mod->setParam(TuningSpringTetherWeights, floatArrayToString(tetherWeights));
            break;
        }
        else if (slider == springSliders[i])
        {
            intervalWeights.setUnchecked(i, value);
            String thing = floatArrayToString(intervalWeights);
            DBG("spring slider: " + thing);
            mod->setParam(TuningSpringIntervalWeights, floatArrayToString(intervalWeights));
            break;
        }
    }
    
    updateModification();
}

void TuningModificationEditor::updateModification(void)
{
    greyOutAllComponents();
    highlightModedComponents();
    
    processor.updateState->modificationDidChange = true;
}

void TuningModificationEditor::buttonClicked (Button* b)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (b == &A1Inversional)
    {
        mod->setParam(TuningA1Inversional, String((int)A1Inversional.getToggleState()));
        A1Inversional.setAlpha(1.);
    }
    else if (b == &A1reset)
    {
        // N/A in mod
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getModOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &showSpringsButton)
    {
        showSprings = !showSprings;
        
        updateComponentVisibility();
    }
    else if (b == &springTuningToggle)
    {
        bool state = b->getToggleState();
        DBG("springTuningToggle " + String((int)state));
        
        mod->setParam(TuningSpringActive, String((int) state));
        
        updateComponentVisibility();
    }
    
    updateModification();
}






