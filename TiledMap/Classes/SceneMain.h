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

	CEntity *GetOtherPly(unsigned long long id){
		std::map<unsigned long long,CEntity*>::iterator it = m_otherply.find(id);
		if(it != m_otherply.end()) return it->second;
		return NULL;
	}

	//用于将触摸屏幕的点映射到寻路tiled
	std::list<tiled*> *m_screen2tiled; 
	tiled*  Screen2Tiled(const cocos2d::CCPoint&) const;

	std::list<AStar::mapnode*> FindPath(tiled *from,tiled *to);

	CCTMXTiledMap*      m_map;
	CEntity*            m_maincha;
	std::map<unsigned long long,CEntity*> m_otherply;
	cocos2d::CCSprite  *m_tree;
	AStar               m_astar;	
	tiled**             m_pTiledGrid;    //斜45度tiled中心点在正向世界地图中的坐标	
	cocos2d::CCSize     m_worldsize;
	int m_totalNumX;
	int m_totalNumY;
};

#endif