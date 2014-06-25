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

	//�����ӳ�䵽Ѱ·tiled
	std::list<tiled*> *m_world2tiled; 
	//��������ת������Ӧ��tiled
	tiled*  World2Tiled(const cocos2d::CCPoint&) const;
	//��Ļ����ת������������
	cocos2d::CCPoint Screen2World(const cocos2d::CCPoint&) const;
	//��������ת������Ļ����
	cocos2d::CCPoint World2Screen(const cocos2d::CCPoint&) const;

	void Addworld2tiled(tiled*t);

	std::list<AStar::mapnode*> FindPath(tiled *from,tiled *to);

	CCTMXTiledMap*      m_map;
	CEntity*            m_maincha;
	std::map<unsigned long long,CEntity*> m_otherply;
	cocos2d::CCSprite  *m_tree;
	AStar               m_astar;	
	tiled**             m_pTiledGrid;    //б45��tiled���ĵ������������ͼ�е�����	
	cocos2d::CCSize     m_worldsize;
	int m_totalNumX;
	int m_totalNumY;
};

#endif