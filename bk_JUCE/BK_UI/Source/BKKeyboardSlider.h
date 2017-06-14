/*
  ==============================================================================

    BKKeyboardSlider.h
    Created: 12 Jun 2017 7:10:27am
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"
#include "BKComponent.h"
#include "BKListener.h"
#include "BKKeyboard.h"
#include "BKKeyboardState.h"

class BKKeyboardSliderListener
{
    
public:
    
    //BKMultiSliderListener() {}
    virtual ~BKKeyboardSliderListener() {};
    
    virtual void keyboardSliderChanged(String name, Array<float> values) = 0;

};


class BKKeyboardSlider :
public BKComponent,
public BKListener,
public BKKeymapKeyboardStateListener
{
    
public:
    
    BKKeyboardSlider();
    ~BKKeyboardSlider() {};
    
    //void setTo(Array<float> newvals, NotificationType newnotify);
    
    void resized() override;
    void paint (Graphics&) override;
    
    ListenerList<BKKeyboardSliderListener> listeners;
    void addMyListener(BKKeyboardSliderListener* listener)     { listeners.add(listener); }
    void removeMyListener(BKKeyboardSliderListener* listener)  { listeners.remove(listener); }
    
    void setName(String newName) { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName() { return sliderName; }
    
    void setAvailableRange(int min, int max);
    void useOrderedPairs(bool op) { orderedPairs = op; };
    
    Array<float> getAllValues();
    Array<float> getActiveValues();
    
    void setAllValues(Array<float> newvals);
    void setActiveValues(Array<float> newvals);
    void updateDisplay();
    
private:
    
    String sliderName;
    BKLabel showName;

    TextEditor keyboardValueTF;
    BKKeymapKeyboardState keyboardState;
    Component *keyboardComponent;
    ScopedPointer<TextEditor> keyboardValsTextField;
    TextButton keyboardValsTextFieldOpen;
    
    int keyboardSize, minKey, maxKey;
    int lastKeyPressed;
    Array<float> keyboardVals;
    bool orderedPairs;
    
    void setActiveValsFromString(String s);
    
    void handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber) override {};
    void handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber) override {};
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    
    void bkMessageReceived (const String& message) override {};
    void bkComboBoxDidChange (ComboBox*) override {};
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void bkTextFieldDidChange (TextEditor& txt) override;
    void bkButtonClicked (Button* b) override;
    void mouseMove(const MouseEvent& e) override;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKKeyboardSlider)
};
