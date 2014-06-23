#include "SceneMain.h"
#include "cocos2d.h"

USING_NS_CC;


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
	m_worldsize = CCSize(480,480);
	int nXnum = m_worldsize.width/16;
	int nYnum = m_worldsize.height/16;
	m_totalNumX = nXnum;
	m_totalNumY = nYnum;

	std::vector<int> values;
	m_pTiledGrid = (tiled**)calloc((nXnum*nYnum),sizeof(*m_pTiledGrid));
	m_screen2tiled = new std::list<tiled*>[nXnum*nYnum];
	float start_x = m_worldsize.width/2;
	float start_y = m_worldsize.height-8;
	for (int y=0;y<nYnum;y++)
	{
		for (int x=0;x<nXnum;x++)
		{
			tiled *tmp = new tiled;
			tmp->point =  ccp(start_x+x*8,start_y-x*8);//90度视角中心点世界坐标
			tmp->r = y;
			tmp->c = x;
			m_pTiledGrid[nXnum*y+x] = tmp;
			values.push_back(0);
			int r = ((tmp->point).y)/16;
			int c = ((tmp->point).x)/16;
			m_screen2tiled[nXnum*r+c].push_back(tmp);
		}
		start_y -= 8;
		start_x -= 8;

	}
	m_astar.Init(nXnum,nYnum,values);
	CCTMXTiledMap *map = CCTMXTiledMap::create("level1.tmx");
	this->addChild(map);
	this->setTouchEnabled(true);
	this->scheduleUpdate();
	return true;
}

void  SceneMain::onAsynLoadFinish(){

	CEntity *e = CEntity::create(RT_CAT);
	e->setScale(0.7f);
	CCSize winsize = CCDirector::sharedDirector()->getWinSize();
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	e->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	e->targetPoint = ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y);
	this->addChild(e);
	e->Run(1);
	m_maincha = e;
	m_curtiled = Screen2Tiled(e->targetPoint);
}



static unsigned char Direction(SceneMain::tiled *old_t,SceneMain::tiled *new_t,unsigned char olddir){
	
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

void SceneMain::update(float tick){

	if(!m_maincha) return;

	CCPoint point = m_maincha->getPosition();
	if(!m_nexttiled && !m_maincha->m_path.empty()){
		AStar::mapnode *node = m_maincha->m_path.front();
		m_maincha->m_path.pop_front();
		m_nexttiled = m_pTiledGrid[node->y*m_totalNumX+node->x];
	}

	if(!m_nexttiled){ 
		m_maincha->Idle();
		return;
	}
	
	m_maincha->targetPoint = m_nexttiled->point;
	bool doMov = false;
	do{
		if(!point.equals(m_maincha->targetPoint)){

			CCPoint oriPoint = point;
			float delta_x = m_maincha->targetPoint.x - point.x;
			float delta_y = m_maincha->targetPoint.y - point.y;		
			float speed = 3.0f;
			if(abs(delta_x) > speed) delta_x = delta_x/abs(delta_x)*speed;
			if(abs(delta_y) > speed) delta_y = delta_y/abs(delta_y)*speed;
			point.x += delta_x;
			point.y += delta_y;
			m_maincha->setPosition(point);
			m_maincha->Run(Direction(m_curtiled,m_nexttiled,m_maincha->GetDirection()));
			doMov = true;
		}
		else
		{
			m_curtiled = m_nexttiled;
			if(m_maincha->m_path.empty()){
				m_nexttiled = NULL;
				m_maincha->Idle();
				return;
			}
			else{
				AStar::mapnode *node = m_maincha->m_path.front();
				m_maincha->m_path.pop_front();
				m_nexttiled = m_pTiledGrid[node->y*m_totalNumX+node->x];
				m_maincha->targetPoint = m_nexttiled->point;
			}

		}
	}while(!doMov);
}

SceneMain *g_scenemain;

CCScene* SceneMain::scene()
{
	CCScene* pScene = CCScene::create();
	SceneMain *pSM = SceneMain::create();
	pScene->addChild(pSM);
	pSM->m_maincha = NULL;
	pSM->m_curtiled = NULL;
	pSM->m_nexttiled = NULL;
	g_scenemain = pSM;
	return pScene;
}


SceneMain::tiled* SceneMain::Screen2Tiled(const cocos2d::CCPoint &pos) const{
	int c = pos.x/16.0;
	int r = pos.y/16.0;

	if(c >= m_worldsize.width/16.0 || r >= m_worldsize.height/16.0) return NULL;
	
	std::list<tiled*>::iterator it = m_screen2tiled[r*m_totalNumX + c].begin();
	std::list<tiled*>::iterator end = m_screen2tiled[r*m_totalNumX + c].end();
	for(; it != end; ++it){
		if((*it)->point.getDistance(pos) <= 16.0f)
			return (*it);
	}
	return NULL;
}

#include "DBWindow.h"
std::list<AStar::mapnode*> SceneMain::FindPath(tiled *from,tiled *to){
	std::list<AStar::mapnode*> path;
	m_astar.find_path(from->c,from->r,to->c,to->r,path);
	if(!path.empty()) path.pop_front();
	DBWindowWrite(&g_console,TEXT("from:(%d,%d) to:(%d,%d) path:%d\n"),from->c,from->r,to->c,to->r,path.size());
	return path;
}

void SceneMain::ccTouchesBegan( cocos2d::CCSet *pTouche, cocos2d::CCEvent *pEvent){
	if(!m_maincha)return;
	CCTouch* touch = (CCTouch*)pTouche->anyObject();
	CCPoint pos = CCDirector::sharedDirector()->convertToUI(touch->getLocationInView());
	tiled* t = Screen2Tiled(pos);
	if(!t) return;

	m_maincha->m_path = FindPath(m_curtiled,t);
}
