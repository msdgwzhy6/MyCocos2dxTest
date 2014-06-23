#include "entity.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;
using namespace cocos2d::extension;


CEntity *CEntity::createWithTexture(CCTexture2D *pTexture,RoleType type){
	CEntity *p_Entity = new CEntity;
	if(p_Entity && p_Entity->initWithTexture(pTexture)){
		p_Entity->autorelease();
		p_Entity->SetRoleType(type);
		return p_Entity;
	}else{
		delete p_Entity;
		return NULL;
	}


}

CEntity *CEntity::create(RoleType type){
	CEntity *p_Entity = new CEntity;
	if(p_Entity && p_Entity->init()){
		p_Entity->autorelease();
		p_Entity->SetDirection(0);
		p_Entity->SetRoleType(type);
		p_Entity->scheduleUpdate();
		
		return p_Entity;
	}else{
		delete p_Entity;
		return NULL;
	}
}

#include "DBWindow.h"
CCAction* CEntity::Run(int i)
{
	if(this->m_rType >= RT_MAX ) return NULL;
	if (m_Atype == AT_Run && i == m_Direction)
	{
		return NULL;
	}
	m_Direction = i;
	DBWindowWrite(&g_console,TEXT("direction:%d\n"),m_Direction);
	stopAllActions();
	m_Atype = AT_Run;

	CCAnimation* animation = CCAnimation::createWithSpriteFrames(ResourcesReader::GetInstance()->GetRoleAniByType(m_rType).GetIdleAni(AT_Run,i), (float)0.03);
	return runAction(CCRepeatForever::create(CCAnimate::create(animation)));
}

CCAction* CEntity::Idle(bool force)
{
	if(this->m_rType >= RT_MAX ) return NULL;
	if (m_Atype == AT_Idle || (!force && m_Atype == AT_Atk) || m_Atype == AT_Dead)
	{
		return NULL;
	}
	stopAllActions();
	m_Atype = AT_Idle;
	CCAnimation* animation = CCAnimation::createWithSpriteFrames(ResourcesReader::GetInstance()->GetRoleAniByType(m_rType).GetIdleAni(AT_Idle,m_Direction), (float)1);
	return runAction(CCRepeatForever::create(CCAnimate::create(animation)));
	
}

CCAction* CEntity::Atk(int i)
{
	if(this->m_rType >= RT_MAX) return NULL;	
	m_Direction = i;
	stopAllActions();
	m_Atype = AT_Atk;
	CCAnimation* animation = CCAnimation::createWithSpriteFrames(ResourcesReader::GetInstance()->GetRoleAniByType(m_rType).GetIdleAni(AT_Atk,i), (float)0.03);
	return runAction(CCRepeat::create(CCAnimate::create(animation),1));	
}

cocos2d::CCAction* CEntity::Hurt()
{
	if(this->m_rType >= RT_MAX) return NULL;
	if (m_Atype == AT_Hurt )
	{
		return NULL;
	}
	stopAllActions();
	m_Atype = AT_Hurt;
	CCAnimation* animation = CCAnimation::createWithSpriteFrames(ResourcesReader::GetInstance()->GetRoleAniByType(m_rType).GetIdleAni(AT_Idle,m_Direction), (float)0.03);
	return runAction(CCRepeat::create(CCAnimate::create(animation),1));
}

cocos2d::CCAction* CEntity::Dead()
{
	if(this->m_rType >= RT_MAX) return NULL;
	stopAllActions();
	m_Atype = AT_Dead;
	CCAnimation* animation = CCAnimation::createWithSpriteFrames(ResourcesReader::GetInstance()->GetRoleAniByType(m_rType).GetIdleAni(AT_Dead,m_Direction), (float)0.03);
	return runAction(CCRepeat::create(CCAnimate::create(animation),1));
}

void CEntity::update()
{

}