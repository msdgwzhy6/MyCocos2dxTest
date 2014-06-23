#ifndef _SCENEMAIN_H
#define _SCENEMAIN_H

#include "cocos2d.h"
#include "cocos-ext.h"
#include "astar.h"
#include "ResourcesReader.h"
#include "entity.h"
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

	struct tiled{
		int c;
		int r;
		cocos2d::CCPoint point;
	};
	//���ڽ�������Ļ�ĵ�ӳ�䵽Ѱ·tiled
	std::list<tiled*> *m_screen2tiled; 
	tiled*  Screen2Tiled(const cocos2d::CCPoint&) const;

	std::list<AStar::mapnode*> FindPath(tiled *from,tiled *to);


	CEntity*            m_maincha;
	tiled*              m_curtiled;
	tiled*              m_nexttiled;
	AStar               m_astar;	
	tiled**             m_pTiledGrid;    //б45��tiled���ĵ������������ͼ�е�����	
	cocos2d::CCSize     m_worldsize;
	int m_totalNumX;
	int m_totalNumY;
};

#endif