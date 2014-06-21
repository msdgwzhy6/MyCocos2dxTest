#ifndef _MYPAGEVIEW_H
#define _MYPAGEVIEW_H

#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
using namespace cocos2d::extension;

class MyPageView;

enum {
	MyPage_Horizontal,//水平翻页奥
	MyPage_Vertical,  //垂直翻页
};

class MyPage : public cocos2d::CCNode{
	friend class MyPageView;
public:
	static MyPage *create();
	virtual bool init();
    virtual void addChild(cocos2d::CCNode * child, int zOrder, int tag);
    virtual void addChild(cocos2d::CCNode * child, int zOrder);
    virtual void addChild(cocos2d::CCNode * child);

	virtual void onPage(){}			//切换到当前页时调用
	virtual void onPageChange(){}   //从当前页切换到其他页时调用

protected:
	MyPage():m_PageView(NULL){}
	void modifyChildPosition(cocos2d::CCNode *child);
	int m_pagenum;
	MyPageView                   *m_PageView;
	cocos2d::CCPoint              m_posInPageView;//当前页面在整个pageview中的相对起始坐标
	std::vector<cocos2d::CCNode*> m_Children;
};

class MyPageView : public CCLayer
{
public:

	void setPageDuration(float duration) {m_PageDuration = duration;}
	void addPage(MyPage*);    //增加一个页面
	int  getCurPage(){return m_currPage;}

	void setCurPage(size_t page);

	const CCSize &GetPageSize(){return m_PageSize;} 
    /**
     *  @js ctor
     */
    MyPageView();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~MyPageView();

    bool init();
    virtual void registerWithTouchDispatcher();

    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static MyPageView* create(CCSize pagesize, int directiontype = MyPage_Horizontal);

	bool initWithViewSize(CCSize size, CCNode *container/* = NULL*/);

    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     *
     * @param offset new offset
     * @param If YES, the view scrolls to the new offset
     */
    void setContentOffset(CCPoint offset, bool animated = false);
    CCPoint getContentOffset();
    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     * You can override the animation duration with this method.
     *
     * @param offset new offset
     * @param animation duration
     */
    void setContentOffsetInDuration(CCPoint offset, float dt); 

    void setZoomScale(float s);
    /**
     * Sets a new scale and does that for a predefined duration.
     *
     * @param s a new scale vale
     * @param animated if YES, scaling is animated
     */
    void setZoomScale(float s, bool animated);

    float getZoomScale();

    /**
     * Sets a new scale for container in a given duration.
     *
     * @param s a new scale value
     * @param animation duration
     */
    void setZoomScaleInDuration(float s, float dt);
    /**
     * Returns the current container's minimum offset. You may want this while you animate scrolling by yourself
     */
    CCPoint minContainerOffset();
    /**
     * Returns the current container's maximum offset. You may want this while you animate scrolling by yourself
     */
    CCPoint maxContainerOffset(); 
    /**
     * Determines if a given node's bounding box is in visible bounds
     *
     * @return YES if it is in visible bounds
     */
    bool isNodeVisible(CCNode * node);
    /**
     * Provided to make scroll view compatible with SWLayer's pause method
     */
    void pause(CCObject* sender);
    /**
     * Provided to make scroll view compatible with SWLayer's resume method
     */
    void resume(CCObject* sender);


    bool isDragging() {return m_bDragging;}
    bool isTouchMoved() { return m_bTouchMoved; }
    bool isBounceable() { return m_bBounceable; }
    void setBounceable(bool bBounceable) { m_bBounceable = bBounceable; }

    /**
     * size to clip. CCNode boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    CCSize getViewSize() { return m_tViewSize; } 
    void setViewSize(CCSize size);

    CCNode * getContainer();
    void setContainer(CCNode * pContainer);

    /**
     * direction allowed to scroll. CCScrollViewDirectionBoth by default.
     */
    CCScrollViewDirection getDirection() { return m_eDirection; }
    virtual void setDirection(CCScrollViewDirection eDirection) { m_eDirection = eDirection; }

    CCScrollViewDelegate* getDelegate() { return m_pDelegate; }
    void setDelegate(CCScrollViewDelegate* pDelegate) { m_pDelegate = pDelegate; }

    /** override functions */
    // optional
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

    virtual void setContentSize(const CCSize & size);
    virtual const CCSize& getContentSize() const;

	void updateInset();
    /**
     * Determines whether it clips its children or not.
     */
    bool isClippingToBounds() { return m_bClippingToBounds; }
    void setClippingToBounds(bool bClippingToBounds) { m_bClippingToBounds = bClippingToBounds; }


	 virtual void onEnterTransitionDidFinish(){
		 if(!m_Pages.empty())
			 m_Pages[m_currPage-1]->onPage();
	 }
    /**
     *  @js NA
     */
    virtual void visit();
    virtual void addChild(CCNode * child, int zOrder, int tag);
    virtual void addChild(CCNode * child, int zOrder);
    virtual void addChild(CCNode * child);
    void setTouchEnabled(bool e);
private:
    /**
     * Relocates the container at the proper offset, in bounds of max/min offsets.
     *
     * @param animated If YES, relocation is animated
     */
    void relocateContainer(bool animated);
    /**
     * implements auto-scrolling behavior. change SCROLL_DEACCEL_RATE as needed to choose
     * deacceleration speed. it must be less than 1.0f.
     *
     * @param dt delta
     */
    void deaccelerateScrolling(float dt);
    /**
     * This method makes sure auto scrolling causes delegate to invoke its method
     */
    void performedAnimatedScroll(float dt);
    /**
     * Expire animated scroll delegate calls
     */
    void stoppedAnimatedScroll(CCNode* node);
    /**
     * clip this view so that outside of the visible bounds can be hidden.
     */
    void beforeDraw();
    /**
     * retract what's done in beforeDraw so that there's no side effect to
     * other nodes.
     */
    void afterDraw();
    /**
     * Zoom handling
     */
    void handleZoom();

protected:
    CCRect getViewRect();
    
    /**
     * current zoom scale
     */
    float m_fZoomScale;
    /**
     * min zoom scale
     */
    float m_fMinZoomScale;
    /**
     * max zoom scale
     */
    float m_fMaxZoomScale;
    /**
     * scroll view delegate
     */
    CCScrollViewDelegate* m_pDelegate;

    CCScrollViewDirection m_eDirection;
    /**
     * If YES, the view is being dragged.
     */
    bool m_bDragging;

    /**
     * Content offset. Note that left-bottom point is the origin
     */
    CCPoint m_tContentOffset;

    /**
     * Container holds scroll view contents, Sets the scrollable container object of the scroll view
     */
    CCNode* m_pContainer;
    /**
     * Determiens whether user touch is moved after begin phase.
     */
    bool m_bTouchMoved;
    /**
     * max inset point to limit scrolling by touch
     */
    CCPoint m_fMaxInset;
    /**
     * min inset point to limit scrolling by touch
     */
    CCPoint m_fMinInset;
    /**
     * Determines whether the scroll view is allowed to bounce or not.
     */
    bool m_bBounceable;

    bool m_bClippingToBounds;

    /**
     * scroll speed
     */
    CCPoint m_tScrollDistance;
    /**
     * Touch point
     */
    CCPoint m_tTouchPoint;
    /**
     * length between two fingers
     */
    float m_fTouchLength;
    /**
     * UITouch objects to detect multitouch
     */
    CCArray* m_pTouches;
    /**
     * size to clip. CCNode boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    CCSize m_tViewSize;
    /**
     * max and min scale
     */
    float m_fMinScale, m_fMaxScale;
    /**
     * scissor rect for parent, just for restoring GL_SCISSOR_BOX
     */
    CCRect m_tParentScissorRect;
    bool m_bScissorRestored;

	//Page Info
public:
	float getDistanceRatioOfTurn(){ return m_distanceRatioOfTurn;};
	void setDistanceRatioOfTurn( float value ){ m_distanceRatioOfTurn = value;};
protected:
	clock_t m_touchBeganTime;
	int m_touchBeganOffset;
	int m_targetPage;
	int m_currPage;
	float m_pageAccSpeed;
	float m_distanceRatioOfTurn;

	void __pageTouchBegan();
	bool __pageTouchEnd();
	void __pageTouchCancel();
	void __pageClearTouch();

public:
    enum ScrollViewScriptEventType
    {
        kScrollViewScroll   = 0,
        kScrollViewZoom,
    };
    void registerScriptHandler(int nFunID,int nScriptEventType);
    void unregisterScriptHandler(int nScriptEventType);
    int  getScriptHandler(int nScriptEventType);
private:
    std::map<int,int>    m_mapScriptHandler;
	std::vector<MyPage*> m_Pages;          //所有页面
	CCSize               m_PageSize;       //每页的大小
	float                m_PageDuration;   //页面滚动时间
};

#endif
