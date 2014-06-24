/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _ASTAR_H
#define _ASTAR_H
#include <stdint.h>
#include <stdlib.h>
#include <map>
#include "dlist.h"
#include "llist.h"
#include "minheap.h"
#include <list>
#include <vector>

extern int direction[8][2];

class AStar{
public:
	struct mapnode : public heapele,public dnode
	{
		mapnode *parent;
		double G;//从初始点到当前点的开销
		double H;//从当前点到目标点的估计开销
		double F;
		int    x;
		int    y;
		int    value;
	};

private:

	static bool _less(heapele*l,heapele*r)
	{
		return ((mapnode*)l)->F < ((mapnode*)r)->F;
	}

	static void _clear(heapele*e){
		((mapnode*)e)->F = ((mapnode*)e)->G = ((mapnode*)e)->H = 0;
	}

	mapnode *get_mapnode(int x,int y)
	{
		if(x < 0 || x >= m_xcount || y < 0 || y >= m_ycount)
			return NULL;
		return &m_map[y*m_xcount+x];
	}

	//获得当前maze_node的8个临近节点,如果是阻挡点会被忽略
	std::vector<mapnode*>* get_neighbors(mapnode *mnode)
	{
		m_neighbors.clear();
		int32_t i = 0;
		int32_t c = 0;
		for( ; i < 8; ++i)
		{
			int x = mnode->x + direction[i][0];
			int y = mnode->y + direction[i][1];
			mapnode *tmp = get_mapnode(x,y);
			if(tmp){
				if(tmp->value != 0xFFFFFFFF)
					m_neighbors.push_back(tmp);
				else
					printf("here\n");
			}
		}
		if(m_neighbors.empty()) return NULL;
		else return &m_neighbors;
	}

	//计算到达相临节点需要的代价
	double cost_2_neighbor(mapnode *from,mapnode *to)
	{
		int delta_x = from->x - to->x;
		int delta_y = from->y - to->y;
		int i = 0;
		for( ; i < 8; ++i)
		{
			if(direction[i][0] == delta_x && direction[i][1] == delta_y)
				break;
		}
		if(i < 4)
			return 50.0f;
		else if(i < 8)
			return 60.0f;
		else
		{
			assert(0);
			return 0.0f;
		}	
	}

	double cost_2_goal(mapnode *from,mapnode *to)
	{
		int delta_x = abs(from->x - to->x);
		int delta_y = abs(from->y - to->y);
		return (delta_x * 50.0f) + (delta_y * 50.0f);
	}

	void   reset();

	std::vector<mapnode*> m_neighbors;//中间计算用

	mapnode *m_map;
	int      m_xcount;
	int      m_ycount;
	minheap  open_list;
	dlist    close_list;
public:
	AStar():open_list(8192,_less,_clear){}

	bool Init(int x,int y,std::map<std::pair<int,int>,int> &values);

	~AStar(){
		if(m_map) free(m_map);
	}

	bool find_path(int x,int y,int x1,int y1,std::list<mapnode*> &path);

};

#endif

/*
//一个地图块节点
struct map_node{};

//路径节点
struct path_node:public heapele,public dnode 
{	
	struct path_node *parent;
	struct map_node  *_map_node;
	double G;//从初始点到当前点的开销
	double H;//从当前点到目标点的估计开销
	double F;
};

//由使用者提供的3个函数
//get_neighbors约定:如果一个map_node*是阻挡点,将不会被返回
typedef map_node** (*get_neighbors)(map_node*);
typedef double (*cost_2_neighbor)(path_node*,path_node*);
typedef double (*cost_2_goal)(path_node*,path_node*);

//一次路径搜索的过程对象
struct A_star_procedure
{
private:
	get_neighbors _get_neighbors;
	cost_2_neighbor _cost_2_neighbor;   //用于计算两个路径点G值的函数指针
	cost_2_goal _cost_2_goal;           //用于计算两个路径点H值的函数指针
	minheap open_list;
	dlist close_list;
	std::map<map_node*,path_node*> mnode_2_pnode;//map_node到path_node的映射

	static bool _less(heapele*l,heapele*r)
	{
		struct path_node *_l = (struct path_node*)l;
		struct path_node *_r = (struct path_node*)r;  
		return _l->F < _r->F;
	}

	//获得与mapnode关联的pathnode
	path_node *get_pnode_from_mnode(map_node *mnode)
	{
		path_node *pnode = NULL;
		
		std::map<map_node*,path_node*>::iterator it = mnode_2_pnode.find(mnode);
		if(it != mnode_2_pnode.end())
			pnode = it->second;
		else{
			pnode = (path_node*)calloc(1,sizeof(*pnode));
			pnode->_map_node = mnode;
			mnode_2_pnode[mnode] = pnode;
		}
		return pnode;
	}

	path_node *remove_min_pnode()
	{
		heapele *hele = open_list.popmin();
		return (path_node*)hele;
	}

	void insert_2_open(path_node *pnode)
	{
		open_list.insert(pnode);
	}

	void insert_2_close(path_node *pnode)
	{
		close_list.Push(pnode);
	}

	void change_open(path_node *pnode)
	{
		open_list.change(pnode);
	}

	void reset()
	{
		while(close_list.Pop()!=NULL);
		open_list.clear();
	}

public:
	A_star_procedure(get_neighbors gn,cost_2_neighbor cn,cost_2_goal cg):_get_neighbors(gn),
		_cost_2_neighbor(cn),_cost_2_goal(cg),open_list(8192,&A_star_procedure::_less){}

	path_node* find_path(struct map_node *from,struct map_node *to);

};
*/