/*
 * CCControlPicker.m
 *
 * Copyright 2013 Yannick Loriot. All rights reserved.
 * http://yannickloriot.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "CCControlPicker.h"

USING_NS_CC_EXT;

#define CCControlPickerFriction         0.70f   // Between 0 and 1
#define CCControlPickerDefaultRowWidth  35 * CC_CONTENT_SCALE_FACTOR()      // px
#define CCControlPickerDefaultRowHeight 35 * CC_CONTENT_SCALE_FACTOR()      // px

CCControlPicker::CCControlPicker() {
	background = 0;
	delegate = 0;
	dataSource = 0;
	rowsLayer = 0;
	_foregroundSprite = 0;
	_selectionSprite = 0;
	_availableRows = new CCArray();
}
CCControlPicker::~CCControlPicker()
{
//    SAFE_ARC_RELEASE(_previousDate);
//	CC_SAFE_RELEASE(rowsLayer);
    CC_SAFE_RELEASE(background);
	
	CC_SAFE_RELEASE(_availableRows);
    
//    SAFE_ARC_SUPER_DEALLOC();
}

CCControlPicker* CCControlPicker::initWithForegroundSprite(CCNode * foregroundSprite)
{
    return initWithForegroundSpriteSelectionSprite(foregroundSprite, 0);
}

CCControlPicker* CCControlPicker::pickerWithForegroundSprite(CCNode * foregroundSprite)
{
	CCControlPicker* picker = new CCControlPicker;
	picker->initWithForegroundSprite(foregroundSprite);
	picker->autorelease();
    return picker;
}

CCControlPicker* CCControlPicker::pickerWithForegroundFile(const std::string& foregroundFile)
{
    // Prepare the foreground for the picker
    CCSprite *foregroundSprite  = CCSprite::create(foregroundFile.c_str());
    
    return pickerWithForegroundSprite(foregroundSprite);
}

CCControlPicker* CCControlPicker::initWithForegroundSpriteSelectionSprite(CCNode *foregroundSprite, CCNode * selectionSprite)
{
    if (init())
    {
		_foregroundSprite = 0;
		_selectionSprite = 0;

        CCAssert(foregroundSprite,   "Foreground sprite must be not nil");
        
        this->decelerating                   = false;
        this->ignoreAnchorPointForPosition( false );
        this->setContentSize( foregroundSprite->getContentSize() );
        
        cachedRowCount                     = 0;
        _selectedRow = -1 ;
        highlightRow                       = -1;
        cacheRowSize                       = CCSizeMake(CCControlPickerDefaultRowWidth,
                                                         CCControlPickerDefaultRowHeight);
        swipeOrientation                   = CCControlPickerOrientationVertical;
        looping                            = false;
        
        CCPoint center                      = ccp (getContentSize().width / 2, getContentSize().height /2);
		foregroundSprite->setContentSize(getContentSize());
        foregroundSprite->setPosition( center );
        addChild(foregroundSprite, 3);
		setForegroundSprite(foregroundSprite);
        
        rowsLayer                      = CCLayer::create();
        addChild(rowsLayer, 1);
        
        if (selectionSprite)
        {
			selectionSprite->setContentSize(CCSizeMake(getContentSize().width, CCControlPickerDefaultRowHeight));
            selectionSprite->setPosition( center );
            addChild(selectionSprite, 2);
        }
		setSelectionSprite(selectionSprite);
    }
    return this;
}

CCControlPicker* CCControlPicker::pickerWithForegroundSpriteSelectionSprite(CCNode * foregroundSprite, CCNode * selectionSprite)
{
	CCControlPicker* picker = new CCControlPicker;
	picker->initWithForegroundSpriteSelectionSprite(foregroundSprite, selectionSprite);
	picker->autorelease();
    return picker;
}

CCControlPicker* CCControlPicker::pickerWithForegroundFileSelectionFile(const std::string &foregroundFile, const std::string & selectionFile)
{
    // Prepare the foreground for the picker
    CCSprite *foregroundSprite  = CCSprite::create(foregroundFile.c_str());
    
    // Prepare the selection for the picker
    CCSprite *selectionSprite   = CCSprite::create(selectionFile.c_str());
    
    return pickerWithForegroundSpriteSelectionSprite(foregroundSprite, selectionSprite);
}

void CCControlPicker::onEnter()
{
	superClass::onEnter();
    
	this->registerWithTouchDispatcher();

	scheduleUpdate();
    
    reloadComponent();
}

void CCControlPicker::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);

    unscheduleUpdate();
    
	superClass::onExit();
}

void CCControlPicker::visit()
{
	if (!isVisible())
        return;
    
    CCPoint worldOrg    = convertToWorldSpace(ccp(0, 0));
    CCPoint dest        = convertToWorldSpace(ccp(getContentSize().width, getContentSize().height));
    CCPoint dims        = ccpSub(dest, worldOrg);
    
	glEnable(GL_SCISSOR_TEST);
		
    CCRect scissorRect  = CCRectMake(worldOrg.x, worldOrg.y, dims.x, dims.y);
//    scissorRect         = CC_RECT_POINTS_TO_PIXELS(scissorRect);
    
    glScissor(scissorRect.origin.x, scissorRect.origin.y,
              scissorRect.size.width, scissorRect.size.height);

	superClass::visit();
    
	glDisable(GL_SCISSOR_TEST);
}

void CCControlPicker::update(float delta)
{
	if (rowsLayer->numberOfRunningActions() > 0)
		updateVisibleRows();

	if (!decelerating) {
        return;
	}
    
    if (velocity.y <= cacheRowSize.height && velocity.y >= -cacheRowSize.height)
    {
        decelerating           = false;
        
        unsigned int rowNumber    = rowNumberAtLocation(rowsLayer->getPosition());
        selectRow(rowNumber, true, true);
        return;
    }
    
    CCPoint tranlation      = ccp (velocity.x * delta, velocity.y * delta);
    rowsLayer->setPosition(     positionWithTranslationForLayerPosition(tranlation, rowsLayer->getPosition()) );
	
	if (rowsLayer->numberOfRunningActions() <= 0)
		updateVisibleRows();
	
    sendPickerRowEventForPosition(rowsLayer->getPosition());
    
    // Update the new velocity
    velocity               = ccp(velocity.x * CCControlPickerFriction,
                                  velocity.y * CCControlPickerFriction);
}

#pragma mark Properties

void CCControlPicker::setBackground(CCNode *nbackground)
{
    if (background)
    {
        removeChild(background,true);
        CC_SAFE_RELEASE(background);
    }
    
    if (nbackground) {
		nbackground->retain();
		background = nbackground;
		
        addChild(background, 0);
	}
}

#pragma mark - CCControlPicker Public Methods

cocos2d::CCSize CCControlPicker::rowSize()
{
    return cacheRowSize;
}

unsigned int CCControlPicker::numberOfRows()
{
    return 0;
}

void CCControlPicker::reloadComponent()
{
	if (!rowsLayer)
		return;

	cachedRowCount        = 0;
    
    if (dataSource)
        cachedRowCount     = dataSource->numberOfRowsInControlPicker(this);
    
//	rowsLayer->removeAllChildrenWithCleanup(true);
    needsLayoutWithRowCount(cachedRowCount);
}

void CCControlPicker::selectRow(unsigned int row, bool animated, bool sendCallback)
{
	if (!rowsLayer)
		return;
	
    CCPoint dest = rowsLayer->getPosition();
    
    if (swipeOrientation == CCControlPickerOrientationVertical)
        dest.y  = cacheRowSize.height * row;
    else
        dest.x  = cacheRowSize.width * row;

    _selectedRow = row;
    
    // Send events
    if (highlightRow != _selectedRow && highlightRow != -1)
    {
        CCControlPickerRowDelegate* rowNode  = dynamic_cast<CCControlPickerRowDelegate*>(rowsLayer->getChildByTag(_selectedRow));
		if (rowNode)
			rowNode->rowDidDownplayed();
    }
    CCControlPickerRowDelegate* rowNode  = dynamic_cast<CCControlPickerRowDelegate*>(rowsLayer->getChildByTag(_selectedRow));
	if (rowNode)
		rowNode->rowWillBeSelected();
    highlightRow   = -1;
    
    if (animated)
    {
		if (sendCallback) {
			rowsLayer->runAction(CCSequence::create(
													CCEaseInOut::create(
																		
																		CCMoveTo::create(0.2f, dest), 1.0f),
													CCCallFunc::create(this, callfunc_selector(CCControlPicker::updateVisibleRows)),
													CCCallFunc::create(this, callfunc_selector(CCControlPicker::sendSelectedRowCallback)),0));
		} else {
			rowsLayer->runAction(CCSequence::create(
													CCEaseInOut::create(
																		CCMoveTo::create(0.2f, dest), 1.0f),
													CCCallFunc::create(this, callfunc_selector(CCControlPicker::updateVisibleRows)),
													0));
		}
    } else {
		updateVisibleRows();
		
		if (sendCallback) {
			sendSelectedRowCallback();
		}
    }
}

CCControlPickerRow* CCControlPicker::dequeueUnusedControlPickerRow() {
	CCControlPickerRow* result = 0;
	if (_availableRows->count() > 0) {
		result = dynamic_cast<CCControlPickerRow*>(_availableRows->lastObject());
	}
	return result;
}

void CCControlPicker::updateVisibleRows() {
	CCPoint center      = ccp (0.0f, getContentSize().height + cacheRowSize.height / 2.0f);
	unsigned int firstRow = rowNumberAtLocation(ccpAdd(rowsLayer->getPosition(), ccpSub(CCPointZero, center)));

	const int visibleRowCount = ceilf(getContentSize().height / cacheRowSize.height) + 4;

    center      = ccp (getContentSize().width / 2, getContentSize().height /2);
	
	int allocatedNode = 0;
	
    for (unsigned int i = 0; i < _rowLayout.size(); ++i)
    {
		RowInfos& rowInfo = _rowLayout[i];
		
		if (rowInfo._rowNode)
			++allocatedNode;
		
        CCPoint position        = rowInfo._position;
		
		CCControlPickerRow *row = 0;
		
		if (i >= firstRow && i <= firstRow + visibleRowCount) {
			if (!rowInfo._rowNode) {
				row = dataSource->controlPickerNodeForRow(this, i);
				if (row) {
					rowsLayer->addChild(row, 1, i);
					_availableRows->removeObject(row);
				
					row->setTag(i);
					row->fitRowInSize(cacheRowSize);
					row->setAnchorPoint( ccp(0.5f, 0.5f) );
					row->setPosition( position );
					rowInfo._rowNode = row;
				}
			}
		} else {
			if (rowInfo._rowNode) {
				_availableRows->addObject( rowInfo._rowNode );
				rowsLayer->removeChild(rowInfo._rowNode, true);
				rowInfo._rowNode = 0;
			}
		}
	}	
}

int CCControlPicker::selectedRow()
{
    return _selectedRow;
}

#pragma mark - CCControlPicker Private Methods

void CCControlPicker::needsLayoutWithRowCount(unsigned int rowCount)
{
    CCPoint center      = ccp (getContentSize().width / 2, getContentSize().height /2);
    
    if (delegate)
    {
        cacheRowSize   = delegate->rowSizeForControlPicker(this);
    }
    
	const int visibleRowCount = ceilf(getContentSize().height / cacheRowSize.height) + 2;
	
    for (unsigned int i = 0; i < _rowLayout.size(); ++i) {
		RowInfos& rowInfo = _rowLayout[i];
		if (rowInfo._rowNode) {
			_availableRows->addObject( rowInfo._rowNode );
			rowsLayer->removeChild(rowInfo._rowNode, true);
		}
	}
	
	_rowLayout.clear();
	rowsLayer->removeAllChildrenWithCleanup(true);
	
    for (unsigned int i = 0; i < rowCount; i++)
    {
        CCPoint position        = center;
        if (swipeOrientation == CCControlPickerOrientationVertical)
            position.y          += -cacheRowSize.height * i;
        else
            position.x          += cacheRowSize.width * i;

		CCControlPickerRow *row = 0;
		
//		if (i < visibleRowCount) {
//			row = dataSource->controlPickerNodeForRow(this, i);
//			row->fitRowInSize(cacheRowSize);
//			row->setAnchorPoint( ccp(0.5f, 0.5f) );
//			rowsLayer->addChild(row, 1, i);
//			row->setPosition( position );
//		}
    
		_rowLayout.push_back( RowInfos( position, row ) );
	}
    
    if (0 && looping)
    {
        CCSize pickerSize = getContentSize();
        int numberOfRowToLoop = 0;
        
        if (swipeOrientation == CCControlPickerOrientationVertical)
            numberOfRowToLoop = pickerSize.height / rowSize().height;
        else
            numberOfRowToLoop = pickerSize.width / rowSize().width;
        
        for (int i = 1; i <= numberOfRowToLoop; i++)
        {
            CCControlPickerRow *row_sub = dataSource->controlPickerNodeForRow(this, rowCount - i);
            row_sub->fitRowInSize(cacheRowSize);
            row_sub->setAnchorPoint( ccp(0.5f, 0.5f) );
            rowsLayer->addChild(row_sub, 1);
            
            CCControlPickerRow *row_over = dataSource->controlPickerNodeForRow(this, (i - 1) % rowCount);
            row_over->fitRowInSize(cacheRowSize);
            row_over->setAnchorPoint( ccp(0.5f, 0.5f) );
            rowsLayer->addChild(row_over, 1);
            
            if (swipeOrientation == CCControlPickerOrientationVertical)
            {
                row_sub->setPosition( ccpAdd(center, ccp(0, cacheRowSize.height * i)) );
                row_over->setPosition( ccpAdd(center, ccp(0, -cacheRowSize.height * (rowCount + i - 1))) );
            } else
            {
                row_sub->setPosition( ccpAdd(center, ccp(-cacheRowSize.width * i, 0)) );
                row_over->setPosition( ccpAdd(center, ccp(cacheRowSize.height * (rowCount + i - 1), 0)) );
            }
        }
    }
    
    // Defines the limit bounds for non-circular picker
    limitBounds    = CCRectMake(-cacheRowSize.width * (cachedRowCount - 1),
                                 0,
                                 0,
                                 cacheRowSize.height * (cachedRowCount - 1));
    
//    selectRow(0, false, false);
}

bool CCControlPicker::isValueOutOfMinBoundMaxBound(double value, double min, double max)
{
    return  (value <= min || max <= value);
}

unsigned int CCControlPicker::rowNumberAtLocation(CCPoint location)
{
    if (swipeOrientation == CCControlPickerOrientationVertical)
    {
        if (location.y < limitBounds.origin.y)
            return 0;
        else if (location.y >= limitBounds.size.height)
            return cachedRowCount - 1;
        else
        {
            unsigned int row  = floorf(location.y / cacheRowSize.height);
            if (row == cachedRowCount)
                row         = 0;
            
            return row;
        }
    } else {
        if (location.x < limitBounds.origin.x)
            return cachedRowCount - 1;
        else if (location.x >= limitBounds.size.width)
            return 0;
        else
        {
            unsigned int row  = round(fabsf(location.x) / cacheRowSize.width);
            if (row == cachedRowCount)
                row         = 0;
            
            return row;
        }
    }
}

cocos2d::CCPoint CCControlPicker::positionWithTranslationForLayerPosition(CCPoint translation, CCPoint position)
{
    if (swipeOrientation == CCControlPickerOrientationVertical)
    {
        position.y      -= adjustTranslationForAxisValueUsingMinBoundMaxBound(translation.y,
																			  position.y,
																			  limitBounds.origin.y,
																			  limitBounds.size.height);
        
        if (0 && looping)
        {
            if (position.y < limitBounds.origin.y)
            {
                double diff = limitBounds.origin.y - position.y;
                position.y  = limitBounds.size.height + cacheRowSize.height - diff;
            } else if ((limitBounds.size.height + cacheRowSize.height) < position.y)
            {
                double diff = position.y - (limitBounds.size.height + cacheRowSize.height);
                position.y  = limitBounds.origin.y + diff;
            }
        }
    } else
    {
        position.x      -= adjustTranslationForAxisValueUsingMinBoundMaxBound(translation.x,
																			  position.x,
																			  limitBounds.origin.x,
																			  limitBounds.size.width);
        
        if (0 && looping)
        {
            if (position.x < limitBounds.origin.x)
            {
                double diff = limitBounds.origin.x - position.x;
                position.x  = limitBounds.size.width + cacheRowSize.width - diff;
            } else if ((limitBounds.size.width + cacheRowSize.width) < position.x)
            {
                double diff = position.x - (limitBounds.size.width + cacheRowSize.width);
                position.x  = limitBounds.origin.x + diff;
            }
        }
    }
    
    return position;
}

double CCControlPicker::adjustTranslationForAxisValueUsingMinBoundMaxBound(double tranlation, double axis, double min, double max)
{
    // If the picker is not circular we check if we have reached an edge
    if ((1 || !looping) && isValueOutOfMinBoundMaxBound(axis, min, max))
    {
        double d1       = fabsf(min - axis);
        double d2       = fabsf(max - axis);
        
        double friction = exp(std::min(d1, d2) / 30.0f) + 1.0f;
        
        return tranlation / friction;
    } else
    {
        return tranlation;
    }
}

void CCControlPicker::setContentSize(const CCSize & size)
{
	CCSize s = getContentSize();
	superClass::setContentSize(size);
	if (size.equals( CCSizeZero ) || s.equals(size))
		return;

	CCPoint center                      = ccp (size.width / 2, size.height /2);

	if (_foregroundSprite) {
		_foregroundSprite->setContentSize(size);
        _foregroundSprite->setPosition( center );
	}
	if (_selectionSprite) {
		CCSize selectionSize = CCSizeMake(getContentSize().width, rowSize().height * 2);
		if (delegate) {
//			selectionSize = delegate->rowSizeForControlPicker(this);
		}
		_selectionSprite->setContentSize(selectionSize);
		_selectionSprite->setPosition( center );
	}
	reloadComponent();
	if (_selectedRow != -1)
		selectRow(selectedRow(), false, false);
}

void CCControlPicker::sendSelectedRowCallback()
{    
    if (delegate)
    {
        delegate->controlPickerDidSelectRow(this, _selectedRow);
    }
    
    // Notifie the row
    CCControlPickerRowDelegate* rowNode  = dynamic_cast<CCControlPickerRowDelegate*>(rowsLayer->getChildByTag(_selectedRow));
	if (rowNode)
		rowNode->rowDidSelected();
}

void CCControlPicker::sendPickerRowEventForPosition(CCPoint location)
{
    unsigned int nhighlightRow = rowNumberAtLocation(rowsLayer->getPosition());
    
    if (highlightRow != nhighlightRow)
    {
        CCControlPickerRowDelegate* rowNode = 0;
        if (highlightRow != -1)
        {
            rowNode     = dynamic_cast<CCControlPickerRowDelegate*>(rowsLayer->getChildByTag(highlightRow));
			if (rowNode)
				rowNode->rowDidDownplayed();
        }
        
        highlightRow               = nhighlightRow;
		rowNode     = dynamic_cast<CCControlPickerRowDelegate*>(rowsLayer->getChildByTag(highlightRow));
		if (rowNode)
			rowNode->rowDidHighlighted();
    }
}

void CCControlPicker::initMoveWithActionLocation(CCPoint location)
{
    setSelected( false );
    
    rowsLayer->stopAllActions();
    
    decelerating       = false;
    previousLocation   = location;

    struct cc_timeval now;
	
    if (CCTime::gettimeofdayCocos2d(&now, NULL) != 0)
    {
        CCLOG("error in gettimeofday");
    }

    previousDate   = now.tv_usec;
    
    // Update the cell layer position
    CCPoint translation = ccpSub(previousLocation, location);
    rowsLayer->setPosition( positionWithTranslationForLayerPosition(translation, rowsLayer->getPosition() ) );
	updateVisibleRows();
}

void CCControlPicker::updateMoveWithActionLocation(CCPoint location)
{
    // Update the cell layer position
    CCPoint translation     = ccpSub(location, previousLocation);
    rowsLayer->setPosition(     positionWithTranslationForLayerPosition(translation, rowsLayer->getPosition() ) );
    updateVisibleRows();
	
    // Sends the picker's row event
    sendPickerRowEventForPosition(rowsLayer->getPosition());
    
    // Compute the current velocity
    struct cc_timeval now;
	
    if (CCTime::gettimeofdayCocos2d(&now, NULL) != 0)
    {
        CCLOG("error in gettimeofday");
    }

    double delta_time       = (now.tv_usec - previousDate) / 1000.0 / 1000.0;
    CCPoint delta_position  = ccpSub(location, previousLocation);
    velocity               = ccp(delta_position.x / delta_time, delta_position.y / delta_time);
	if (fabsf(velocity.x) > 1000.0f)
		velocity.x = 0.0f;
	if (fabsf(velocity.y) > 1000.0f)
		velocity.y = 0.0f;
	
	CCLOG("delta_time: %f delta_position: %f velocity: %f", delta_time, delta_position.y, velocity.y);
	CCAssert(!isinf(velocity.x) && !isinf(velocity.y), "error velocity");
    
    // Update the previous location and date
    previousLocation       = location;
    previousDate       = now.tv_usec;
}

void CCControlPicker::endMoveWithActionLocation(CCPoint location)
{
    setSelected(false);
    decelerating   = true;
}

#pragma mark -
#pragma mark CCTargetedTouch Delegate Methods

bool CCControlPicker::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (!isTouchInside(pTouch)
        || !isEnabled()
        || !isVisible()
        || !hasVisibleParents())
        return false;

    CCPoint touchLocation   = pTouch->getLocation();
    touchLocation           = CCDirector::sharedDirector()->convertToGL(touchLocation);
    touchLocation           = getParent()->convertToNodeSpace(touchLocation);
    
    initMoveWithActionLocation(touchLocation);
    
    return true;
}

void CCControlPicker::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint touchLocation   = pTouch->getLocation();
    touchLocation           = CCDirector::sharedDirector()->convertToGL(touchLocation);
    touchLocation           = getParent()->convertToNodeSpace(touchLocation);
    
    updateMoveWithActionLocation(touchLocation);
}

void CCControlPicker::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint touchLocation   = pTouch->getLocation();
//    touchLocation           = CCDirector::sharedDirector()->convertToGL(touchLocation);
    CCPoint localTouchLocation           = convertToNodeSpace(pTouch->getLocation());
    touchLocation           = getParent()->convertToNodeSpace(touchLocation);
    
	if (pTouch->getTapCount() == 1) {
		CCPoint center      = ccp (0.0f, getContentSize().height - cacheRowSize.height / 2.0f);
		
		CCPoint touchPos = CCPointMake(rowsLayer->getPosition().x + getContentSize().width / 2.0f, rowsLayer->getPosition().y - (localTouchLocation.y - getContentSize().height / 2.0f));
		unsigned int nhighlightRow = rowNumberAtLocation(touchPos);
		selectRow(nhighlightRow, true, true);
	} else {
		endMoveWithActionLocation(touchLocation);
	}
}

void CCControlPicker::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    ccTouchEnded(pTouch, pEvent);
}


#pragma mark - CCControlPickerRow

CCControlPickerRow::CCControlPickerRow() {
	textLabel = 0;
}
CCControlPickerRow::~CCControlPickerRow()
{
//    CC_SAFE_RELEASE(textLabel);
}

bool CCControlPickerRow::init()
{
//    if (superClass::init())
    {
        CCSize defaultSize              = CCSizeMake(CCControlPickerDefaultRowWidth, CCControlPickerDefaultRowHeight);

        textLabel                      = CCLabelTTF::create("",
															 "HelveticaNeue-Bold",
															 18 * CC_CONTENT_SCALE_FACTOR(),
															 CCSizeMake(CCControlPickerDefaultRowWidth,
																		CCControlPickerDefaultRowHeight),
															 kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
		
        textLabel->setVerticalAlignment( kCCVerticalTextAlignmentCenter );
        textLabel->setColor(                ccc3(0, 0, 0));
        textLabel->setAnchorPoint( ccp(0.5f, 0.5f) );
		textLabel->ignoreAnchorPointForPosition(true);
        textLabel->setPosition( ccp (CCControlPickerDefaultRowWidth / 2, CCControlPickerDefaultRowHeight / 2) );

        addChild(textLabel, 1);
        
        setContentSize( defaultSize );
    }
	return true;
}

CCControlPickerRow* CCControlPickerRow::initWithTitle(const std::string& title)
{
    if (init())
    {
        textLabel->setString( title.c_str() );
    }
    return this;
}

CCControlPickerRow* CCControlPickerRow::rowWithTitle(const std::string &title)
{
	CCControlPickerRow* row = new CCControlPickerRow;
	row->initWithTitle(title);
	row->autorelease();
    return row;
}

void CCControlPickerRow::setTitle(const std::string& title) {
	if (textLabel)
		textLabel->setString( title.c_str() );
}


#pragma mark Properties

#pragma mark - Public Methods

void CCControlPickerRow::fitRowInSize(CCSize size)
{
    CCPoint center          = ccp(size.width / 2, size.height / 2);

    setContentSize( size );
	
    textLabel->setDimensions( size );
    textLabel->setContentSize( size );
    textLabel->setPosition( CCPointMake(0, -center.y / 2) );
}

#pragma mark - CCControlPickerRow Delegate Methods

void CCControlPickerRow::rowDidHighlighted()
{
//    textLabel->setFontSize( 24.0f );
//    textLabel->setColor( ccc3(201, 62, 119) );
}

void CCControlPickerRow::rowDidDownplayed()
{
//    textLabel->setFontSize( 18.0f );
//    textLabel->setColor( ccc3(86, 86, 86) );
}

void CCControlPickerRow::rowWillBeSelected()
{
//    textLabel->setFontSize( 29.0f );
//    textLabel->setColor( ccc3(201, 62, 119) );
}

void CCControlPickerRow::rowDidSelected()
{
    
}

