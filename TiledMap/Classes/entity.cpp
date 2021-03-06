#include "entity.h"
#include "SimpleAudioEngine.h"
#include "SceneMain.h"
#include "DBWindow.h"
USING_NS_CC;
using namespace cocos2d::extension;
extern SceneMain *g_scenemain;

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

static unsigned char Direction(tiled *old_t,tiled *new_t,unsigned char olddir){
	
	unsigned char dir = olddir;	
	if(new_t->r == old_t->r){
		if(new_t->c > old_t->c)
			dir = east;
		else if(new_t->c < old_t->c)
			dir = west;
	}else if(new_t->r > old_t->r){
		if(new_t->c > old_t->c)
			dir = south_east;
		else if(new_t->c < old_t->c)
			dir = south_west;
		else 
			dir = south;
	}else{
		if(new_t->c > old_t->c)
			dir = north_east;
		else if(new_t->c < old_t->c)
			dir = north_west;
		else 
			dir = north;	
	
	}
	return dir;
}

void CEntity::update()
{
	if(!m_curtiled) return;
	CCPoint point = worldPoint;
	if(!m_nexttiled && !m_path.empty()){
		AStar::mapnode *node = m_path.front();
		m_path.pop_front();
		m_nexttiled = g_scenemain->m_pTiledGrid[node->y*g_scenemain->m_totalNumX+node->x];
	}

	if(!m_nexttiled){ 
		Idle();
		setPosition(g_scenemain->World2Screen(worldPoint));
		return;
	}
	
	targetPoint = m_nexttiled->point;
	bool doMov = false;
	do{
		if(!point.equals(targetPoint)){
			CCPoint oriPoint = point;
			float dis = point.getDistance(targetPoint);
			float speed = 3.0f;
			float delta_x;
			float delta_y;
			if(dis > speed){				
				delta_x = targetPoint.x - point.x;
				delta_y = targetPoint.y - point.y;
				if(abs(delta_x) < 0.0000001f) delta_x = 0;
				else if(delta_x > 0) delta_x = speed;
				else delta_x = -speed;

				if(abs(delta_y) < 0.0000001f) delta_y = 0;
				else if(delta_y > 0) delta_y = speed/2;
				else delta_y = -speed/2;

				if(abs(delta_x) < 0.0000001f) delta_y*=2;

				point.x += delta_x;
				point.y += delta_y;			
			}else{
				delta_x = targetPoint.x - point.x;
				delta_y = targetPoint.y - point.y;
				point = targetPoint;
			}
			worldPoint = point;
			if(this == g_scenemain->m_maincha){	
				g_scenemain->m_map->setPositionX(g_scenemain->m_map->getPositionX()-delta_x);
				g_scenemain->m_map->setPositionY(g_scenemain->m_map->getPositionY()-delta_y);
			}else{
				setPosition(g_scenemain->World2Screen(point));
			}
			Run(Direction(m_curtiled,m_nexttiled,GetDirection()));
			doMov = true;
		}
		else
		{
			m_curtiled = m_nexttiled;
			if(m_path.empty()){
				DBWindowWrite(&g_console,TEXT("arrive:(%d,%d)\n"),m_curtiled->c,m_curtiled->r);
				m_nexttiled = NULL;
				Idle();
				return;
			}
			else{
				AStar::mapnode *node = m_path.front();
				m_path.pop_front();
				m_nexttiled = g_scenemain->m_pTiledGrid[node->y*g_scenemain->m_totalNumX+node->x];
				targetPoint = m_nexttiled->point;
			}

		}
	}while(!doMov);
}