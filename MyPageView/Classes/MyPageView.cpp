#include "MyPageView.h"

bool MyPage::init(){
	return CCNode::init();
}

MyPage *MyPage::create(){
	MyPage *p_Page = new MyPage;
	if(p_Page && p_Page->init()){
		p_Page->autorelease();
		return p_Page;
	}else{
		delete p_Page;
		return NULL;
	}	
}

void MyPage::addChild(cocos2d::CCNode * child, int zOrder, int tag)
{
	CCNode::addChild(child,zOrder,tag);
	m_Children.push_back(child);
	if(m_PageView){
		modifyChildPosition(child);
	}
}

void MyPage::addChild(cocos2d::CCNode * child, int zOrder)
{
	CCNode::addChild(child,zOrder,0);
	m_Children.push_back(child);
	if(m_PageView){
		modifyChildPosition(child);
	}
}

void MyPage::addChild(cocos2d::CCNode * child)
{
	CCNode::addChild(child,0,0);
	m_Children.push_back(child);
	if(m_PageView){
		modifyChildPosition(child);
	}
}

void MyPage::modifyChildPosition(cocos2d::CCNode *child)
{
	CCSize  viewsize = m_PageView->getViewSize();
	CCSize  pagesize = m_PageView->GetPageSize();
	CCPoint pos = child->getPosition();
	if(m_PageView->getDirection() == kCCScrollViewDirectionHorizontal){
		pos.x += m_posInPageView.x+(m_pagenum-1)*(pagesize.width/2);
		pos.y += m_posInPageView.y;
	}
	else{
		pos.x += m_posInPageView.x;
		pos.y += m_posInPageView.y+(m_pagenum-1)*(pagesize.height/2);	
	}
	child->setPosition(pos);		
}


void MyPageView::addPage(MyPage *page){
	int pagenum = m_Pages.size() + 1; 
	page->m_pagenum = pagenum;
	m_Pages.push_back(page);
	page->m_PageView = this;
	addChild(page);
	CCPoint viewpos = getPosition();	
	page->m_posInPageView = CCPointZero;
	page->setContentSize(m_PageSize);
	CCSize viewsize = getViewSize();
	if(getDirection() == kCCScrollViewDirectionHorizontal){
		setContentSize(CCSizeMake(m_PageSize.width*pagenum,m_PageSize.height));//设置scrollview区域的大小
		page->m_posInPageView.x += (((pagenum-1)*(m_PageSize.width/2)) + (viewsize.width - m_PageSize.width)/2);
		page->m_posInPageView.y += (viewsize.height - m_PageSize.height)/2;
	}
	else{
		setContentSize(CCSizeMake(m_PageSize.width,m_PageSize.height*pagenum));//设置scrollview区域的大小
		page->m_posInPageView.x += (viewsize.width - m_PageSize.width)/2;
		page->m_posInPageView.y += (((pagenum-1)*(m_PageSize.height/2)) + (viewsize.height - m_PageSize.height)/2);
	}
	page->setPosition(CCPointZero);
	for(size_t i = 0;i < page->m_Children.size(); ++i){
		page->modifyChildPosition(page->m_Children[i]);
	}
}

#define SCROLL_DEACCEL_RATE  0.5f
#define SCROLL_DEACCEL_DIST  5.0f
#define BOUNCE_DURATION      0.15f
#define INSET_RATIO          0.2f
#define MOVE_INCH            7.0f/160.0f

#define TURN_PAGE_SPEED		0.20f				//designPixl/ms
#define INVALID_PAGE		0xfff
#define TURN_PAGE_MIN_OFFSET_RATIO		0.4f

void MyPageView::setCurPage(size_t page){
	if(page > m_Pages.size() || page <= 0) return;
	m_currPage = page;
	CCPoint offset = getContentOffset();
	if(m_eDirection == kCCScrollViewDirectionHorizontal)
		setContentOffset(ccp(-(offset.x+(m_currPage-1)*m_PageSize.width), offset.y));
	else
		setContentOffset(ccp(offset.x, -(offset.y+(m_currPage-1)*m_PageSize.height)));

}

static float convertDistanceFromPointToInch(float pointDis)
{
    float factor = ( CCEGLView::sharedOpenGLView()->getScaleX() + CCEGLView::sharedOpenGLView()->getScaleY() ) / 2;
    return pointDis * factor / CCDevice::getDPI();
}


MyPageView::MyPageView()
: m_fZoomScale(0.0f)
, m_fMinZoomScale(0.0f)
, m_fMaxZoomScale(0.0f)
, m_pDelegate(NULL)
, m_eDirection(kCCScrollViewDirectionBoth)
, m_bDragging(false)
, m_pContainer(NULL)
, m_bTouchMoved(false)
, m_bBounceable(false)
, m_bClippingToBounds(false)
, m_fTouchLength(0.0f)
, m_pTouches(NULL)
, m_fMinScale(0.0f)
, m_fMaxScale(0.0f)
, m_currPage(1)
, m_targetPage(INVALID_PAGE)
, m_touchBeganTime(0)
, m_touchBeganOffset(0)
,m_PageDuration(0.5f)
{
}

MyPageView::~MyPageView()
{
    CC_SAFE_RELEASE(m_pTouches);
    this->unregisterScriptHandler(kScrollViewScroll);
    this->unregisterScriptHandler(kScrollViewZoom);
}


MyPageView* MyPageView::create(CCSize pagesize,int directiontype)
{
    MyPageView* pRet = new MyPageView();
    if (pRet && pRet->init())
    {
		pRet->m_PageSize = pagesize;
		if(directiontype == MyPage_Horizontal)
			pRet->setDirection(kCCScrollViewDirectionHorizontal);
		else
			pRet->setDirection(kCCScrollViewDirectionVertical);
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}


bool MyPageView::initWithViewSize(CCSize size, CCNode *container/* = NULL*/)
{
    if (CCLayer::init())
    {
        m_pContainer = container;
        
        if (!this->m_pContainer)
        {
            m_pContainer = CCLayer::create();
            this->m_pContainer->ignoreAnchorPointForPosition(false);
            this->m_pContainer->setAnchorPoint(ccp(0.0f, 0.0f));
        }

        this->setViewSize(size);

        setTouchEnabled(true);
        m_pTouches = new CCArray();
        m_pDelegate = NULL;
        m_bBounceable = true;
        m_bClippingToBounds = true;
        //m_pContainer->setContentSize(CCSizeZero);
        m_eDirection  = kCCScrollViewDirectionBoth;
        m_pContainer->setPosition(ccp(0.0f, 0.0f));
        m_fTouchLength = 0.0f;
        
        this->addChild(m_pContainer);
        m_fMinScale = m_fMaxScale = 1.0f;
        m_mapScriptHandler.clear();
        return true;
    }
    return false;
}

bool MyPageView::init()
{
    return this->initWithViewSize(CCSizeMake(200, 200), NULL);
}

void MyPageView::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, CCLayer::getTouchPriority(), false);
}

bool MyPageView::isNodeVisible(CCNode* node)
{
    const CCPoint offset = this->getContentOffset();
    const CCSize  size   = this->getViewSize();
    const float   scale  = this->getZoomScale();
    
    CCRect viewRect;
    
    viewRect = CCRectMake(-offset.x/scale, -offset.y/scale, size.width/scale, size.height/scale); 
    
    return viewRect.intersectsRect(node->boundingBox());
}

void MyPageView::pause(CCObject* sender)
{
    m_pContainer->pauseSchedulerAndActions();

    CCObject* pObj = NULL;
    CCArray* pChildren = m_pContainer->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->pauseSchedulerAndActions();
    }
}

void MyPageView::resume(CCObject* sender)
{
    CCObject* pObj = NULL;
    CCArray* pChildren = m_pContainer->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->resumeSchedulerAndActions();
    }

    m_pContainer->resumeSchedulerAndActions();
}

void MyPageView::setTouchEnabled(bool e)
{
    CCLayer::setTouchEnabled(e);
    if (!e)
    {
        m_bDragging = false;
        m_bTouchMoved = false;
        m_pTouches->removeAllObjects();
    }
}

void MyPageView::setContentOffset(CCPoint offset, bool animated/* = false*/)
{
    if (animated)
    { //animate scrolling
        this->setContentOffsetInDuration(offset, BOUNCE_DURATION);
    } 
    else
    { //set the container position directly
        if (!m_bBounceable)
        {
            const CCPoint minOffset = this->minContainerOffset();
            const CCPoint maxOffset = this->maxContainerOffset();
            
            offset.x = MAX(minOffset.x, MIN(maxOffset.x, offset.x));
            offset.y = MAX(minOffset.y, MIN(maxOffset.y, offset.y));
        }

        m_pContainer->setPosition(offset);

        if (m_pDelegate != NULL)
        {
            m_pDelegate->scrollViewDidScroll((CCScrollView*)this);   
        }
    }
}

void MyPageView::setContentOffsetInDuration(CCPoint offset, float dt)
{
    CCFiniteTimeAction *scroll, *expire;
    
    scroll = CCMoveTo::create(dt, offset);
    expire = CCCallFuncN::create(this, callfuncN_selector(MyPageView::stoppedAnimatedScroll));
    m_pContainer->runAction(CCSequence::create(scroll, expire, NULL));
    this->schedule(schedule_selector(MyPageView::performedAnimatedScroll));
}

CCPoint MyPageView::getContentOffset()
{
    return m_pContainer->getPosition();
}

void MyPageView::setZoomScale(float s)
{
    if (m_pContainer->getScale() != s)
    {
        CCPoint oldCenter, newCenter;
        CCPoint center;
        
        if (m_fTouchLength == 0.0f) 
        {
            center = ccp(m_tViewSize.width*0.5f, m_tViewSize.height*0.5f);
            center = this->convertToWorldSpace(center);
        }
        else
        {
            center = m_tTouchPoint;
        }
        
        oldCenter = m_pContainer->convertToNodeSpace(center);
        m_pContainer->setScale(MAX(m_fMinScale, MIN(m_fMaxScale, s)));
        newCenter = m_pContainer->convertToWorldSpace(oldCenter);
        
        const CCPoint offset = ccpSub(center, newCenter);
        if (m_pDelegate != NULL)
        {
            m_pDelegate->scrollViewDidZoom((CCScrollView*)this);
        }
        this->setContentOffset(ccpAdd(m_pContainer->getPosition(),offset));
    }
}

float MyPageView::getZoomScale()
{
    return m_pContainer->getScale();
}

void MyPageView::setZoomScale(float s, bool animated)
{
    if (animated)
    {
        this->setZoomScaleInDuration(s, BOUNCE_DURATION);
    }
    else
    {
        this->setZoomScale(s);
    }
}

void MyPageView::setZoomScaleInDuration(float s, float dt)
{
    if (dt > 0)
    {
        if (m_pContainer->getScale() != s)
        {
            CCActionTween *scaleAction;
            scaleAction = CCActionTween::create(dt, "zoomScale", m_pContainer->getScale(), s);
            this->runAction(scaleAction);
        }
    }
    else
    {
        this->setZoomScale(s);
    }
}

void MyPageView::setViewSize(CCSize size)
{
	if(size.height < m_PageSize.height) size.height = m_PageSize.height;
	if(size.width <  m_PageSize.width)  size.width = m_PageSize.width;
    m_tViewSize = size;
    CCLayer::setContentSize(size);
}

CCNode * MyPageView::getContainer()
{
    return this->m_pContainer;
}

void MyPageView::setContainer(CCNode * pContainer)
{
    // Make sure that 'm_pContainer' has a non-NULL value since there are
    // lots of logic that use 'm_pContainer'.
    if (NULL == pContainer)
        return;

    this->removeAllChildrenWithCleanup(true);
    this->m_pContainer = pContainer;

    this->m_pContainer->ignoreAnchorPointForPosition(false);
    this->m_pContainer->setAnchorPoint(ccp(0.0f, 0.0f));

    this->addChild(this->m_pContainer);

    this->setViewSize(this->m_tViewSize);
}

void MyPageView::relocateContainer(bool animated)
{
    CCPoint oldPoint, min, max;
    float newX, newY;
    
    min = this->minContainerOffset();
    max = this->maxContainerOffset();
    
    oldPoint = m_pContainer->getPosition();

    newX     = oldPoint.x;
    newY     = oldPoint.y;
    if (m_eDirection == kCCScrollViewDirectionBoth || m_eDirection == kCCScrollViewDirectionHorizontal)
    {
        newX     = MAX(newX, min.x);
        newX     = MIN(newX, max.x);
    }

    if (m_eDirection == kCCScrollViewDirectionBoth || m_eDirection == kCCScrollViewDirectionVertical)
    {
        newY     = MIN(newY, max.y);
        newY     = MAX(newY, min.y);
    }

    if (newY != oldPoint.y || newX != oldPoint.x)
    {
        this->setContentOffset(ccp(newX, newY), animated);
    }
}

CCPoint MyPageView::maxContainerOffset()
{
    return ccp(0.0f, 0.0f);
}

CCPoint MyPageView::minContainerOffset()
{
    return ccp(m_tViewSize.width - m_pContainer->getContentSize().width*m_pContainer->getScaleX(), 
               m_tViewSize.height - m_pContainer->getContentSize().height*m_pContainer->getScaleY());
}

void MyPageView::deaccelerateScrolling(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(MyPageView::deaccelerateScrolling));
        return;
    }

    float newX, newY;
    CCPoint maxInset, minInset;
    
    m_pContainer->setPosition(ccpAdd(m_pContainer->getPosition(), m_tScrollDistance));
    
    if (m_bBounceable)
    {
        maxInset = m_fMaxInset;
        minInset = m_fMinInset;
    }
    else
    {
        maxInset = this->maxContainerOffset();
        minInset = this->minContainerOffset();
    }
    
    //check to see if offset lies within the inset bounds
    newX     = MIN(m_pContainer->getPosition().x, maxInset.x);
    newX     = MAX(newX, minInset.x);
    newY     = MIN(m_pContainer->getPosition().y, maxInset.y);
    newY     = MAX(newY, minInset.y);
    
    newX = m_pContainer->getPosition().x;
    newY = m_pContainer->getPosition().y;
    
    m_tScrollDistance     = ccpSub(m_tScrollDistance, ccp(newX - m_pContainer->getPosition().x, newY - m_pContainer->getPosition().y));
    m_tScrollDistance     = ccpMult(m_tScrollDistance, SCROLL_DEACCEL_RATE);
    this->setContentOffset(ccp(newX,newY));
    
    if ((fabsf(m_tScrollDistance.x) <= SCROLL_DEACCEL_DIST &&
         fabsf(m_tScrollDistance.y) <= SCROLL_DEACCEL_DIST) ||
        newY > maxInset.y || newY < minInset.y ||
        newX > maxInset.x || newX < minInset.x ||
        newX == maxInset.x || newX == minInset.x ||
        newY == maxInset.y || newY == minInset.y)
    {
        this->unschedule(schedule_selector(MyPageView::deaccelerateScrolling));
        this->relocateContainer(true);
    }
}

void MyPageView::stoppedAnimatedScroll(CCNode * node)
{
	m_Pages[m_currPage-1]->onPage();
    this->unschedule(schedule_selector(MyPageView::performedAnimatedScroll));
    // After the animation stopped, "scrollViewDidScroll" should be invoked, this could fix the bug of lack of tableview cells.
    if (m_pDelegate != NULL)
    {
        m_pDelegate->scrollViewDidScroll((CCScrollView*)this);
    }
}

void MyPageView::performedAnimatedScroll(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(MyPageView::performedAnimatedScroll));
        return;
    }

    if (m_pDelegate != NULL)
    {
        m_pDelegate->scrollViewDidScroll((CCScrollView*)this);
    }
}


const CCSize& MyPageView::getContentSize() const
{
	return m_pContainer->getContentSize();
}

void MyPageView::setContentSize(const CCSize & size)
{
    if (this->getContainer() != NULL)
    {
        this->getContainer()->setContentSize(size);
		this->updateInset();
    }
}

void MyPageView::updateInset()
{
	if (this->getContainer() != NULL)
	{
		m_fMaxInset = this->maxContainerOffset();
		m_fMaxInset = ccp(m_fMaxInset.x + m_tViewSize.width * INSET_RATIO,
			m_fMaxInset.y + m_tViewSize.height * INSET_RATIO);
		m_fMinInset = this->minContainerOffset();
		m_fMinInset = ccp(m_fMinInset.x - m_tViewSize.width * INSET_RATIO,
			m_fMinInset.y - m_tViewSize.height * INSET_RATIO);
	}
}

/**
 * make sure all children go to the container
 */
void MyPageView::addChild(CCNode * child, int zOrder, int tag)
{
    child->ignoreAnchorPointForPosition(false);
    child->setAnchorPoint(ccp(0.0f, 0.0f));
    if (m_pContainer != child) {
        m_pContainer->addChild(child, zOrder, tag);
    } else {
        CCLayer::addChild(child, zOrder, tag);
    }
}

void MyPageView::addChild(CCNode * child, int zOrder)
{
    this->addChild(child, zOrder, child->getTag());
}

void MyPageView::addChild(CCNode * child)
{
    this->addChild(child, child->getZOrder(), child->getTag());
}

/**
 * clip this view so that outside of the visible bounds can be hidden.
 */
void MyPageView::beforeDraw()
{
    if (m_bClippingToBounds)
    {
		m_bScissorRestored = false;
        CCRect frame = getViewRect();
        if (CCEGLView::sharedOpenGLView()->isScissorEnabled()) {
            m_bScissorRestored = true;
            m_tParentScissorRect = CCEGLView::sharedOpenGLView()->getScissorRect();
            //set the intersection of m_tParentScissorRect and frame as the new scissor rect
            if (frame.intersectsRect(m_tParentScissorRect)) {
                float x = MAX(frame.origin.x, m_tParentScissorRect.origin.x);
                float y = MAX(frame.origin.y, m_tParentScissorRect.origin.y);
                float xx = MIN(frame.origin.x+frame.size.width, m_tParentScissorRect.origin.x+m_tParentScissorRect.size.width);
                float yy = MIN(frame.origin.y+frame.size.height, m_tParentScissorRect.origin.y+m_tParentScissorRect.size.height);
                CCEGLView::sharedOpenGLView()->setScissorInPoints(x, y, xx-x, yy-y);
            }
        }
        else {
            glEnable(GL_SCISSOR_TEST);
            CCEGLView::sharedOpenGLView()->setScissorInPoints(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
        }
    }
}

/**
 * retract what's done in beforeDraw so that there's no side effect to
 * other nodes.
 */
void MyPageView::afterDraw()
{
    if (m_bClippingToBounds)
    {
        if (m_bScissorRestored) {//restore the parent's scissor rect
            CCEGLView::sharedOpenGLView()->setScissorInPoints(m_tParentScissorRect.origin.x, m_tParentScissorRect.origin.y, m_tParentScissorRect.size.width, m_tParentScissorRect.size.height);
        }
        else {
            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void MyPageView::visit()
{
	// quick return if not visible
	if (!isVisible())
    {
		return;
    }

	kmGLPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }

	this->transform();
    this->beforeDraw();

	if(m_pChildren)
    {
		ccArray *arrayData = m_pChildren->data;
		unsigned int i=0;
		
		// draw children zOrder < 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode *child =  (CCNode*)arrayData->arr[i];
			if ( child->getZOrder() < 0 )
            {
				child->visit();
			}
            else
            {
				break;
            }
		}
		
		// this draw
		this->draw();
		
		// draw children zOrder >= 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode* child = (CCNode*)arrayData->arr[i];
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }

    this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }

	kmGLPopMatrix();
}

bool MyPageView::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return false;
    }
    

    CCRect frame = getViewRect();

    //dispatcher does not know about clipping. reject touches outside visible bounds.
    if (m_pTouches->count() > 2 ||
        m_bTouchMoved          ||
        !frame.containsPoint(m_pContainer->convertToWorldSpace(m_pContainer->convertTouchToNodeSpace(touch))))
    {
        return false;
    }

    if (!m_pTouches->containsObject(touch))
    {
        m_pTouches->addObject(touch);
    }

    if (m_pTouches->count() == 1)
    { // scrolling
        m_tTouchPoint     = this->convertTouchToNodeSpace(touch);
        m_bTouchMoved     = false;
        m_bDragging     = true; //dragging started
        m_tScrollDistance = ccp(0.0f, 0.0f);
        m_fTouchLength    = 0.0f;

		__pageTouchBegan();
    }
    else if (m_pTouches->count() == 2)
    {
        m_tTouchPoint  = ccpMidpoint(this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                   this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
        m_fTouchLength = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                   m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
        m_bDragging  = false;
    } 
    return true;
}

void MyPageView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return;
    }

    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bDragging)
        { // scrolling
            CCPoint moveDistance, newPoint, maxInset, minInset;
            CCRect  frame;
            float newX, newY;
            
            frame = getViewRect();

            newPoint     = this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0));
            moveDistance = ccpSub(newPoint, m_tTouchPoint);
            
            float dis = 0.0f;
            if (m_eDirection == kCCScrollViewDirectionVertical)
            {
                dis = moveDistance.y;
            }
            else if (m_eDirection == kCCScrollViewDirectionHorizontal)
            {
                dis = moveDistance.x;
            }
            else
            {
                dis = sqrtf(moveDistance.x*moveDistance.x + moveDistance.y*moveDistance.y);
            }

            if (!m_bTouchMoved && fabs(convertDistanceFromPointToInch(dis)) < MOVE_INCH )
            {
                //CCLOG("Invalid movement, distance = [%f, %f], disInch = %f", moveDistance.x, moveDistance.y);
                return;
            }
            
            if (!m_bTouchMoved)
            {
                moveDistance = CCPointZero;
            }
            
            m_tTouchPoint = newPoint;
            m_bTouchMoved = true;
            
            if (frame.containsPoint(this->convertToWorldSpace(newPoint)))
            {
                switch (m_eDirection)
                {
                    case kCCScrollViewDirectionVertical:
                        moveDistance = ccp(0.0f, moveDistance.y);
                        break;
                    case kCCScrollViewDirectionHorizontal:
                        moveDistance = ccp(moveDistance.x, 0.0f);
                        break;
                    default:
                        break;
                }
                
                maxInset = m_fMaxInset;
                minInset = m_fMinInset;

                newX     = m_pContainer->getPosition().x + moveDistance.x;
                newY     = m_pContainer->getPosition().y + moveDistance.y;

                m_tScrollDistance = moveDistance;
                this->setContentOffset(ccp(newX, newY));
            }
        }
        else if (m_pTouches->count() == 2 && !m_bDragging)
        {
            const float len = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                            m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
            this->setZoomScale(this->getZoomScale()*len/m_fTouchLength);
        }
    }
}

void MyPageView::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return;
    }
	

    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bTouchMoved)
        {

			if( __pageTouchEnd() )
			{
				__pageClearTouch();
			}
			else
			{

            this->schedule(schedule_selector(MyPageView::deaccelerateScrolling));
			}

        }
        m_pTouches->removeObject(touch);
    } 

    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

void MyPageView::ccTouchCancelled(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return;
    }
    m_pTouches->removeObject(touch); 
    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

CCRect MyPageView::getViewRect()
{
    CCPoint screenPos = this->convertToWorldSpace(CCPointZero);
    
    float scaleX = this->getScaleX();
    float scaleY = this->getScaleY();
    
    for (CCNode *p = m_pParent; p != NULL; p = p->getParent()) {
        scaleX *= p->getScaleX();
        scaleY *= p->getScaleY();
    }

    // Support negative scaling. Not doing so causes intersectsRect calls
    // (eg: to check if the touch was within the bounds) to return false.
    // Note, CCNode::getScale will assert if X and Y scales are different.
    if(scaleX<0.f) {
        screenPos.x += m_tViewSize.width*scaleX;
        scaleX = -scaleX;
    }
    if(scaleY<0.f) {
        screenPos.y += m_tViewSize.height*scaleY;
        scaleY = -scaleY;
    }

    return CCRectMake(screenPos.x, screenPos.y, m_tViewSize.width*scaleX, m_tViewSize.height*scaleY);
}

void MyPageView::registerScriptHandler(int nFunID,int nScriptEventType)
{
    this->unregisterScriptHandler(nScriptEventType);
    m_mapScriptHandler[nScriptEventType] = nFunID;
}
void MyPageView::unregisterScriptHandler(int nScriptEventType)
{
    std::map<int,int>::iterator iter = m_mapScriptHandler.find(nScriptEventType);
    
    if (m_mapScriptHandler.end() != iter)
    {
        m_mapScriptHandler.erase(iter);
    }
}
int  MyPageView::getScriptHandler(int nScriptEventType)
{
    std::map<int,int>::iterator iter = m_mapScriptHandler.find(nScriptEventType);
    
    if (m_mapScriptHandler.end() != iter)
        return iter->second;
    
    return 0;
}

void MyPageView::__pageTouchBegan()
{
	if(( m_eDirection != kCCScrollViewDirectionHorizontal && m_eDirection != kCCScrollViewDirectionVertical )) return ;
	m_touchBeganTime = clock();
	m_touchBeganOffset = m_eDirection == kCCScrollViewDirectionHorizontal ? getContentOffset().x : getContentOffset().y;

}
bool MyPageView::__pageTouchEnd()
{
	if(( m_eDirection != kCCScrollViewDirectionHorizontal && m_eDirection != kCCScrollViewDirectionVertical )) return false ;

	//constant
	const float PAGE_DISTENCE = m_eDirection == kCCScrollViewDirectionHorizontal ? m_PageSize.width : m_PageSize.height ;
	if( PAGE_DISTENCE <= 0 ) return false;

	const float MAX_PAGE = m_Pages.size();//( m_eDirection == kCCScrollViewDirectionHorizontal ? getContentSize().width : getContentSize().height ) / PAGE_DISTENCE;
	const float MIN_PAGE = 1;

	float currOffset = m_eDirection == kCCScrollViewDirectionHorizontal ? getContentOffset().x : getContentOffset().y;
	int   oldpage = m_currPage;
	if(m_eDirection == kCCScrollViewDirectionHorizontal){
		int   pagemove = (currOffset - m_touchBeganOffset)/m_PageSize.width;
		int   tarpage;
		if(pagemove > 0){
			//向左滑动
			tarpage = m_currPage - pagemove;
			if(tarpage < 0) m_currPage = 1;
			else m_currPage = tarpage;
			pagemove = abs(oldpage-m_currPage);
			if(pagemove > 1){
				currOffset += pagemove * m_PageSize.width;
			}
		}else if(pagemove < 0){
			//向右滑动
			tarpage = m_currPage - pagemove;
			if(tarpage > (int)m_Pages.size()) m_currPage = (int)m_Pages.size();
			else m_currPage = tarpage;
			if(pagemove > 1){
				currOffset -= pagemove * m_PageSize.width;
			}
		}
	}else{
		int   pagemove = (currOffset - m_touchBeganOffset)/m_PageSize.height;
		int   tarpage;
		if(pagemove > 0){
			//向上滑动
			tarpage = m_currPage - pagemove;
			if(tarpage < 0) m_currPage = 1;
			else m_currPage = tarpage;
			pagemove = abs(oldpage-m_currPage);
			if(pagemove > 1){
				currOffset += pagemove * m_PageSize.height;
			}
		}else if(pagemove < 0){
			//向下滑动
			tarpage = m_currPage - pagemove;
			if(tarpage > (int)m_Pages.size()) m_currPage = (int)m_Pages.size();
			else m_currPage = tarpage;
			if(pagemove > 1){
				currOffset -= pagemove * m_PageSize.height;
			}
		}	
	}

	float deltaOffset = -(currOffset - m_touchBeganOffset);

	if(!(deltaOffset > 0 || deltaOffset < 0)){ 
		m_Pages[m_currPage-1]->onPage();
		if(oldpage != m_currPage)
			m_Pages[oldpage-1]->onPageChange();
		return false;
	}


	clock_t currTime = clock();
	float speed =  currTime != m_touchBeganTime ? deltaOffset / ( currTime - m_touchBeganTime ) : 0;


	m_targetPage = m_currPage;
	if( abs(deltaOffset) >= TURN_PAGE_MIN_OFFSET_RATIO*PAGE_DISTENCE )
	{//滑动距离大于某一阈值.
		
		if( deltaOffset > 0 )
		{
			m_targetPage = m_currPage + 1;
		}
		else if( deltaOffset < 0 )
		{
			m_targetPage = m_currPage - 1;
		}
	}
	else if( abs(speed) >= TURN_PAGE_SPEED )
	{//速度大于某一阈值.
		if( speed > 0 )
		{
			m_targetPage = m_currPage + 1;
		}
		else if( speed < 0 )
		{
			m_targetPage = m_currPage - 1;
		}
	}
	
	if( m_targetPage > MAX_PAGE ) m_targetPage = MAX_PAGE;
	else if( m_targetPage < MIN_PAGE ) m_targetPage = 1;
	
	int delta = m_targetPage - 1;
	float targetOffset = -delta*( m_eDirection == kCCScrollViewDirectionHorizontal ? m_PageSize.width:m_PageSize.height);
	CCPoint targetPointOffset = m_eDirection == kCCScrollViewDirectionHorizontal ? ccp( targetOffset, getContentOffset().y ) : ccp(getContentOffset().x, targetOffset );
	setContentOffsetInDuration(targetPointOffset, m_PageDuration);
	m_currPage = m_targetPage;
	if(oldpage != m_currPage)
		m_Pages[oldpage-1]->onPageChange();
	return true;
}
void MyPageView::__pageTouchCancel()
{
	if(( m_eDirection != kCCScrollViewDirectionHorizontal && m_eDirection != kCCScrollViewDirectionVertical )) return ;

	__pageClearTouch();

}
void MyPageView::__pageClearTouch()
{
	m_touchBeganOffset = 0;
	m_touchBeganTime = 0;
	m_targetPage = m_currPage;
}
