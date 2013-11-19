#pragma once

#include "CCControl.h"
#include "CCInvocation.h"

NS_CC_EXT_BEGIN

/**
 * Swipe orientation.
 */
typedef enum
{
    CCControlPickerOrientationVertical,
    CCControlPickerOrientationHorizontal
} CCControlPickerOrientation;

class CCControlPickerDataSource;
class CCControlPickerDelegate;
class CCControlPickerRow;

/**
 * Picker control for Cocos2D.
 *
 * The CCControlPicker class implements objects, called control pickers, that
 * use a spinning-wheel or slot-machine metaphor to show one set of values.
 * Users select values by rotating the wheels so that the desired row of values
 * aligns with a selection indicator.
 *
 * @see http://yannickloriot.com/library/ios/cccontrolextension/Classes/CCControlPicker.html
 */
class CCControlPicker : public CCControl {
public:
	typedef CCControl superClass;
	
	CCControlPicker();
	virtual ~CCControlPicker();
	
	virtual void onEnter();
	virtual void onExit();
	virtual void visit();
	virtual void update(float delta);
	virtual void setContentSize(const CCSize & size);
	
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	
#pragma mark Contructors - Initializers
	/** @name Create Pickers */
	
	/**
	 * @abstract Initializes a picker by only defining the foreground sprite (with no
	 * selection indicator).
	 * @param foregroundSprite foreground sprite which defines the area of the picker.
	 * @see initWithForegroundSprite:selectionSprite:
	 */
	CCControlPicker* initWithForegroundSprite(CCNode *foregroundSprite);
	
	/**
	 * @abstract Creates a picker by only defining the foreground sprite (with no
	 * selection indicator).
	 * @see initWithForegroundSprite:
	 */
	static CCControlPicker* pickerWithForegroundSprite(CCNode * foregroundSprite);
	
	/**
	 * @abstract Creates a picker by only defining the foreground filename (with no
	 * selection indicator).
	 * @see pickerWithForegroundSprite:
	 */
	static CCControlPicker* pickerWithForegroundFile(const std::string& foregroundFile);
	
	/**
	 * @abstract Initializes a picker by defining the foreground and the selection sprites.
	 * @param foregroundSprite foreground sprite which defines the area of the picker.
	 * @param selectionSprite  selection indicator sprite.
	 */
	CCControlPicker* initWithForegroundSpriteSelectionSprite(CCNode * foregroundSprite, CCNode * selectionSprite);
	
	/**
	 * @abstract Creates a picker by defining the foreground and the selection sprites.
	 * @see initWithForegroundSprite:selectionSprite:
	 */
	static CCControlPicker* pickerWithForegroundSpriteSelectionSprite(CCNode * foregroundSprite, CCNode *selectionSprite);
	
	/**
	 * @abstract Creates a picker by defining the foreground and the selection filenames.
	 * @see pickerWithForegroundSprite:selectionSprite:
	 */
	static CCControlPicker* pickerWithForegroundFileSelectionFile(const std::string& foregroundFile, const std::string& selectionFile);
	
#pragma mark - Properties
#pragma mark Changing the Picker’s Appearance
	/** @name Changing the Picker’s Appearance */
	
	/**
	 * @abstract Contains the node that is drawn on the background of the picker.
	 * @discussion The node you specify should fit within the bounding rectangle of
	 * the foreground sprite. If it does not, the node will be clip.
	 *
	 * This default value of this property is nil.
	 */
	CCNode* background;
	void setBackground(CCNode *nbackground);
	
#pragma mark Getting the Dimensions of the Control Picker
	/** @name Getting the Dimensions of the Control Picker */
	/**
	 * @abstract Returns the size of a row.
	 * @return The size of rows.
	 * @discussion A picker control fetches the value of this property by
	 * calling the rowHeightInControlPicker: delegate methods, and caches
	 * it. The default value is CCControlPickerDefaultRowHeight.
	 */
	CCSize rowSize();
	
	/**
	 * @abstract Returns the number of rows.
	 * @return The number of rows.
	 * @discussion A picker control fetches the value of this property
	 * from the data source and and caches it. The default value is zero.
	 */
	unsigned int numberOfRows();
	
#pragma mark Managing the Behavior of the Control Picker
	/** @name Managing the Behavior of the Control Picker */
	
	/**
	 * @abstract The swipe orientation of the picker.
	 * @discussion The orientation constrains the swipe direction.
	 * E.g if the orientation is set to CCControlPickerOrientationVertical
	 * the element can move in vertical only.
	 *
	 * The default value for this property is CCControlPickerOrientationVertical.
	 */
	CC_SYNTHESIZE(CCControlPickerOrientation, swipeOrientation, SwipeOrientation);
	
	/**
	 * @abstract The looping vs. nonlooping state of the picker.
	 * @discussion If YES, the picker will display the data source as a
	 * loop. I.e that when the end of the source is reached the picker
	 * will display the first element.
	 *
	 * The default value for this property is NO.
	 */
	CC_SYNTHESIZE(bool, looping, IsLooping);
	
#pragma mark Specifying the Delegate
	/** @name Specifying the Delegate */
	/**
	 * @abstract The delegate for the control picker.
	 * @discussion The delegate must adopt the CCControlPickerDelegate protocol
	 * and implement the required methods to respond to new selections or
	 * deselections.
	 */
	CC_SYNTHESIZE(CCControlPickerDelegate*, delegate, Delegate);
	
#pragma mark Specifying the Data Source
	/** @name Specifying the Data Source */
	/**
	 * @abstract The data source for the control picker.
	 * @discussion The data source must adopt the CCControlPickerDataSource
	 * protocol and implement the required methods to return the number of
	 * rows in each component.
	 */
	CC_SYNTHESIZE(CCControlPickerDataSource*, dataSource, DataSource);
	
#pragma mark - Public Methods
#pragma mark Reloading the Control Picker
	/** @name Reloading the Control Picker */
	
	/**
	 * Reloads the component of the picker control.
	 */
	void reloadComponent();
	
#pragma mark Selecting Rows in the Control Picker
	/** @name Selecting Rows in the Control Picker */
	
	/**
	 * Selects a row in the picker control.
	 * @param row A zero-indexed number identifying a row of component.
	 * @param animated YES to animate the selection by spinning the wheel
	 * (component) to the new value; if you specify NO, the new selection
	 * is shown immediately.
	 */
	void selectRow(unsigned int row, bool animated, bool sendCallback = false);
	
	/**
	 * Returns the index of the selected row.
	 * @return A zero-indexed number identifying the selected row , or -1
	 * if no row is selected.
	 * @see selectRow:animated:
	 */
	int selectedRow();
	
public:
	// Scroll Animation
	CC_SYNTHESIZE(bool, decelerating, isDecelerating);
	CC_SYNTHESIZE(CCPoint, previousLocation, PreviousLocation);
	CC_SYNTHESIZE(CCPoint, velocity, Velocity);
	CC_SYNTHESIZE(CCRect, limitBounds, LimitBounds);
	CC_SYNTHESIZE(int, previousDate, PreviousDate);
	CC_SYNTHESIZE(unsigned int, highlightRow, HighlightRow);
	// Picker
	CC_SYNTHESIZE(CCLayer*, rowsLayer, RowsLayer);
	CC_SYNTHESIZE(unsigned int, cachedRowCount, CachedRowCount);
	CC_SYNTHESIZE(int, _selectedRow, SelectedRow);
	CC_SYNTHESIZE(CCSize, cacheRowSize, CacheRowSize);
	
	CC_SYNTHESIZE(CCNode *, _foregroundSprite, ForegroundSprite);
	CC_SYNTHESIZE(CCNode *, _selectionSprite, SelectionSprite);
	
	/** Layout the picker with the number given row count. */
	void needsLayoutWithRowCount(unsigned int rowCount);
	
	/** Returns YES whether the given value is out of the given bounds. */
	bool isValueOutOfMinBoundMaxBound(double value, double min, double max);
	
	/** Returns the row number at the closest location. */
	unsigned int rowNumberAtLocation(CCPoint location);
	
	/** Apply the given translation to the given position and return it. */
	CCPoint positionWithTranslationForLayerPosition(CCPoint translation, CCPoint position);
	
	/** Returns the translation to apply using the given axis location
	 * value and the bounds of the control picker. */
	double adjustTranslationForAxisValueUsingMinBoundMaxBound(double tranlation, double axis, double min, double max);
	
	/** Calls the delegate to warn it that the selected row has changed. */
	void sendSelectedRowCallback();
	
	/** Send to the picker's rows the appropriate events. */
	void sendPickerRowEventForPosition(CCPoint location);
	
	// Manage mouse/touch events
	void initMoveWithActionLocation(CCPoint location);
	void updateMoveWithActionLocation(CCPoint location);
	void endMoveWithActionLocation(CCPoint location);
	
	struct RowInfos {
		RowInfos(CCPoint p, CCControlPickerRow* r = 0) : _position(p), _rowNode(r) {}
		CCPoint _position;
		CCControlPickerRow* _rowNode;
	};
	std::vector< RowInfos > _rowLayout;
	CCArray* _availableRows;
//	std::vector< CCControlPickerRow* > _visibleRows;
	void updateVisibleRows();
	
	CCControlPickerRow* dequeueUnusedControlPickerRow();
};

#pragma mark - CCControlPickerRowDelegate

/**
 * The CCControlPickerRowDelegate class allows the receiver to respond to the
 * CCControlPicker's events. By implementing these methods you can improve the
 * user experience with appropriate visuals.
 */
class CCControlPickerRowDelegate {
public:
	
#pragma mark Responding to Control Picker Events
	/** @name Responding to Control Picker Events */
	
	/**
	 * @abstract Notifies the row that enters under the selection node.
	 * @discussion You can implement this method to perform additional tasks
	 * associated with presenting the view.
	 */
	virtual void rowDidHighlighted() {}
	
	/**
	 * @abstract Notifies the row that leaves the selection node.
	 * @discussion You can implement this method to perform additional tasks
	 * associated with presenting the view.
	 */
	virtual void rowDidDownplayed() {}
	
	/**
	 * @abstract Notifies the row that will be selected.
	 * @discussion You can implement this method to perform additional tasks
	 * associated with presenting the view.
	 */
	virtual void rowWillBeSelected() {}
	
	/**
	 * @abstract Notifies the row that is selected.
	 * @discussion You can implement this method to perform additional tasks
	 * associated with presenting the view.
	 */
	virtual void rowDidSelected() {}
	
};

#pragma mark - CCControlPickerRowNode

/**
 * The CCControlPickerRow class implements the row node representation for
 * the CCControlPicker.
 *
 * A row node implements some methods and callbacks to make the
 * CCControlPicker customization more easier.
 *
 * @see http://yannickloriot.com/library/ios/cccontrolextension/Classes/CCControlPickerRow.html
 */
class CCControlPickerRow : public CCNode, public CCControlPickerRowDelegate {
public:
	typedef CCNode superClass;
	
	CCControlPickerRow();
	virtual ~CCControlPickerRow();
	
	virtual bool init();
	
#pragma mark Contructors - Initializers
	/** @name Create Picker' Rows */
	
	/** Initializes a simple row node with the content title. */
	virtual CCControlPickerRow* initWithTitle(const std::string& title);
	/** Creates a simple row node with the content title. */
	static CCControlPickerRow* rowWithTitle(const std::string& title);
	
	virtual void setTitle(const std::string& title);

#pragma mark Managing Text as Row Content
	/** @name Managing Text as Row Content */
	
	/**
	 * @abstract Returns the label used for the main textual content of
	 * the control picker row. (read-only)
	 * @discussion Holds the main label of the row. CCControlPickerRow
	 * adds an appropriate label when you create the row.
	 */
	CC_SYNTHESIZE_READONLY(CCLabelTTF*, textLabel, TextLabel);
	
#pragma mark Managing Row Size
	/** @name Managing Row Size */
	
	/**
	 * @abstract Called when the size must resize.
	 * @param size The size that the row should fit.
	 * @discussion The method is called by the CCControlPicker when its needed.
	 * The control picker uses the size defined by the rowSizeForControlPicker:
	 * method of the CCControlPickerDelegate.
	 *
	 * You have to override this method to layout your cell correctly.<br />
	 * (*do not forget to call the super [super fitRowInSize:size])*
	 */
	virtual void fitRowInSize(CCSize size);
	
	virtual void rowDidHighlighted();
	
	virtual void rowDidDownplayed();
	
	virtual void rowWillBeSelected();
	
	virtual void rowDidSelected();
	
};

#pragma mark - CCControlPickerDataSource

/**
 * The CCControlPickerDataSource protocol must be adopted by an object
 * that mediates between a CCControlPicker object and your application’s
 * data model for that control picker. The data source provides the control
 * picker with the number of components, and the number of rows in the
 * component, for displaying the control picker data.
 * Both methods in this protocol are required.
 */
class CCControlPickerDataSource {
public:
	
#pragma mark Providing Counts for the Control Picke
	/** @name Providing Counts for the Control Picker */
	
	/**
	 * Called by the picker control when it needs the number of rows. (required)
	 * @param controlPicker The picker control requesting the data.
	 * @return The number of rows.
	 */
	virtual unsigned int numberOfRowsInControlPicker(CCControlPicker *controlPicker) { return 0; }
	
#pragma mark Setting the Content of Component Rows
	/** @name Setting the Content of Component Rows */
	
	/**
	 * @abstract Called by the picker control when it needs the node to use for a given row.
	 * @param controlPicker An object representing the control picker requesting the data.
	 * @param row A zero-indexed number identifying a row of component. Rows are
	 * numbered top-to-bottom.
	 * @return The node to use as the visual representation of the indicated row.
	 */
	virtual CCControlPickerRow * controlPickerNodeForRow(CCControlPicker *controlPicker, unsigned int row) = 0;
	
};

#pragma mark - CCControlPickerDelegate

/**
 * The delegate of a CCControlPicker object must adopt this protocol and
 * implement at least some of its methods to provide the control picker with
 * the data it needs to construct itself.
 *
 * Typically the delegate implements optional methods to respond to new
 * selections or deselections of component rows.
 *
 * See CCControlPicker Class Reference for a discussion of components, rows,
 * row content, and row selection.
 */
class CCControlPickerDelegate {
public:
	
#pragma mark Setting the Dimensions of the Control Picker row
	/** @name Setting the Dimensions of the Control Picker's row */
	
	/**
	 * @abstract Called by the control picker when it needs the row size to use for drawing
	 * row content.
	 * @param controlPicker The control picker requesting this information.
	 * @return A CGSize indicating the size of the row in points.
	 */
	virtual CCSize rowSizeForControlPicker(CCControlPicker * controlPicker) { return CCSizeZero; }
	
#pragma mark Responding to Row Selection
	/** @name Responding to Row Selection */
	/**
	 * @abstract Called by the control picker when the user selects a row.
	 * @param controlPicker An object representing the control picker view
	 * requesting the data.
	 * @param row A zero-indexed number identifying a row of component.
	 * Rows are numbered top-to-bottom.
	 * @discussion To determine what value the user selected, the delegate
	 * uses the row index to access the value at the corresponding position
	 * in the array used to construct the component.
	 */
	virtual void controlPickerDidSelectRow(CCControlPicker * controlPicker, unsigned int row) {}
	
};

NS_CC_EXT_END
