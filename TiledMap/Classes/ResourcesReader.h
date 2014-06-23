#ifndef RESOUTCESREADER_H
#define RESOUTCESREADER_H

#include "cocos2d.h"

enum AniType
{
	AT_Idle,
	AT_Run,
	AT_Atk,
	AT_Hurt,
	AT_Dead,
	AT_Max
};

struct RoleAnimation
{
	//cocos2d::CCAnimate*  Ani[AT_Max][8];
	cocos2d::CCArray* Ani[AT_Max][8];
	RoleAnimation(){
		for (int i = 0;i<AT_Max;i++)
		{
			for (int j=0;j<8;j++)
			{
				Ani[i][j] = NULL;
			}
		}
	}
	cocos2d::CCArray* GetIdleAni(AniType type,int i){
		return Ani[type][i];
	}
};


enum RoleType
{
	RT_CAT,
	RT_RABBIT,
	RT_MAX,
	RT_TRAP = RT_MAX,
	RT_BERRY,
	RT_COCO,
	RT_BUSH,
	RT_BOMB,
};

enum{
	north,
	south,
	east,
	west,
	north_east,
	north_west,
	south_east,
	south_west 
};


class ResourcesReader : public cocos2d::CCObject
{
public:

	static ResourcesReader* GetInstance(){
		if(!m_pInst)
		{
			m_pInst = new ResourcesReader();
		}
		return m_pInst;
	}

	void InitRole();
	RoleAnimation GetRoleAniByType(RoleType type){return m_RoleAni[type];}
	bool IsRoleInit(){return bRoleInit;}
	void loadingRoleCallBack(cocos2d::CCObject *obj);

private:
	ResourcesReader();
	~ResourcesReader();
	ResourcesReader(const ResourcesReader&);
	ResourcesReader& operator = (const ResourcesReader&);
	static ResourcesReader  *m_pInst;
	bool bRoleInit;
	bool bSceneInit;
	RoleAnimation m_RoleAni[RT_MAX];
	unsigned int m_nFileNum;
};

#endif