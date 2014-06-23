#ifndef _ENTITY_H
#define _ENTITY_H

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ResourcesReader.h"
#include "astar.h"
#include "dlist.h"
class SceneMap;

class CEntity;
enum{
	BF_SLOW = 0,
	BF_RECVORALL,
	BF_HIT,
	BF_RECVOR,
	BF_BUSH,
	BF_IDLE,
	BF_TOUCH,
};


class CEntity : public cocos2d::CCSprite,public dnode
{
public:
	static  CEntity *createWithTexture(cocos2d::CCTexture2D *pTexture,RoleType type);
	static  CEntity *create(RoleType type);
	cocos2d::CCAction* Run(int i);
	cocos2d::CCAction* Idle(bool force = false);
	cocos2d::CCAction* Atk(int i);
	cocos2d::CCAction* Hurt();
	cocos2d::CCAction* Dead();
	void update();

	void SetDirection(int nDirection){m_Direction = nDirection;}
	int  GetDirection(){return m_Direction;}
	void SetRoleType(RoleType type){m_rType = type;}
	RoleType GetRoleType(){return m_rType;}
	AniType GetAniType(){return m_Atype;}
	
	cocos2d::CCPoint   targetPoint;
	std::list<AStar::mapnode*> m_path;
	CEntity(){}
private:
	int m_Direction;
	RoleType m_rType;
	AniType  m_Atype;
};

#endif