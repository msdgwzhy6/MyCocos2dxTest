#include "ResourcesReader.h"
#include "cocos2d.h"
#include "SceneMain.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

ResourcesReader *ResourcesReader::m_pInst = NULL;
const int AniFrameIdle = 31;
const int AniFrameRun = 25;
const int AniFrameAtk = 33;
const int AniFrameHurt = 20;
const unsigned int TotalFrameNum = 12;


ResourcesReader::ResourcesReader()
{
	bRoleInit = false;
	bSceneInit = false;
	m_nFileNum = 0;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Run.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Atk.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Knife.wav");
}

ResourcesReader::~ResourcesReader()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect("Run.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect("Atk.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect("Knife.wav");
}

void ResourcesReader::InitRole()
{
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_idle_east.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_idle_north.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_idle_south.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_idle_west.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_east.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_west.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_north_west.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_north.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_north_east.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_south.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_south_west.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
	CCTextureCache::sharedTextureCache()->addImageAsync("Cat/Cat_run_south_east.png",this,callfuncO_selector(ResourcesReader::loadingRoleCallBack));
}


extern SceneMain *g_scenemain;

void ResourcesReader::loadingRoleCallBack(cocos2d::CCObject *obj)
{
	m_nFileNum++;
	if (m_nFileNum >= TotalFrameNum)
	{
		CCTexture2D *texture;

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_idle_east.png");
		m_RoleAni[RT_CAT].Ani[AT_Idle][4] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][4]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][4]->retain();

		m_RoleAni[RT_CAT].Ani[AT_Idle][5] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160+512,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][5]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][5]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_idle_north.png");
		m_RoleAni[RT_CAT].Ani[AT_Idle][2] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][2]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][2]->retain();

		m_RoleAni[RT_CAT].Ani[AT_Idle][3] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160+512,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][3]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][3]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_idle_south.png");
		m_RoleAni[RT_CAT].Ani[AT_Idle][6] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][6]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][6]->retain();

		m_RoleAni[RT_CAT].Ani[AT_Idle][7] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160+512,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][7]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][7]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_idle_west.png");
		m_RoleAni[RT_CAT].Ani[AT_Idle][0] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][0]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][0]->retain();

		m_RoleAni[RT_CAT].Ani[AT_Idle][1] = CCArray::create();
		for (int a=0;a<16;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160+512,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Idle][1]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Idle][1]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_north.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][north] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][north]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][north]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_south.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][south] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][south]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][south]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_east.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][east] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][east]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][east]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_west.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][west] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][west]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][west]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_north_east.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][north_east] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][north_east]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][north_east]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_north_west.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][north_west] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][north_west]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][north_west]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_south_east.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][south_east] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][south_east]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][south_east]->retain();

		texture = CCTextureCache::sharedTextureCache()->textureForKey("Cat/Cat_run_south_west.png");
		m_RoleAni[RT_CAT].Ani[AT_Run][south_west] = CCArray::create();
		for (int a=0;a<26;a++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture,CCRect(a%6*160,a/6*160,160,160));		
			m_RoleAni[RT_CAT].Ani[AT_Run][south_west]->addObject(frame);
		}
		m_RoleAni[RT_CAT].Ani[AT_Run][south_west]->retain();
		g_scenemain->onAsynLoadFinish();
	}
}