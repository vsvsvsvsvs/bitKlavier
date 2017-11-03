/*
  ==============================================================================

    BKSlider.h
    Created: 6 Apr 2017 9:50:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

/* TODO
 
 1. need version of SingleSlider that can handle multiple values (for Nostalgic transposition)
 
 */

#ifndef BKSLIDER_H_INCLUDED
#define BKSLIDER_H_INCLUDED

#include "BKUtilities.h"
#include "BKComponent.h"
#include "BKLookAndFeel.h"
#include "BKNumberPad.h"

class BKStackedSlider;
class BKSingleSlider;
class BKMultiSlider;
class BKRangeSlider;
class BKWaveDistanceUndertowSlider;
class BKKeyboardSlider;
class BKTextEditor;

typedef enum BKRangeSliderType
{
    BKRangeSliderMin = 0,
    BKRangeSliderMax,
    BKRangeSliderNil
    
} BKRangeSliderType;

typedef enum KSliderTextFieldType
{
    KSliderAllValues,
    KSliderThisValue,
    KSliderTextFieldTypeNil,
} KSliderTextFieldType;

class WantsKeyboardListener
{
    
public:
    virtual ~WantsKeyboardListener() {};
    
    virtual void textEditorWantsKeyboard(BKTextEditor*) {};
    virtual void bkStackedSliderWantsKeyboard(BKStackedSlider*) {};
    virtual void bkSingleSliderWantsKeyboard(BKSingleSlider*) {};
    virtual void multiSliderWantsKeyboard(BKMultiSlider*) {};
    virtual void bkRangeSliderWantsKeyboard(BKRangeSlider*, BKRangeSliderType which) {};
    virtual void bkWaveDistanceUndertowSliderWantsKeyboard(BKWaveDistanceUndertowSlider*, NostalgicParameterType type) {};
    virtual void keyboardSliderWantsKeyboard(BKKeyboardSlider*, KSliderTextFieldType which) {};
    
};

class BKTextEditor : public TextEditor
{
public:
    BKTextEditor(void):
    TextEditor()
    {
        Font font;
        
        font.setSizeAndStyle(fontHeight, 0, 0.75, 0.25);
        
        
        setFont(font);
#if JUCE_IOS
        setReadOnly(true);
        setCaretVisible(true);
        setSelectAllWhenFocused(false);
#endif
    }
    
    ~BKTextEditor(void){}
    
    inline void mouseDown(const MouseEvent& e)
    {
        inputListeners.call(&WantsKeyboardListener::textEditorWantsKeyboard, this);
    }

    void addWantsKeyboardListener(WantsKeyboardListener* listener)     { inputListeners.add(listener);      }
    void removeWantsKeyboardListener(WantsKeyboardListener* listener)  { inputListeners.remove(listener);   }

private:
    
    ListenerList<WantsKeyboardListener> inputListeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKTextEditor)
    
};

typedef enum BKMultiSliderType {
    HorizontalMultiSlider = 0,
    VerticalMultiSlider,
    HorizontalMultiBarSlider,
    VerticalMultiBarSlider,
    BKMultiSliderTypeNil
} BKMultiSliderType;


// ******************************************************************************************************************** //
// **************************************************  BKSubSlider **************************************************** //
// ******************************************************************************************************************** //

class BKSubSlider : public Slider
{
public:
    
    BKSubSlider (SliderStyle sstyle, double min, double max, double def, double increment, int width, int height);
    ~BKSubSlider();

    double getValueFromText	(const String & text ) override;
    bool isActive() { return active; }
    void isActive(bool newactive) {active = newactive; }
    void setMinMaxDefaultInc(std::vector<float> newvals);
    void setSkewFromMidpoint(bool sfm);
        
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    int sliderWidth, sliderHeight;
    
    bool sliderIsVertical;
    bool sliderIsBar;
    bool skewFromMidpoint;
    
    bool active;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSubSlider)
};




// ******************************************************************************************************************** //
// **************************************************  BKMultiSlider ************************************************** //
// ******************************************************************************************************************** //

class BKMultiSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
{
    
public:
    
    BKMultiSlider(BKMultiSliderType which);
    ~BKMultiSlider();

    void addSlider(int where, bool active, NotificationType newnotify);
    void addSubSlider(int where, bool active, NotificationType newnotify);

    void deactivateSlider(int where, NotificationType notify);
    void deactivateAll(NotificationType notify);
    void deactivateAllAfter(int where, NotificationType notify);
    void deactivateAllBefore(int where, NotificationType notify);
    
    
    
    inline void setText(String text) { editValsTextField->setText(text, dontSendNotification); }
    
    inline TextEditor* getTextEditor(void)
    {
        return editValsTextField.get();
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        if (setValue)   textEditorReturnKeyPressed(*editValsTextField);
        else            textEditorEscapeKeyPressed(*editValsTextField);
    }
    
    int whichSlider (const MouseEvent &e);
    int whichSubSlider (int which);
    int whichSubSlider (int which, const MouseEvent &e);
    int whichActiveSlider (int which);
    
    void mouseDrag(const MouseEvent &e) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    void mouseDown (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    
    void setTo(Array<float> newvals, NotificationType newnotify);
    void setTo(Array<Array<float>> newvals, NotificationType newnotify);
    void setMinMaxDefaultInc(std::vector<float> newvals);
    void setCurrentSlider(int activeSliderNum);
    
    void setAllowSubSlider(bool ss) { allowSubSliders = ss; }
    void setSubSliderName(String ssname) { subSliderName = ssname; }
    void setSkewFromMidpoint(bool sfm);
    
    void cleanupSliderArray();
    void resetRanges();
    
    inline int getNumActive(void) const noexcept { return numActiveSliders;}
    inline int getNumVisible(void) const noexcept { return numVisibleSliders;}
    
    void resized() override;
    
    class Listener
    {
        
    public:
        
        //BKMultiSlider::Listener() {}
        virtual ~Listener() {};
        
        virtual void multiSliderValueChanged(String name, int whichSlider, Array<float> values) = 0;
        virtual void multiSliderAllValuesChanged(String name, Array<Array<float>> values) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    ListenerList<WantsKeyboardListener> inputListeners;
    void addWantsKeyboardListener(WantsKeyboardListener* listener)     { inputListeners.add(listener);      }
    void removeWantsKeyboardListener(WantsKeyboardListener* listener)  { inputListeners.remove(listener);   }
    
    void setName(String newName)                            { sliderName = newName; showName.setText(sliderName, dontSendNotification);        }
    String getName()                                        { return sliderName; }
    
    Array<Array<float>> getAllValues();
    Array<Array<float>> getAllActiveValues();
    Array<float> getOneSliderBank(int which);

    inline String getText(void){
        return editValsTextField->getText();
    }
    
private:
    
    BKMultiSliderLookAndFeel activeSliderLookAndFeel;
    BKMultiSliderLookAndFeel passiveSliderLookAndFeel;
    BKMultiSliderLookAndFeel highlightedSliderLookAndFeel;
    BKMultiSliderLookAndFeel displaySliderLookAndFeel;
    
    String sliderName;
    BKLabel showName;

    bool dragging;
    bool arrangedHorizontally;
    bool sliderIsVertical;
    bool sliderIsBar;
    int currentSubSlider;
    int lastHighlightedSlider;
    bool focusLostByEscapeKey;
    bool skewFromMidpoint;
    
    Slider::SliderStyle subsliderStyle;
    
    double currentInvisibleSliderValue;
    
    OwnedArray<OwnedArray<BKSubSlider>> sliders;
    ScopedPointer<BKSubSlider> displaySlider;
    ScopedPointer<BKSubSlider> bigInvisibleSlider;
    ScopedPointer<TextEditor> editValsTextField;
    
    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    
    int totalWidth;
    int sliderHeight;
    float sliderWidth;
    int displaySliderWidth;
    
    bool allowSubSliders;
    String subSliderName;
    
    int numActiveSliders;
    int numDefaultSliders;
    int numVisibleSliders;
    
    float clickedHeight;
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    
    void showModifyPopupMenu(int which);
    static void sliderModifyMenuCallback (const int result, BKMultiSlider* slider, int which);
    
    void highlight(int activeSliderNum);
    void deHighlight(int sliderNum);
    int getActiveSlider(int sliderNum);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMultiSlider)
};



// ******************************************************************************************************************** //
// **************************************************  BKSingleSlider ************************************************* //
// ******************************************************************************************************************** //


//basic horizontal slider with its own text box and label
//entering values in the text box will reset the range as needed
class BKSingleSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
{
public:
    BKSingleSlider(String sliderName, double min, double max, double def, double increment);
    ~BKSingleSlider() {};
    
    Slider thisSlider;
    
    String sliderName;
    BKLabel showName;
    bool textIsAbove;
    
    BKTextEditor valueTF;
    
    inline String getText(void) { return valueTF.getText(); }
    inline void setText(String text, NotificationType notify = dontSendNotification) { valueTF.setText(text, notify);}
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    void setTextIsAbove(bool nt)    { textIsAbove = nt; }
    
    void setJustifyRight(bool jr)
    {
        justifyRight = jr;
        if (justifyRight) showName.setJustificationType(Justification::bottomRight);
        else showName.setJustificationType(Justification::bottomLeft);
    }
    
    void setValue(double newval, NotificationType notify);
    void checkValue(double newval);
    double getValue() {return thisSlider.getValue();}
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &e) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    void resized() override;
    
    void setSkewFactor (double factor, bool symmetricSkew) { thisSlider.setSkewFactor(factor, symmetricSkew); }
    void setSkewFactorFromMidPoint (double sliderValueToShowAtMidPoint	) { thisSlider.setSkewFactorFromMidPoint(sliderValueToShowAtMidPoint); }
    
    class Listener
    {
    public:
        
        //BKSingleSlider::Listener() {}
        virtual ~Listener() {};
        
        virtual void BKSingleSliderValueChanged(String name, double val) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    ListenerList<WantsKeyboardListener> inputListeners;
    void addWantsKeyboardListener(WantsKeyboardListener* listener)     { inputListeners.add(listener);      }
    void removeWantsKeyboardListener(WantsKeyboardListener* listener)  { inputListeners.remove(listener);   }
    
    void setDim(float newAlpha);
    void setBright();

private:
    
    double sliderMin, sliderMax;
    double sliderDefault;
    double sliderIncrement;
    
    bool focusLostByEscapeKey;
    
    bool justifyRight;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKSingleSlider)

};



// ******************************************************************************************************************** //
// **************************************************  BKRangeSlider ************************************************** //
// ******************************************************************************************************************** //


class BKRangeSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
{
public:
    BKRangeSlider(String sliderName, double min, double max, double defmin, double defmax, double increment);
    ~BKRangeSlider() {};
    
    Slider minSlider;
    Slider maxSlider;
    String minSliderName;
    String maxSliderName;
    
    Slider invisibleSlider;

    String sliderName;
    BKLabel showName;
    
    BKTextEditor minValueTF;
    BKTextEditor maxValueTF;
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    void setMinValue(double newval, NotificationType notify);
    void setMaxValue(double newval, NotificationType notify);
    void setIsMinAlwaysLessThanMax(bool im) { isMinAlwaysLessThanMax = im; }
    void setJustifyRight(bool jr)
    {
        justifyRight = jr;
        if (justifyRight) showName.setJustificationType(Justification::bottomRight);
        else showName.setJustificationType(Justification::bottomLeft);
    }
    
    inline void setText(BKRangeSliderType which, String text)
    {
        if (which == BKRangeSliderMin)      minValueTF.setText(text, false);
        else if (which == BKRangeSliderMax) maxValueTF.setText(text, false);
    }
    
    inline TextEditor* getTextEditor(BKRangeSliderType which)
    {
        if (which == BKRangeSliderMin) return &minValueTF;
        if (which == BKRangeSliderMax) return &maxValueTF;
        
        return nullptr;
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        if (setValue)
        {
            textEditorReturnKeyPressed(minValueTF);
            textEditorReturnKeyPressed(maxValueTF);
        }
        else
        {
            textEditorEscapeKeyPressed(minValueTF);
            textEditorEscapeKeyPressed(maxValueTF);
        }
    }
    
    void checkValue(double newval);
    void rescaleMinSlider();
    void rescaleMaxSlider();
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    void resized() override;
    void sliderDragEnded(Slider *slider) override;
    void mouseDown (const MouseEvent &event) override;
    
    void setDim(float newAlpha);
    void setBright();
    
    class Listener
    {
        
    public:
        virtual ~Listener() {};
        
        virtual void BKRangeSliderValueChanged(String name, double min, double max) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    ListenerList<WantsKeyboardListener> inputListeners;
    void addWantsKeyboardListener(WantsKeyboardListener* listener)     { inputListeners.add(listener);      }
    void removeWantsKeyboardListener(WantsKeyboardListener* listener)  { inputListeners.remove(listener);   }
    
private:
    
    double sliderMin, sliderMax;
    double sliderDefaultMin, sliderDefaultMax;
    double sliderIncrement;
    
    bool newDrag;
    bool clickedOnMinSlider;
    bool isMinAlwaysLessThanMax;
    bool focusLostByEscapeKey;
    bool justifyRight;
    
    BKRangeMinSliderLookAndFeel minSliderLookAndFeel;
    BKRangeMaxSliderLookAndFeel maxSliderLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKRangeSlider)
    
};



// ******************************************************************************************************************** //
// *******************************************  BKWaveDistanceUndertowSlider ****************************************** //
// ******************************************************************************************************************** //


class BKWaveDistanceUndertowSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
//public BKSingleSlider::Listener,

{
public:
    BKWaveDistanceUndertowSlider();
    ~BKWaveDistanceUndertowSlider() {};
    
    ScopedPointer<Slider> wavedistanceSlider;
    ScopedPointer<Slider> undertowSlider;
    OwnedArray<Slider> displaySliders;
    
    String wavedistanceSliderName;
    String undertowSliderName;
    
    String sliderName;
    BKLabel wavedistanceName;
    BKLabel undertowName;
    
    BKTextEditor wavedistanceValueTF;
    BKTextEditor undertowValueTF;
    
    void setName(String newName)    { sliderName = newName; }
    String getName()                { return sliderName; }
    
    void updateSliderPositions(Array<int> newpositions);
    
    void sliderValueChanged (Slider *slider) override {};
    void resized() override;
    void sliderDragEnded(Slider *slider) override;
    void mouseDoubleClick (const MouseEvent &event) override;
    
    void setWaveDistance(int newwavedist, NotificationType notify);
    void setUndertow(int newundertow, NotificationType notify);
    
    void setDim(float newAlpha);
    void setBright();
    
    inline void setText(NostalgicParameterType which, String text)
    {
        if (which == NostalgicUndertow)             undertowValueTF.setText(text, false);
        else if (which == NostalgicWaveDistance)    wavedistanceValueTF.setText(text, false);
    }
    
    inline TextEditor* getTextEditor(NostalgicParameterType which)
    {
        if (which == NostalgicUndertow) return &undertowValueTF;
        
        if (which == NostalgicWaveDistance) return &wavedistanceValueTF;
        
        return nullptr;
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        if (setValue)
        {
            textEditorReturnKeyPressed(wavedistanceValueTF);
            textEditorReturnKeyPressed(undertowValueTF);
        }
        else
        {
            textEditorEscapeKeyPressed(wavedistanceValueTF);
            textEditorEscapeKeyPressed(undertowValueTF);
        }
    }
    
    class Listener
    {
        
    public:
        
        //BKRangeSlider::Listener() {}
        virtual ~Listener() {};
        
        virtual void BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow) = 0;
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    ListenerList<WantsKeyboardListener> inputListeners;
    void addWantsKeyboardListener(WantsKeyboardListener* listener)     { inputListeners.add(listener);      }
    void removeWantsKeyboardListener(WantsKeyboardListener* listener)  { inputListeners.remove(listener);   }
    
    
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& textEditor) override;
    
private:
    double sliderMin, sliderMax;
    double sliderIncrement;
    
    int maxSliders;
    
    bool newDrag;
    bool clickedOnMinSlider;

    ImageComponent sampleImageComponent;
    
    bool focusLostByEscapeKey;
    
    //BKRangeMinSliderLookAndFeel minSliderLookAndFeel; //possibly need to remake for this
    //BKRangeMaxSliderLookAndFeel maxSliderLookAndFeel;
    BKMultiSliderLookAndFeel displaySliderLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKWaveDistanceUndertowSlider)
    
};



// ******************************************************************************************************************** //
// ************************************************** BKStackedSlider ************************************************* //
// ******************************************************************************************************************** //

class BKStackedSlider :
public Component,
public Slider::Listener,
public TextEditor::Listener
{
public:
    
    BKStackedSlider(String sliderName, double min, double max, double defmin, double defmax, double def, double increment);
    ~BKStackedSlider() {};
    
    void sliderValueChanged (Slider *slider) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void mouseDown (const MouseEvent &event) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent &e) override;
    
    inline BKTextEditor* getTextEditor(void)
    {
        return editValsTextField.get();
    }
    
    inline void dismissTextEditor(bool setValue = false)
    {
        if (setValue)   textEditorReturnKeyPressed(*editValsTextField);
        else            textEditorEscapeKeyPressed(*editValsTextField);
    }
    
    void setTo(Array<float> newvals, NotificationType newnotify);
    void setValue(Array<float> newvals, NotificationType newnotify) { setTo(newvals, newnotify); }
    void resetRanges();
    int whichSlider();
    int whichSlider(const MouseEvent& e);
    void addSlider(NotificationType newnotify);
    
    inline String getText(void) { return editValsTextField->getText(); }
    inline void setText(String text) { editValsTextField->setText(text, dontSendNotification); }
    
    void setName(String newName)    { sliderName = newName; showName.setText(sliderName, dontSendNotification); }
    String getName()                { return sliderName; }
    
    void resized() override;
    
    void setDim(float newAlpha);
    void setBright();
    
    class Listener
    {
        
    public:
        
        virtual ~Listener() {};
        
        virtual void BKStackedSliderValueChanged(String name, Array<float> val) = 0; //rewrite all this to pass "this" and check by slider ref instead of name?
        
        virtual void bkStackedSliderValueChanged(BKStackedSlider*) {};
    };
    
    ListenerList<Listener> listeners;
    void addMyListener(Listener* listener)     { listeners.add(listener);      }
    void removeMyListener(Listener* listener)  { listeners.remove(listener);   }
    
    ListenerList<WantsKeyboardListener> inputListeners;
    void addWantsKeyboardListener(WantsKeyboardListener* listener)     { inputListeners.add(listener);      }
    void removeWantsKeyboardListener(WantsKeyboardListener* listener)  { inputListeners.remove(listener);   }
    
private:
    
    ScopedPointer<Slider> topSlider; //user interacts with this
    OwnedArray<Slider> dataSliders;  //displays data, user controls with topSlider
    Array<bool> activeSliders;
    
    ScopedPointer<BKTextEditor> editValsTextField;
    
    int numSliders;
    int numActiveSliders;
    int clickedSlider;
    float clickedPosition;
    
    String sliderName;
    BKLabel showName;
    bool justifyRight;
    
    BKMultiSliderLookAndFeel stackedSliderLookAndFeel;
    BKMultiSliderLookAndFeel topSliderLookAndFeel;

    double sliderMin, sliderMax, sliderMinDefault, sliderMaxDefault;
    double sliderDefault;
    double sliderIncrement;
    double currentDisplaySliderValue;
    
    bool focusLostByEscapeKey;
    bool focusLostByNumPad;
    bool mouseJustDown;
    
    Array<float> getAllActiveValues();
    
    void showModifyPopupMenu();
    static void sliderModifyMenuCallback (const int result, BKStackedSlider* ss);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKStackedSlider)
};




#endif  // BKSLIDER_H_INCLUDED
