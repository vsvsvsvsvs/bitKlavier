/*==============================================================================

    BKGraph.cpp
    Created: 6 Apr 2017 12:24:29pm
    Author:  Michael R Mulshine

  ==============================================================================*/


#include "BKGraph.h"

#include "PreparationMap.h"

#include "PluginProcessor.h"

#include "BKConstructionSite.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BKItem ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
BKItem::BKItem(BKPreparationType type, int Id, BKAudioProcessor& p):
ItemMapper(type, Id),
BKDraggableComponent(true,false,true, 50, 50, 50, 50),
processor(p),
constrain(new ComponentBoundsConstrainer()),
resizer(new ResizableCornerComponent (this, constrain))
{
    fullChild.setAlwaysOnTop(true);
    addAndMakeVisible(fullChild);
    
    comment.setColour(TextEditor::ColourIds::backgroundColourId, Colours::antiquewhite.withAlpha(0.4f));
    comment.setColour(TextEditor::ColourIds::textColourId, Colours::antiquewhite);
    
    comment.setText("Text here...");
    comment.setMultiLine(true);
    comment.setName("comment");
    comment.setSize(150, 75);
    comment.setScrollbarsShown(false);
    comment.setEnabled(false);
    
#if !JUCE_IOS
    comment.addMouseListener(this, true);
#endif
    
    setPianoTarget(0);
    
    if (type == PreparationTypeTuning)
    {
        setImage(ImageCache::getFromMemory(BinaryData::tuning_icon_png, BinaryData::tuning_icon_pngSize));
    }
    else if (type == PreparationTypeTempo)
    {
        setImage(ImageCache::getFromMemory(BinaryData::tempo_icon_png, BinaryData::tempo_icon_pngSize));
    }
    else if (type == PreparationTypeSynchronic)
    {
        setImage(ImageCache::getFromMemory(BinaryData::synchronic_icon_png, BinaryData::synchronic_icon_pngSize));
    }
    else if (type == PreparationTypeNostalgic)
    {
        setImage(ImageCache::getFromMemory(BinaryData::nostalgic_icon_png, BinaryData::nostalgic_icon_pngSize));
    }
    else if (type == PreparationTypeDirect)
    {
        setImage(ImageCache::getFromMemory(BinaryData::direct_icon_png, BinaryData::direct_icon_pngSize));
    }
    else if (type == PreparationTypeKeymap)
    {
        setImage(ImageCache::getFromMemory(BinaryData::keymap_icon_png, BinaryData::keymap_icon_pngSize));
    }
    else if (type == PreparationTypeGenericMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_unassigned_icon_png, BinaryData::mod_unassigned_icon_pngSize));
    }
    else if (type == PreparationTypeReset)
    {
        setImage(image = ImageCache::getFromMemory(BinaryData::reset_icon_png, BinaryData::reset_icon_pngSize));
    }
    else if (type == PreparationTypeDirectMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_direct_icon_png, BinaryData::mod_direct_icon_pngSize));
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_synchronic_icon_png, BinaryData::mod_synchronic_icon_pngSize));
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_nostalgic_icon_png, BinaryData::mod_nostalgic_icon_pngSize));
    }
    else if (type == PreparationTypeTuningMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_tuning_icon_png, BinaryData::mod_tuning_icon_pngSize));
    }
    else if (type == PreparationTypeTempoMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_tempo_icon_png, BinaryData::mod_tempo_icon_pngSize));
    }
    else if (type == PreparationTypePianoMap)
    {
        setImage(ImageCache::getFromMemory(BinaryData::piano_icon_png, BinaryData::piano_icon_pngSize));
    }
    else if (type == PreparationTypeComment)
    {
        setSize(150*processor.uiScaleFactor,75*processor.uiScaleFactor);
        comment.setSize(150*processor.uiScaleFactor,75*processor.uiScaleFactor);
        constrain->setSizeLimits(50,25,500,500);
        
        addAndMakeVisible (resizer);
        resizer->setAlwaysOnTop(true);
        
        comment.setName("Comment");
    }
    startTimerHz(20);
}

BKItem::~BKItem()
{

}

BKItem* BKItem::duplicate(void)
{
    BKItem* newItem = new BKItem(type, Id, processor);
    
    newItem->setPianoTarget(pianoTarget);
    
    newItem->setCommentText(comment.getText());
    
    newItem->setImage(image);
    
    newItem->setItemName(name);
    
    newItem->setActive(active);
    
    newItem->setBounds(getBounds());
    
    return newItem;
}

/*
void BKItem::bkTextFieldReturnKeyPressed(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    if (name == comment.getName())
    {
        DBG(text);
 
        unfocusAllComponents();
    }
}
 */

void BKItem::configureComment(void)
{
    addAndMakeVisible(comment);
}

void BKItem::configurePianoCB(void)
{
    addAndMakeVisible(menu);
    
    menu.clear();
    
    menu.addListener(this);
    menu.setName(cPreparationTypes[type]);
    
    Piano::PtrArr pianos = processor.gallery->getPianos();
    
    bool targetExists = false;
    for (int i = 0; i < pianos.size(); i++)
    {
        String name = pianos[i]->getName();
        
        if (pianos[i]->getId() == getPianoTarget()) targetExists = true;
        
        if (name != String::empty)
        {
            menu.addItem(pianos[i]->getName(), pianos[i]->getId());
            menu.addSeparator();
        }
    }
    
    if (!targetExists)
    {
        setPianoTarget(processor.currentPiano->getId());
        processor.currentPiano->configure();
    }
    
    menu.setSelectedId(getPianoTarget(), NotificationType::dontSendNotification);
}

void BKItem::setImage(Image newImage)
{
    image = newImage;
    
    placement = RectanglePlacement::centred;
    
    int val =
    (type == PreparationTypeGenericMod && type == PreparationTypeReset) ?  80 :
    (type == PreparationTypePianoMap) ? 120 :
    (type == PreparationTypeKeymap) ? 80 :
    (type == PreparationTypeTempo || type == PreparationTypeTuning) ? 60 :
    65;
    
#if JUCE_IOS 
    val *= processor.uiScaleFactor;
#endif
    
    while (!(image.getWidth() < val || image.getHeight() < val))
        image = image.rescaled(image.getWidth() * 0.9, image.getHeight() * 0.9);
    
    if (type != PreparationTypePianoMap)    setSize(image.getWidth(), image.getHeight());
    else                                    setSize(image.getWidth(), image.getHeight() + 25);

}

void BKItem::setItemType(BKPreparationType newType, bool create)
{
    
    if (type != PreparationTypeGenericMod) setActive(false);

    if (create)
    {
        Id = processor.gallery->getNewId(newType);
        processor.gallery->addTypeWithId(newType, Id);
    }
    
    type = newType;
    
    if (type == PreparationTypeGenericMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_unassigned_icon_png, BinaryData::mod_unassigned_icon_pngSize));
    }
    else if (type == PreparationTypeReset)
    {
        setImage(ImageCache::getFromMemory(BinaryData::reset_icon_png, BinaryData::reset_icon_pngSize));
    }
    else if (type == PreparationTypeDirectMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_direct_icon_png, BinaryData::mod_direct_icon_pngSize));
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_synchronic_icon_png, BinaryData::mod_synchronic_icon_pngSize));
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_nostalgic_icon_png, BinaryData::mod_nostalgic_icon_pngSize));
    }
    else if (type == PreparationTypeTuningMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_tuning_icon_png, BinaryData::mod_tuning_icon_pngSize));
    }
    else if (type == PreparationTypeTempoMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_tempo_icon_png, BinaryData::mod_tempo_icon_pngSize));
    }
    else if (type == PreparationTypePianoMap)
    {
        setImage(ImageCache::getFromMemory(BinaryData::piano_icon_png, BinaryData::piano_icon_pngSize));
        
        configurePianoCB();
    }
    else if (type == PreparationTypeComment)
    {
        
    }
    
    if (type != PreparationTypeGenericMod)
    {
        setActive(true);
        
        processor.currentPiano->add(this);
    }
    
    repaint();
}

void BKItem::paint(Graphics& g)
{
    
    g.setOpacity (1.0f);
    g.drawImage (image, getLocalBounds().toFloat(), placement);
    
    if (isSelected)
    {
        g.setColour(Colours::antiquewhite);
        g.drawRect(getLocalBounds(),2);
    }
    else
    {
        g.setColour(Colours::transparentWhite);
        g.drawRect(getLocalBounds(),0);
    }
    
    
}


void BKItem::resized(void)
{
#if JUCE_IOS
    resizer->setBounds(getWidth()-18, getHeight()-18, 18, 18);
#else
    resizer->setBounds(getWidth()-10, getHeight()-10, 10, 10);
#endif
    
    if (type == PreparationTypePianoMap)
    {
        menu.setBounds(0, image.getHeight(), getWidth(), (processor.platform == BKIOS) ? 15 : 25);
    }
    else if (type == PreparationTypeComment)
    {
        comment.setSize(getWidth(), getHeight());
    }
    else
    {
        label.setBounds(0,0,getWidth(),getHeight());
    }
    
    fullChild.setBounds(0,0,getWidth(),getHeight());
}



void BKItem::copy(BKItem::Ptr itemToCopy)
{
    name = itemToCopy->getItemName();
    position = itemToCopy->getPosition();
    type = itemToCopy->getType();
    Id = itemToCopy->getId();
}

void BKItem::bkComboBoxDidChange    (ComboBox* cb)
{
    String name = cb->getName();
    int pianoId = cb->getSelectedId();
    
    if (name == "PianoMap")
    {
        if (pianoId != getPianoTarget())
        {
            setPianoTarget(pianoId);
            
            processor.currentPiano->configure();
        }
    }
}

void BKItem::itemIsBeingDragged(const MouseEvent& e)
{
    wasJustDragged = true;
}



void BKItem::mouseDoubleClick(const MouseEvent& e)
{
//#if !JUCE_IOS
    if (type == PreparationTypePianoMap)
    {
        menu.showPopup();
    }
    else
    {
        processor.updateState->comment = getCommentText();
        processor.updateState->setCurrentDisplay(type, Id);
    }
//#endif
    
}

#define PHATNESS 5

void BKItem::mouseDown(const MouseEvent& e)
{
    if (e.originalComponent == resizer) resizing = true;
    else                                resizing = false;
        
    BKConstructionSite* cs = ((BKConstructionSite*)getParentComponent());
    BKItem* current = cs->getCurrentItem();
    
    if ((current == this) && !wasJustDragged)
    {
        if (time < PHATNESS)
        {
            if (type == PreparationTypePianoMap)
            {
                menu.showPopup();
            }
            else
            {
                processor.updateState->setCurrentDisplay(type, Id);
            }
        }
        else
        {
            time = 0;
        }
    }
    else
    {
        wasJustDragged = false;
        time = 0;
    }
    
    cs->setCurrentItem(this);
    
    if (isDraggable)
    {
        prepareDrag(e);
    }
    
}

void BKItem::keyPressedWhileSelected(const KeyPress& e)
{
    
}

ValueTree BKItem::getState(void)
{
    ValueTree itemVT( "item");
    
    itemVT.setProperty("name", getItemName(), 0);
    
    itemVT.setProperty("type", type, 0);
    
    if (type != PreparationTypeComment)
    {
        itemVT.setProperty("Id", Id, 0);
        
        itemVT.setProperty("piano", getPianoTarget(), 0);
        
        itemVT.setProperty("active", isActive(), 0);
    }
    else
    {
        itemVT.setProperty("text", getCommentText(), 0);
    }
    
    
    float scale = 1.0f;
#if JUCE_IOS
    scale /= processor.uiScaleFactor;
#endif

    itemVT.setProperty("X", (getX() + getWidth() * 0.5f) * scale, 0);
    itemVT.setProperty("Y", (getY() + getHeight() * 0.5f) * scale, 0);
    
    itemVT.setProperty("W", getWidth() * scale, 0);
    itemVT.setProperty("H", getHeight() * scale, 0);

    return itemVT;
}

void BKItem::setState(XmlElement* e)
{
    String s; bool b; int i;
    
    s = e->getStringAttribute( "name" );
    name = s;
    
    i = e->getStringAttribute( "type" ).getIntValue();
    type = (BKPreparationType)i;
    
    if (type != PreparationTypeComment)
    {
        i = e->getStringAttribute( "Id" ).getIntValue();
        Id = i;
        
        i = e->getStringAttribute( "piano" ).getIntValue();
        setPianoTarget(i);
        b = (bool) e->getStringAttribute( "active" ).getIntValue();
        setActive(b);
    }
    
    i = e->getStringAttribute( "X" ).getIntValue();
    int x = i;
    
    i = e->getStringAttribute( "Y" ).getIntValue();
    int y = i;
    
    setCentrePosition(x,y);
    
    if (type == PreparationTypeComment)
    {
        i = e->getStringAttribute( "W" ).getIntValue();
        int w = i;
        
        i = e->getStringAttribute( "H" ).getIntValue();
        int h = i;
        
        setSize(w, h);
        
        s = e->getStringAttribute( "text" );
        setCommentText(s);
    }
}
    




// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BKGraph ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

BKItemGraph::BKItemGraph(BKAudioProcessor& p):
processor(p)
{
    
}

BKItemGraph::~BKItemGraph(void)
{
    
}

void BKItemGraph::clear()
{
    processor.currentPiano->clearItems();
}

void BKItemGraph::addItem(BKItem* thisItem)
{
    thisItem->setActive(true);
    processor.currentPiano->add(thisItem);
}


void BKItemGraph::removeItem(BKItem* item)
{
    BKItem::PtrArr connections = item->getConnections();
    
    for (int i = connections.size(); --i >= 0;)
    {
        BKItem* connectionItem = connections[i];
        
        connectionItem->removeConnection(item->getType(), item->getId());
    }
    
    item->clearConnections();
    
    item->setActive(false);
    item->setSelected(false);
    
    processor.currentPiano->remove(item);
    
}


bool BKItemGraph::contains(BKItem* thisItem)
{
    bool alreadyThere = false;
    for (auto item : getItems())
    {
        if (item->getType() == BKPreparationTypeNil) continue;
        
        if ((item->getType() == thisItem->getType()) && (item->getId() == thisItem->getId()))
        {
            alreadyThere = true;
            break;
        }
    }
    
    return alreadyThere;
}


bool BKItemGraph::contains(BKPreparationType type, int Id)
{
    bool alreadyThere = false;
    for (auto item : getItems())
    {
        if ((item->getType() == type) && (item->getId() == Id))
        {
            alreadyThere = true;
            break;
        }
    }
    
    return alreadyThere;
}

BKItem::Ptr BKItemGraph::get(BKPreparationType type, int Id)
{
    BKItem::Ptr thisItem = nullptr;

    for (auto item : getItems())
    {
        if (item->getType() == type && item->getId() == Id)
        {
            thisItem = item;
            break;
        }
    }

    return thisItem;
}

void BKItemGraph::clearItems(void)
{
    processor.currentPiano->clearItems();
}

void BKItemGraph::connect(BKPreparationType type1, int id1, BKPreparationType type2, int id2)
{
    BKItem* item1 = get(type1,id1); BKItem* item2 = get(type2,id2);
    connect(item1,item2);
}

void BKItemGraph::connect(BKItem* item1, BKItem* item2)
{
    if ((item1 == item2) || (item1->isConnectedTo(item2) && item2->isConnectedTo(item1))) return;
    
    BKPreparationType item1Type = item1->getType();
    BKPreparationType item2Type = item2->getType();
    
    if (!(isValidConnection(item1Type, item2Type))) return;
    
    if (item1Type == PreparationTypeGenericMod)
    {
        if (item2Type >= PreparationTypeDirect && item2Type <= PreparationTypeTempo)
        {
            item1->setItemType(getModType(item2Type), true);
        }
    }
    else if (item2Type == PreparationTypeGenericMod)
    {
        if (item1Type >= PreparationTypeDirect && item1Type <= PreparationTypeTempo)
        {
            item2->setItemType(getModType(item1Type), true);
        }
    }
    else if (item1Type == PreparationTypeNostalgic && item2Type == PreparationTypeSynchronic)
    {
        BKItem::PtrArr synchronics = item1->getConnectionsOfType(PreparationTypeSynchronic);
        
        for (auto sync : synchronics)
        {
            item1->removeConnection(sync);
            sync->removeConnection(item1);
        }
    }
    else if (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeNostalgic)
    {
        BKItem::PtrArr synchronics = item2->getConnectionsOfType(PreparationTypeSynchronic);
        
        for (auto sync : synchronics)
        {
            item2->removeConnection(sync);
            sync->removeConnection(item2);
        }
    }
    else if (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeTempo)
    {
        BKItem::PtrArr tempos = item1->getConnectionsOfType(PreparationTypeTempo);
        
        for (auto temp : tempos)
        {
            item1->removeConnection(temp);
            temp->removeConnection(item1);
        }
    }
    else if (item1Type == PreparationTypeTempo && item2Type == PreparationTypeSynchronic)
    {
        BKItem::PtrArr tempos = item2->getConnectionsOfType(PreparationTypeTempo);
        
        for (auto temp : tempos)
        {
            item2->removeConnection(temp);
            temp->removeConnection(item2);
        }
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypePianoMap)
    {
        BKItem::PtrArr pianos = item1->getConnectionsOfType(PreparationTypePianoMap);
        
        for (auto piano : pianos)
        {
            item1->removeConnection(piano);
            piano->removeConnection(item1);
        }
    }
    else if (item1Type == PreparationTypePianoMap && item2Type == PreparationTypeKeymap)
    {
        BKItem::PtrArr pianos = item2->getConnectionsOfType(PreparationTypePianoMap);
        
        for (auto piano : pianos)
        {
            item2->removeConnection(piano);
            piano->removeConnection(item2);
        }
    }
    else if ((item1Type >= PreparationTypeDirect && item1Type <= PreparationTypeTempo) && item2Type == PreparationTypeTuning)
    {
        BKItem::PtrArr tunings = item1->getConnectionsOfType(PreparationTypeTuning);
        
        for (auto tune : tunings)
        {
            item1->removeConnection(tune);
            tune->removeConnection(item1);
        }
    }
    else if (item1Type == PreparationTypeTuning && (item2Type >= PreparationTypeDirect && item2Type <= PreparationTypeTempo))
    {
        BKItem::PtrArr tunings = item2->getConnectionsOfType(PreparationTypeTuning);
        
        for (auto tune : tunings)
        {
            item2->removeConnection(tune);
            tune->removeConnection(item2);
        }
    }
    
    item1->addConnection(item2);
    item2->addConnection(item1);

    processor.currentPiano->configure();
}

void BKItemGraph::disconnect(BKItem* item1, BKItem* item2)
{
    BKPreparationType item1Type = item1->getType();
    BKPreparationType item2Type = item2->getType();
    
    item1->removeConnection(item2);
    item2->removeConnection(item1);
    
    if (item1Type == PreparationTypeGenericMod)
    {
        if (!item1->isConnectedToAnyPreparation())
        {
            item1->setItemType(PreparationTypeGenericMod, false);
        }
    }
    else if (item2Type == PreparationTypeGenericMod)
    {
        if (!item2->isConnectedToAnyPreparation())
        {
            item2->setItemType(PreparationTypeGenericMod, false);
        }
    }
}

Array<Line<int>> BKItemGraph::getLines(void)
{
    Array<Line<int>> lines;
    
    for (auto item : getItems())
    {
        BKPreparationType type = item->getType();
    
        if (type == PreparationTypeKeymap)
        {
            for (auto target : item->getConnections())
            {
                Rectangle<int> otherBounds = target->getBounds();
                
                lines.add(Line<int>(item->getX() + item->getWidth()/2.0f,
                                    item->getY() + item->getHeight()/2.0f,
                                    otherBounds.getX() + otherBounds.getWidth()/2.0f,
                                    otherBounds.getY() + otherBounds.getHeight()/2.0f));
            }
        }
        else if (type == PreparationTypeTuning)
        {
            // Look for synchronic, direct, and nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType >= PreparationTypeDirect && targetType <= PreparationTypeNostalgic)
                {
                    Rectangle<int> otherBounds = target->getBounds();
                    
                    lines.add(Line<int>(item->getX() + item->getWidth()/2.0f,
                                        item->getY() + item->getHeight()/2.0f,
                                        otherBounds.getX() + otherBounds.getWidth()/2.0f,
                                        otherBounds.getY() + otherBounds.getHeight()/2.0f));
                }
            }
        }
        else if (type == PreparationTypeTempo)
        {
            // Look for synchronic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType == PreparationTypeSynchronic)
                {
                    Rectangle<int> otherBounds = target->getBounds();
                    
                    lines.add(Line<int>(item->getX() + item->getWidth()/2.0f,
                                        item->getY() + item->getHeight()/2.0f,
                                        otherBounds.getX() + otherBounds.getWidth()/2.0f,
                                        otherBounds.getY() + otherBounds.getHeight()/2.0f));
                }
            }
        }
        else if (type == PreparationTypeSynchronic)
        {
            // Look for nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType == PreparationTypeNostalgic)
                {
                    Rectangle<int> otherBounds = target->getBounds();
                    
                    lines.add(Line<int>(item->getX() + item->getWidth()/2.0f,
                                        item->getY() + item->getHeight()/2.0f,
                                        otherBounds.getX() + otherBounds.getWidth()/2.0f,
                                        otherBounds.getY() + otherBounds.getHeight()/2.0f));
                }
            }
        }
        else if ((type >= PreparationTypeDirectMod && type <= PreparationTypeTempoMod) || type == PreparationTypeReset)
        {
            // Look for nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType != PreparationTypeKeymap)
                {
                    Rectangle<int> otherBounds = target->getBounds();
                    
                    lines.add(Line<int>(item->getX() + item->getWidth()/2.0f,
                                        item->getY() + item->getHeight()/2.0f,
                                        otherBounds.getX() + otherBounds.getWidth()/2.0f,
                                        otherBounds.getY() + otherBounds.getHeight()/2.0f));
                }
            }
        }
    }
    return lines;
}

void BKItemGraph::reconstruct(void)
{
    Piano::Ptr thisPiano = processor.currentPiano;
    
    for (auto item : thisPiano->getItems())
    {
        BKPreparationType type = item->getType();

        if (type == PreparationTypePianoMap)
        {
            item->configurePianoCB();
        }
        else if (type == PreparationTypeComment)
        {
            item->configureComment();
        }
    }
}

BKPreparationType BKItemGraph::getModType(BKPreparationType type)
{
    return (BKPreparationType)(type+6);
}

void BKItemGraph::select(BKItem* item)
{
    item->setSelected(true);
}

void BKItemGraph::deselect(BKItem* item)
{
    item->setSelected(false);
}

void BKItemGraph::deselectAll(void)
{
    for (auto item : processor.currentPiano->getItems())
    {
        item->unfocusAllComponents();
        item->setSelected(false);
    }
}

void BKItemGraph::selectAll(void)
{
    for (auto item : processor.currentPiano->getItems())
        item->setSelected(true);
}

BKItem::PtrArr BKItemGraph::getSelectedItems(void)
{
    BKItem::PtrArr selectedItems;
    
    for (auto item : processor.currentPiano->getItems())
    {
        if (item->getSelected()) selectedItems.add(item);
    }
    
    return selectedItems;
}

Array<int> BKItemGraph::getPreparationIds(BKItem::PtrArr theseItems)
{
    Array<int> whichPreps;
    for (auto item : theseItems) whichPreps.add(item->getId());
    
    return whichPreps;
}

BKItem::PtrArr BKItemGraph::getItems(void)
{
    return processor.currentPiano->getItems();
}

void BKItemGraph::print(void)
{
    DBG("\n~ ~ ~ ~ ~ ~ ~ GRAPH ~ ~ ~ ~ ~ ~ ~:\n");
    for (auto item : processor.currentPiano->getItems())
    {
        item->print();
    }
    DBG("\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
}


bool BKItemGraph::isValidConnection(BKPreparationType type1, BKPreparationType type2)
{
    if (type1 == PreparationTypeDirect)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeTuning ||
            type2 == PreparationTypeDirectMod ||
            type2 == PreparationTypeGenericMod ||
            type2 == PreparationTypeReset)
            return true;
    }
    else if (type1 == PreparationTypeNostalgic)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeTuning ||
            type2 == PreparationTypeNostalgicMod ||
            type2 == PreparationTypeGenericMod ||
            type2 == PreparationTypeSynchronic ||
            type2 == PreparationTypeReset)
            return true;
    }
    else if (type1 == PreparationTypeSynchronic)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeTuning ||
            type2 == PreparationTypeSynchronicMod ||
            type2 == PreparationTypeGenericMod ||
            type2 == PreparationTypeNostalgic ||
            type2 == PreparationTypeTempo ||
            type2 == PreparationTypeReset)
            return true;
    }
    else if (type1 == PreparationTypeTuning)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeSynchronic ||
            type2 == PreparationTypeDirect ||
            type2 == PreparationTypeNostalgic ||
            type2 == PreparationTypeTuningMod ||
            type2 == PreparationTypeGenericMod ||
            type2 == PreparationTypeReset)
            return true;
    }
    else if (type1 == PreparationTypeTempo)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeSynchronic ||
            type2 == PreparationTypeTempoMod ||
            type2 == PreparationTypeGenericMod ||
            type2 == PreparationTypeReset)
            return true;
    }
    else if (type1 == PreparationTypeKeymap)
    {
        if (type2 != PreparationTypeKeymap)
            return true;
    }
    else if (type1 == PreparationTypeDirectMod)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeDirect)
            return true;
    }
    else if (type1 == PreparationTypeNostalgicMod)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeNostalgic)
            return true;
    }
    else if (type1 == PreparationTypeSynchronicMod)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeSynchronic)
            return true;
    }
    else if (type1 == PreparationTypeTuningMod)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeTuning)
            return true;
    }
    else if (type1 == PreparationTypeTempoMod)
    {
        if (type2 == PreparationTypeKeymap ||
            type2 == PreparationTypeTempo)
            return true;
    }
    else if (type1 == PreparationTypeReset)
    {
        if (type2 == PreparationTypeKeymap ||
            (type2 >= PreparationTypeDirect && type2 <= PreparationTypeTempo))
            return true;
    }
    else if (type1 == PreparationTypePianoMap)
    {
        if (type2 == PreparationTypeKeymap)
            return true;
    }
    else if (type1 == PreparationTypeGenericMod)
    {
        if (type2 == PreparationTypeKeymap ||
            (type2 >= PreparationTypeDirect && type2 <= PreparationTypeTempo))
            return true;
    }
    
    
    return false;
}




