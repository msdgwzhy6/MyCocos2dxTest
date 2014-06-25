#include "SceneMain.h"
#include "cocos2d.h"
#include "DBWindow.h"

USING_NS_CC;

void SceneMain::Addworld2tiled(tiled *t){
	//中
	int r = ((t->point).y)/16;
	int c = ((t->point).x)/16;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//上
	r = ((t->point).y + 8)/16;
	c = ((t->point).x)/16;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//下
	r = ((t->point).y - 8)/16;
	c = ((t->point).x)/16;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//左
	r = ((t->point).y )/16;
	c = ((t->point).x - 8)/16;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//右
	r = ((t->point).y )/16;
	c = ((t->point).x + 8)/16;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
}

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
	m_worldsize = CCSize(960,960);
	int nXnum = m_worldsize.width/16;
	int nYnum = m_worldsize.height/16;
	m_totalNumX = nXnum;
	m_totalNumY = nYnum;

	std::map<std::pair<int,int>,int> values;
	m_pTiledGrid = (tiled**)calloc((nXnum*nYnum),sizeof(*m_pTiledGrid));
	m_world2tiled = new std::list<tiled*>[nXnum*nYnum];
	float start_x = m_worldsize.width/2;
	float start_y = m_worldsize.height-8;
	m_map = CCTMXTiledMap::create("level1.tmx");
	CCTMXLayer *meta =  m_map->layerNamed("barrier");

	for (int y=0;y<nYnum;y++)
	{
		for (int x=0;x<nXnum;x++)
		{
			tiled *tmp = new tiled;
			tmp->point =  ccp(start_x+x*8,start_y-x*8);//90度视角中心点世界坐标
			tmp->r = y;
			tmp->c = x;
			m_pTiledGrid[nXnum*y+x] = tmp;
			do{
				int tiledGid = meta->tileGIDAt(ccp(x,y));
				if(tiledGid != 0) {
					CCDictionary* propertiesDict = m_map->propertiesForGID(tiledGid);
					const CCString* prop = propertiesDict->valueForKey("Collidable");
					if(prop->m_sString.compare("true") == 0){
						values.insert(std::make_pair(std::make_pair(x,y),0xFFFFFFFF));
						DBWindowWrite(&g_console,TEXT("cli:(%d,%d)\n"),x,y);
						break;
					}
				}
				values.insert(std::make_pair(std::make_pair(x,y),0));
			}
			while(0);
			Addworld2tiled(tmp);
			//int r = ((tmp->point).y)/16;
			//int c = ((tmp->point).x)/16;
			//m_world2tiled[nXnum*r+c].push_back(tmp);
		}
		start_y -= 8;
		start_x -= 8;

	}
	m_astar.Init(nXnum,nYnum,values);
	m_map->setPosition(ccp(-240,-320));
	this->addChild(m_map);
	this->setTouchEnabled(true);
	this->scheduleUpdate();
	//CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("bush.png");
	//m_tree = CCSprite::createWithTexture(texture);
	//m_tree->setScale(0.7f);
	//m_tree->setPosition(ccp(250,250));
	//this->addChild(m_tree);
	for (int y=0;y<nYnum;y++)
	{
		for (int x=0;x<nXnum;x++)
		{
			std::list<tiled*>::iterator it = m_world2tiled[y*m_totalNumX + x].begin();
			std::list<tiled*>::iterator end = m_world2tiled[y*m_totalNumX + x].end();
			for(; it != end; ++it){
				//CCPoint p(x*16+8,y*16+8);
			}
		}
	}
	return true;
}

void on_client_connect(){
	comm::begply();
}

void on_disconnected(){
	exit(0);
}

void on_client_connect_fail(){
	//连接失败,休眠一会重试
	//sleepms(1000);
	//comm::Connect(ip.c_str(),port,on_client_connect,on_client_connect_fail,on_disconnected);
}


void  SceneMain::onAsynLoadFinish(){
	if(!comm::Connect("127.0.0.1",8010,on_client_connect,on_client_connect_fail,on_disconnected)){
	}
}





void SceneMain::update(float tick){
	comm::Run();
	if(!m_maincha) return;
	m_maincha->update();
	for(std::map<unsigned long long,CEntity*>::iterator it = m_otherply.begin();
		it != m_otherply.end(); ++it)
		it->second->update();
}

SceneMain *g_scenemain;

CCScene* SceneMain::scene()
{
	CCScene* pScene = CCScene::create();
	SceneMain *pSM = SceneMain::create();
	pScene->addChild(pSM);
	pSM->m_maincha = NULL;
	g_scenemain = pSM;
	return pScene;
}


tiled* SceneMain::World2Tiled(const cocos2d::CCPoint &worldpos) const{
	int c = worldpos.x/16.0;
	int r = worldpos.y/16.0;
	if(c >= m_worldsize.width/16.0 || r >= m_worldsize.height/16.0) return NULL;	
	std::list<tiled*>::iterator it = m_world2tiled[r*m_totalNumX + c].begin();
	std::list<tiled*>::iterator end = m_world2tiled[r*m_totalNumX + c].end();
	tiled *bestfit = NULL;
	float  mindis = 999.0f;
	for(; it != end; ++it){
		float dis = (*it)->point.getDistance(worldpos);
		if(!bestfit || dis < mindis){
			bestfit = (*it);
			mindis = dis;
		}
	}
	return bestfit;
}

#include "DBWindow.h"
std::list<AStar::mapnode*> SceneMain::FindPath(tiled *from,tiled *to){
	std::list<AStar::mapnode*> path;
	m_astar.find_path(from->c,from->r,to->c,to->r,path);
	if(!path.empty()) path.pop_front();
	//DBWindowWrite(&g_console,TEXT("from:(%d,%d) to:(%d,%d) path:%d\n"),from->c,from->r,to->c,to->r,path.size());
	
	for(std::list<AStar::mapnode*>::iterator it = path.begin();
		it != path.end(); ++it)
		DBWindowWrite(&g_console,TEXT("(%d,%d)\n"),(*it)->x,(*it)->y);
	DBWindowWrite(&g_console,TEXT("path size:%d\n"),path.size());
	return path;
}

void SceneMain::ccTouchesBegan( cocos2d::CCSet *pTouche, cocos2d::CCEvent *pEvent){
	if(!m_maincha)return;
	CCTouch* touch = (CCTouch*)pTouche->anyObject();
	CCPoint pos = CCDirector::sharedDirector()->convertToUI(touch->getLocationInView());
	tiled* t = World2Tiled(Screen2World(pos));
	DBWindowWrite(&g_console,TEXT("cur(%d,%d),tar(%d,%d)\n"),m_maincha->m_curtiled->c,m_maincha->m_curtiled->r,t->c,t->r);
	if(!t) return;

	m_maincha->m_path = FindPath(m_maincha->m_curtiled,t);
	if(!m_maincha->m_path.empty()){
		AStar::mapnode *t = m_maincha->m_path.back();
		comm::move(t->x,t->y);
	}

}

void SceneMain::CMD_ENTERSEE(comm::stEnterSee *st){
	if(st->id == g_scenemain->m_maincha->identity){
		CCPoint pos = g_scenemain->m_pTiledGrid[st->y*g_scenemain->m_totalNumX+st->x]->point;
		g_scenemain->m_maincha->worldPoint = pos;
		g_scenemain->m_maincha->targetPoint = pos;
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();			
		g_scenemain->m_maincha->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2));

		CCPoint tmp = g_scenemain->Screen2World(g_scenemain->m_maincha->getPosition());
		double delta_x = pos.x - tmp.x;
		double delta_y = pos.y - tmp.y;
		g_scenemain->m_map->setPositionX(g_scenemain->m_map->getPositionX()-delta_x);
		g_scenemain->m_map->setPositionY(g_scenemain->m_map->getPositionY()-delta_y);
		
		g_scenemain->addChild(g_scenemain->m_maincha);
		g_scenemain->m_maincha->Run(st->dir);
		g_scenemain->m_maincha->m_curtiled = g_scenemain->m_pTiledGrid[st->y*g_scenemain->m_totalNumX+st->x];			
	}else{
		CEntity *e = CEntity::create(RT_CAT);
		e->setScale(0.7f);
		e->identity = st->id;
		CCPoint pos = g_scenemain->m_pTiledGrid[st->y*g_scenemain->m_totalNumX+st->x]->point;
		e->setPosition(pos);
		e->targetPoint = pos;
		e->worldPoint = pos;
		e->setPosition(g_scenemain->World2Screen(pos));
		g_scenemain->addChild(e);
		e->Run(st->dir);
		e->m_curtiled = g_scenemain->m_pTiledGrid[st->y*g_scenemain->m_totalNumX+st->x];
		g_scenemain->m_otherply.insert(std::make_pair(st->id,e));

		DBWindowWrite(&g_console,TEXT("other enter:(%d,%d)\n"),e->m_curtiled->c,e->m_curtiled->r);
	}
}

void SceneMain::CMD_ENDSEE(comm::stLevSee *st){
	CEntity *e = g_scenemain->GetOtherPly(st->id);
	if(e){
		g_scenemain->m_otherply.erase(st->id);
		g_scenemain->removeChild(e);
	}
}

void SceneMain::CMD_MOV(comm::stMov *st){
	if(st->id != g_scenemain->m_maincha->identity){
		CEntity *e = g_scenemain->GetOtherPly(st->id);
		if(e){
			tiled *t = g_scenemain->m_pTiledGrid[st->y*g_scenemain->m_totalNumX+st->x];
			e->m_path = g_scenemain->FindPath(e->m_curtiled,t);
		}
	}
}

void SceneMain::CMD_BEGPLY(comm::stBegPly *st){
	CEntity *e = CEntity::create(RT_CAT);
	e->setScale(0.7f);
	e->identity = st->id;
	g_scenemain->m_maincha = e;	
}

cocos2d::CCPoint SceneMain::World2Screen(const cocos2d::CCPoint &pos) const{
	CCPoint map_screen_pos = m_map->getPosition();
	CCPoint screenZeroOnWorld = ccp(-map_screen_pos.x,-map_screen_pos.y);
	return ccp(pos.x - screenZeroOnWorld.x,pos.y - screenZeroOnWorld.y);
}

cocos2d::CCPoint SceneMain::Screen2World(const cocos2d::CCPoint &pos) const{	
	CCPoint map_screen_pos = m_map->getPosition();
	CCPoint screenZeroOnWorld = ccp(-map_screen_pos.x,-map_screen_pos.y); 	
	return  ccp(pos.x + screenZeroOnWorld.x,pos.y + screenZeroOnWorld.y);
}