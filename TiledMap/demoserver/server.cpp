#include <stdio.h>
#include "../common/net.h"
#include "../common/systime.h"
#include <map>
#include <set>
#include "../common/netcmd.h"
#include "../common/entitydef.h"
#include "../common/astar/astar.h"

struct gamemap;
static net::Reactor *reactor = NULL;

struct avatar{
	int            x;
	int            y;
	unsigned char  dir;

	void pack(net::WPacket &wpk){
		wpk.WriteUint64((unsigned long long)(this));
		wpk.WriteUint8(dir);
		wpk.WriteUint32(x);
		wpk.WriteUint32(y);
	}

	unsigned long m_lasttick;
	unsigned long m_movmargin;
	virtual void tick(){}
};

struct Map;

struct point{
	int x;
	int y;
	point(int x,int y):x(x),y(y){}
};

static inline double distance(int x1,int y1,int x2,int y2){
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

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

static unsigned char Direction(point old_t,point new_t,unsigned char olddir){
	
	unsigned char dir = olddir;	
	if(new_t.y == old_t.y){
		if(new_t.x > old_t.x)
			dir = east;
		else if(new_t.x < old_t.x)
			dir = west;
	}else if(new_t.y > old_t.y){
		if(new_t.x > old_t.x)
			dir = south_east;
		else if(new_t.x < old_t.x)
			dir = south_west;
		else 
			dir = south;
	}else{
		if(new_t.x > old_t.x)
			dir = north_east;
		else if(new_t.x < old_t.x)
			dir = north_west;
		else 
			dir = north;	
	
	}
	return dir;
}

struct player : public avatar{
	net::Socket  *m_sock;
	Map*          m_map;
	std::list<AStar::mapnode*> m_path;
	void tick();
};

//游戏地图
struct Map{
	std::set<player*> m_players;  //地图上的玩家
	AStar             m_astar;
	int               m_totalNumX;
	int               m_totalNumY;


	std::list<AStar::mapnode*> FindPath(const point &from,const point &to){
		std::list<AStar::mapnode*> path;
		m_astar.find_path(from.x,from.y,to.x,to.y,path);
		if(!path.empty()) path.pop_front();
		return path;	
	}
	void init(int width,int hight){
		m_totalNumX = width;
		m_totalNumY = hight;
		std::map<std::pair<int,int>,int> values;

		values.insert(std::make_pair(std::make_pair(17,5),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(18,5),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(19,5),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(16,6),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(17,6),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(18,6),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(19,6),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(20,6),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,7),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(15,7),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(16,7),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(17,7),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(18,7),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(20,7),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(21,7),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,8),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(14,8),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(15,8),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(16,8),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(21,8),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(22,8),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,9),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,9),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(14,9),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(15,9),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(22,9),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(23,9),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,10),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,10),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(14,10),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(23,10),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(24,10),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,11),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,11),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(14,11),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(24,11),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(25,11),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,12),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,12),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(14,12),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(25,12),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,13),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,13),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(14,13),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(25,13),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(26,13),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,14),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,14),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(26,14),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,15),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,15),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(26,15),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(27,15),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(5,16),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,16),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,16),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(27,16),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(5,17),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,17),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(27,17),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(5,18),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,18),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(27,18),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(5,19),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,19),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(27,19),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(6,20),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(7,20),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(26,20),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(27,20),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(7,21),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(8,21),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(9,21),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(25,21),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(26,21),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(9,22),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(10,22),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(11,22),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(23,22),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(24,22),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(25,22),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(11,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(12,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(13,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(14,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(15,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(16,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(17,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(18,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(19,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(20,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(21,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(22,23),0xFFFFFFFF));
		values.insert(std::make_pair(std::make_pair(23,23),0xFFFFFFFF));
		for (int y=0;y<m_totalNumY;y++)
		{
			for (int x=0;x<m_totalNumX;x++)
			{
				values.insert(std::make_pair(std::make_pair(x,y),0));
			}

		}
		m_astar.Init(m_totalNumX,m_totalNumY,values);
	}

	void enter(player *ply){
		net::WPacket wpk1;
		wpk1.WriteUint16(CMD_SC_ENTERSEE);
		ply->pack(wpk1);
		ply->m_sock->Send(wpk1);
		{
			std::set<player*>::iterator it = m_players.begin();
			for(; it != m_players.end(); ++it){
				//将自己通告给所有人,包括自己
				(*it)->m_sock->Send(wpk1);
				//将别人通告给自己
				net::WPacket wpk2;
				wpk2.WriteUint16(CMD_SC_ENTERSEE);
				(*it)->pack(wpk2);
				ply->m_sock->Send(wpk2);
				//如果别人有移动请求，将移动请求也通告给自己
				if(!ply->m_path.empty()){
					net::WPacket wpk3;
					wpk3.WriteUint16(CMD_SC_MOV);
				}
			}
		}
		m_players.insert(ply);
		ply->m_map = this;

	}

	void leave(player *ply){
		ply->m_map = NULL;
		m_players.erase(ply);
		net::WPacket wpk;
		wpk.WriteUint16(CMD_SC_LEVSEE);
		wpk.WriteUint64((unsigned long long)ply);
		boardcast(wpk);
	}

	void tick(){
		std::set<player*>::iterator it = m_players.begin();
		int i = 0;
		for(; it !=  m_players.end(); ++it,++i){
			(*it)->tick();
		}
	}

	void boardcast(net::WPacket &wpk){
		std::set<player*>::iterator it = m_players.begin();
		int i = 0;
		for(; it !=  m_players.end(); ++it,++i){
			(*it)->m_sock->Send(wpk);
		}
	}
};

void player::tick(){
	unsigned long now = GetSystemMs();
	if(!m_path.empty()){
		m_movmargin += now - m_lasttick;
		//计算移动一格需要的时间
		AStar::mapnode *node = m_path.front();
		while(node){
			double dis = distance(x,y,node->x,node->y)*16.0f;
			double speed = 4.0f * (1000/30) / 1000;
			unsigned long elapse =  (unsigned long)(dis/speed);
			if(elapse <  m_movmargin){
				dir = Direction(point(x,y),point(node->x,node->y),dir);
				x = node->x;
				y = node->y;
				m_path.pop_front();
				if(m_path.empty()){
					printf("arrive:(%d,%d)\n",x,y);
					break;
				}
				node = m_path.front();
				m_movmargin -= elapse;		
			}else
				break;
		}
	}
	m_lasttick = now;
}

static std::map<net::Socket*,player*> g_plys;

Map g_map;

static player* GetPly(net::Socket *s){
	std::map<net::Socket*,player*>::iterator it = g_plys.find(s);
	if(it == g_plys.end()) return NULL;
	return it->second;
}

void process_packet(net::Socket *s,net::RPacket &rpk){
	unsigned short cmd = rpk.ReadUint16();
	switch(cmd){
	case CMD_CS_BEGPLY:{
			unsigned char mapid = rpk.ReadUint8();
			player *ply = new player;
			ply->m_sock = s;
			ply->dir = rand()%8;
			ply->x = 14;
			ply->y = 14;
			ply->m_lasttick = GetSystemMs();
			ply->m_movmargin = 0;
			g_plys.insert(std::make_pair(s,ply));
			net::WPacket wpk;
			wpk.WriteUint16(CMD_SC_BEGPLY);
			wpk.WriteUint64((unsigned long long)ply);
			s->Send(wpk);
			g_map.enter(ply);
			return;
		}
	case CMD_CS_MOV:{
			int x = rpk.ReadUint32();
			int y = rpk.ReadUint32();
			printf("CMD_CS_MOV (%d,%d)\n",x,y);
			player *ply = GetPly(s);
			if(ply && ply->m_map){
				ply->m_path = ply->m_map->FindPath(point(ply->x,ply->y),point(x,y));
				if(!ply->m_path.empty()){
					ply->m_movmargin = 0;
					net::WPacket wpk;
					wpk.WriteUint16(CMD_SC_MOV);
					wpk.WriteUint64((unsigned long long)ply);
					wpk.WriteUint32(x);
					wpk.WriteUint32(y);
					ply->m_map->boardcast(wpk);
					for(std::list<AStar::mapnode*>::iterator it = ply->m_path.begin();
						it != ply->m_path.end(); ++it)
						printf("(%d,%d)\n",(*it)->x,(*it)->y);
					printf("path size:%d\n",ply->m_path.size());
				}
			}
			return;
		}
	}
}


void client_disconnected(net::Socket *s){
	player *ply = GetPly(s);
	if(!ply) return;
	if(ply->m_map) ply->m_map->leave(ply);
	g_plys.erase(s);
	delete ply;
}

void new_client(net::Socket *s){
	printf("new_client\n");
	s->Bind(reactor,process_packet,client_disconnected);
}



int main(int argc,char **argv){
	if(argc < 3) return 0;
	net::init();
	g_map.init(30,30);
	reactor = new net::Reactor;
	net::Socket *l = new net::Socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	l->Listen(reactor,argv[1],atol(argv[2]),new_client);
	unsigned long tick= ::GetSystemMs();
	while(1){
		reactor->LoopOnce(25);
		g_map.tick();
		unsigned long now = ::GetSystemMs();
		if(now - tick >= 1000){
			tick = now;
		}
	}
	net::clean();
}
