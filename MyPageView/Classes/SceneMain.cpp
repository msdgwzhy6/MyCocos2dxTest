#include "SceneMain.h"
//#include "SceneMgr.h"
#include "cocos2d.h"
#include "MyPageView.h"

USING_NS_CC;


class ShowPage : public MyPage{
public:
	static ShowPage *create(){
		ShowPage *p_Page = new ShowPage;
		if(p_Page && p_Page->init()){
			p_Page->autorelease();
			return p_Page;
		}else{
			delete p_Page;
			return NULL;
		}	
	}
	
	virtual bool init(){
		return MyPage::init();
	}
	virtual void onPage(){
		CCSprite *s = (CCSprite*)getChildByTag(1);
		if(s) s->setScale(1.2f);
	}
	
	virtual void onPageChange(){
		CCSprite *s = (CCSprite*)getChildByTag(1);
		if(s) s->setScale(1.0f);
	}

protected:
	ShowPage(){}
};

bool SceneMain::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	CCSize winsize = CCDirector::sharedDirector()->getWinSize();
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	{
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("gujidao_01.png");
		CCSprite* pSprite = CCSprite::createWithTexture(texture,CCRect(0,0,300,200));
		pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
		this->addChild(pSprite);
	}

	CCSize pagesize(100,200);

	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("Main.png");
	MyPageView *pageview = MyPageView::create(pagesize);
	pageview->setContentOffset(CCPointZero);
	pageview->setViewSize(CCSize(300,200));
	ShowPage *page1 = ShowPage::create();
	ShowPage *page2 = ShowPage::create();
	ShowPage *page3 = ShowPage::create();
	ShowPage *page4 = ShowPage::create();

	CCSize pageviewsize = CCSize(300,150);

	CCSprite* pSprite1 = CCSprite::createWithTexture(texture,CCRect(0,0,pagesize.width-20,pagesize.height-20));
	pSprite1->setPosition(ccp(pagesize.width/2 + origin.x, pagesize.height/2 + origin.y));

	CCSprite* pSprite2 = CCSprite::createWithTexture(texture,CCRect(180,0,pagesize.width-20,pagesize.height-20));
	pSprite2->setPosition(ccp(pagesize.width/2 + origin.x, pagesize.height/2 + origin.y));


	CCSprite* pSprite3 = CCSprite::createWithTexture(texture,CCRect(280,0,pagesize.width-20,pagesize.height-20));
	pSprite3->setPosition(ccp(pagesize.width/2 + origin.x, pagesize.height/2 + origin.y));

	CCSprite* pSprite4 = CCSprite::createWithTexture(texture,CCRect(380,0,pagesize.width-20,pagesize.height-20));
	pSprite4->setPosition(ccp(pagesize.width/2 + origin.x, pagesize.height/2 + origin.y));


	//pageview->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	pageview->setPosition(ccp(90, 60));
	page1->addChild(pSprite1,0,1);
	page2->addChild(pSprite2,0,1);
	page3->addChild(pSprite3,0,1);
	page4->addChild(pSprite4,0,1);
	pageview->addPage(page1);
	pageview->addPage(page2);
	pageview->addPage(page3);
	pageview->addPage(page4);
	pageview->setCurPage(2);
	pageview->setPageDuration(0.2f);
	this->addChild(pageview);


	this->scheduleUpdate();
	return true;
}

void SceneMain::update(float tick){
}

CCScene* SceneMain::scene()
{
	// 'scene' is an autorelease object
	CCScene* pScene = CCScene::create();
	SceneMain *pSM = SceneMain::create();
	pScene->addChild(pSM);
	return pScene;
}
