#include "SceneMain.h"
#include "cocos2d.h"
#include "DBWindow.h"

USING_NS_CC;

static const int tiled_width = 32;
static const int tiled_half_width = tiled_width/2;

static const int tiled_hight = 16;
static const int tiled_half_hight = tiled_hight/2;


std::string actname;
std::string nickname;

void SceneMain::Addworld2tiled(tiled *t){
	//中
	int r = ((t->point).y)/tiled_hight;
	int c = ((t->point).x)/tiled_width;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//上
	r = ((t->point).y + tiled_half_hight)/tiled_hight;
	c = ((t->point).x)/tiled_width;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//下
	r = ((t->point).y - tiled_half_hight)/tiled_hight;
	c = ((t->point).x)/tiled_width;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//左
	r = ((t->point).y )/tiled_hight;
	c = ((t->point).x - tiled_half_width)/tiled_width;
	if(r < m_totalNumY && c < m_totalNumX && r >= 0 && c >= 0)
		m_world2tiled[m_totalNumX*r+c].push_back(t);
	//右
	r = ((t->point).y )/tiled_hight;
	c = ((t->point).x + tiled_half_width)/tiled_width;
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
	m_worldsize = CCSize(960,480);
	int nXnum = m_worldsize.width/tiled_width;
	int nYnum = m_worldsize.height/tiled_hight;
	m_totalNumX = nXnum;
	m_totalNumY = nYnum;

	std::map<std::pair<int,int>,int> values;
	m_pTiledGrid = (tiled**)calloc((nXnum*nYnum),sizeof(*m_pTiledGrid));
	m_world2tiled = new std::list<tiled*>[nXnum*nYnum];
	float start_x = m_worldsize.width/2;
	float start_y = m_worldsize.height-tiled_half_hight;
	m_map = CCTMXTiledMap::create("level1.tmx");
	CCTMXLayer *meta =  m_map->layerNamed("barrier");

	//FILE *f = fopen("map1.coli","w");
	for (int y=0;y<nYnum;y++)
	{
		for (int x=0;x<nXnum;x++)
		{
			tiled *tmp = new tiled;
			tmp->point =  ccp(start_x+x*16,start_y-x*8);//90度视角中心点世界坐标
			tmp->r = y;
			tmp->c = x;
			m_pTiledGrid[nXnum*y+x] = tmp;
			do{
				int tiledGid = meta->tileGIDAt(ccp(x,y));
				if(tiledGid != 0) {
					CCDictionary* propertiesDict = m_map->propertiesForGID(tiledGid);
					if(propertiesDict){
						const CCString* prop = propertiesDict->valueForKey("Collidable");
						if(prop->m_sString.compare("true") == 0){
							values.insert(std::make_pair(std::make_pair(x,y),0xFFFFFFFF));
							DBWindowWrite(&g_console,TEXT("cli:(%d,%d)\n"),x,y);
							//fprintf(f,"%u\n",0xFFFFFFFF);
							break;
						}
					}
				}
				//fprintf(f,"%u\n",0);
				values.insert(std::make_pair(std::make_pair(x,y),0));
			}
			while(0);
			Addworld2tiled(tmp);
		}
		start_y -= tiled_half_hight;
		start_x -= tiled_half_width;

	}
	//fclose(f);
	m_astar.Init(nXnum,nYnum,values);
	m_map->setPosition(ccp(-240,-80));
	this->addChild(m_map);
	this->setTouchEnabled(true);
	this->scheduleUpdate();
	return true;
}

void on_client_connect(){
	comm::LOGIN(actname);
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
	if(!comm::Connect("192.168.220.128",8010,on_client_connect,on_client_connect_fail,on_disconnected)){
	}
}





void SceneMain::update(float tick){
	comm::Run();
	if(!m_maincha) return;
	m_maincha->update();
	for(std::map<unsigned long,CEntity*>::iterator it = m_otherply.begin();
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
	int c = worldpos.x/tiled_width;
	int r = worldpos.y/tiled_hight;
	if(c >= m_worldsize.width/tiled_width || r >= m_worldsize.height/tiled_hight || r < 0 || c < 0) 
		return NULL;	
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
	if(!t) return;

	comm::MOV(t->c,t->r);
	DBWindowWrite(&g_console,TEXT("cur(%d,%d),tar(%d,%d)\n"),m_maincha->m_curtiled->c,m_maincha->m_curtiled->r,t->c,t->r);
	//DBWindowWrite(&g_console,TEXT("cur(%d,%d),tar(%d,%d)\n"),m_maincha->m_curtiled->c,m_maincha->m_curtiled->r,t->c,t->r);
	//m_maincha->m_path = FindPath(m_maincha->m_curtiled,t);
	//if(!m_maincha->m_path.empty()){
		//AStar::mapnode *t = m_maincha->m_path.back();
	
	//comm::move(t->c,t->r);
	//}

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
	CEntity *e;
	if(st->id == g_scenemain->m_maincha->identity)
		e = g_scenemain->m_maincha;
	else e = g_scenemain->GetOtherPly(st->id);

	//if(st->id != g_scenemain->m_maincha->identity){
	//	CEntity *e = g_scenemain->GetOtherPly(st->id);
		if(e){
			tiled *t = g_scenemain->m_pTiledGrid[st->y*g_scenemain->m_totalNumX+st->x];
			e->m_path = g_scenemain->FindPath(e->m_curtiled,t);
		}
	//}
}

void SceneMain::CMD_BEGPLY(comm::stBegPly *st){
	comm::ENTERMAP();
}

void SceneMain::CMD_CREATE(comm::stCreate *st){
	comm::CREATE(nickname);	
}

void SceneMain::CMD_ENTERMAP(comm::stEnterMap *st){
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