#ifndef _SCENEMAIN_H
#define _SCENEMAIN_H

#include "cocos2d.h"
#include "cocos-ext.h"
#include "astar/astar.h"
#include "ResourcesReader.h"
#include "entity.h"
#include "net.h"
#include "systime.h"
#include "netcmd.h"
#include "netcs.h"
#include "netsc.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;



class SceneMain :  public CCLayer
{
public:
	virtual bool init();  
	static cocos2d::CCScene* scene();
	CREATE_FUNC(SceneMain);
	void update(float tick);
	void  onAsynLoadFinish();

	virtual void ccTouchesBegan(cocos2d::CCSet *pTouche, cocos2d::CCEvent *pEvent);

	static void CMD_ENTERSEE(comm::stEnterSee *st);

	static void CMD_ENDSEE(comm::stLevSee *st);

	static void CMD_MOV(comm::stMov *st);

	static void CMD_BEGPLY(comm::stBegPly *st);

	static void CMD_CREATE(comm::stCreate *st);

	static void CMD_ENTERMAP(comm::stEnterMap *st);


	CEntity *GetOtherPly(unsigned long id){
		std::map<unsigned long,CEntity*>::iterator it = m_otherply.find(id);
		if(it != m_otherply.end()) return it->second;
		return NULL;
	}

	//世界点映射到寻路tiled
	std::list<tiled*> *m_world2tiled; 
	//世界坐标转换到对应的tiled
	tiled*  World2Tiled(const cocos2d::CCPoint&) const;
	//屏幕坐标转换到世界坐标
	cocos2d::CCPoint Screen2World(const cocos2d::CCPoint&) const;
	//世界坐标转换到屏幕坐标
	cocos2d::CCPoint World2Screen(const cocos2d::CCPoint&) const;

	void Addworld2tiled(tiled*t);

	std::list<AStar::mapnode*> FindPath(tiled *from,tiled *to);

	CCTMXTiledMap*      m_map;
	CEntity*            m_maincha;
	std::map<unsigned long,CEntity*> m_otherply;
	cocos2d::CCSprite  *m_tree;
	AStar               m_astar;	
	tiled**             m_pTiledGrid;    //斜45度tiled中心点在正向世界地图中的坐标	
	cocos2d::CCSize     m_worldsize;
	int m_totalNumX;
	int m_totalNumY;
};

#endif