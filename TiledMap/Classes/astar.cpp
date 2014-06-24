#include "astar.h"
#include <float.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "DBWindow.h"
int direction[8][2] = {
	{0,-1},//��
	{0,1},//��
	{-1,0},//��
	{1,0},//��
	{-1,-1},//����
	{1,-1},//����
	{-1,1},//����
	{1,1},//����
};	


bool AStar::Init(int x,int y,std::map<std::pair<int,int>,int> &values){
	
	if(x < 0 || y < 0 || x*y != values.size()) 
		return false;
	m_xcount = x;
	m_ycount = y;
	m_map = (mapnode*)calloc(m_xcount*m_ycount,sizeof(*m_map));
	int i = 0;
	int j = 0;
	int c = 0;
	DBWindowWrite(&g_console,TEXT("###################################\n"));
	for( ; i < m_ycount; ++i)
	{
		for(j = 0; j < m_xcount;++j)
		{		
			mapnode *tmp = &m_map[i*m_xcount+j];
			tmp->x = j;
			tmp->y = i;
			
			std::pair<int,int> idx = std::make_pair(tmp->x,tmp->y);
			tmp->value = values[idx];
			if(tmp->value == 0xFFFFFFFF){
				DBWindowWrite(&g_console,TEXT("cli:(%d,%d)\n"),tmp->x,tmp->y);
			}
		}
	}
	return true;
}

void AStar::reset(){
	//����close list
	mapnode *n = NULL;
	while(n = (mapnode*)close_list.Pop()){
		n->G = n->H = n->F = 0;
	}
	//����open list
	open_list.Clear();
}

bool AStar::find_path(int x,int y,int x1,int y1,std::list<mapnode*> &path)
{
	mapnode *from = get_mapnode(x,y);
	mapnode *to = get_mapnode(x1,y1);
	if(from == to){
		path.push_back(from);		
		return true;
	}
	open_list.insert(from);	
	mapnode *current_node = NULL;	
	while(1){	
		current_node = (mapnode*)open_list.popmin();
		if(!current_node){ 
			reset();
			return false;
		}
		if(current_node == to){ 
			
			while(current_node)
			{
				path.push_front(current_node);
				mapnode *t = current_node;
				current_node = current_node->parent;
				t->parent = NULL;
				t->F = t->G = t->H = 0;
				t->index = 0;
			}	
			reset();
			return true;
		}
		//current���뵽close��
		close_list.Push(current_node);
		//��ȡcurrent�����ڽڵ�
		std::vector<mapnode*> *neighbors =  get_neighbors(current_node);
		if(neighbors)
		{
			int size = neighbors->size();
			for(int i =0 ; i < size; i++)
			{
				mapnode *neighbor = (*neighbors)[i];
				if(neighbor->pre || neighbor->next){
					continue;//��close����,��������
				}

				if(neighbor->index)//��openlist��
				{
					double new_G = current_node->G + cost_2_neighbor(current_node,neighbor);
					if(new_G < neighbor->G)
					{
						//������ǰneighbor·������,����·��
						neighbor->G = new_G;
						neighbor->F = neighbor->G + neighbor->H;
						neighbor->parent = current_node;
						open_list.change(neighbor);
					}
					continue;
				}
				neighbor->parent = current_node;
				neighbor->G = current_node->G + cost_2_neighbor(current_node,neighbor);
				neighbor->H = cost_2_goal(neighbor,to);
				neighbor->F = neighbor->G + neighbor->H;
				open_list.insert(neighbor);						
			}
			neighbors = NULL;
		}	
	}
}

/*
//����path_node�Ƿ���open����
static inline bool is_in_openlist(path_node *pnode)
{
	return pnode->index > 0;
}

//����path_node�Ƿ���close����
static inline bool is_in_closelist(path_node *pnode)
{
	return pnode->pre && pnode->next;
}




path_node* A_star_procedure::find_path(map_node *from,map_node *to)
{
	reset();
	path_node *_from = get_pnode_from_mnode(from);
	if(from == to) return _from;//��ʼ����Ŀ�����ͬһ���ڵ�
	path_node *_to = get_pnode_from_mnode(to);
	insert_2_open(_from);
	path_node *current_node = NULL;	
	while(1){	
		current_node = remove_min_pnode();
		if(!current_node) return NULL;//��·�ɴ�
		if(current_node == _to) return current_node;
		//current���뵽close��
		insert_2_close(current_node);	
		//��ȡcurrent�����ڽڵ�
		struct map_node **neighbors = _get_neighbors(current_node->_map_node);
		if(neighbors)
		{
			uint32_t i = 0;
			for( ; ; i++)
			{
				if(NULL == neighbors[i])
					break;
				struct path_node *neighbor = get_pnode_from_mnode(neighbors[i]);
				if(is_in_closelist(neighbor))
					continue;//��close����,��������
				if(is_in_openlist(neighbor))
				{
					double new_G = current_node->G + _cost_2_neighbor(current_node,neighbor);
					if(new_G < neighbor->G)
					{
						//������ǰneighbor·������,����·��
						neighbor->G = new_G;
						neighbor->F = neighbor->G + neighbor->H;
						neighbor->parent = current_node;
						change_open(neighbor);
					}
					continue;
				}
				neighbor->parent = current_node;
				neighbor->G = current_node->G + _cost_2_neighbor(current_node,neighbor);
				neighbor->H = _cost_2_goal(neighbor,_to);
				neighbor->F = neighbor->G + neighbor->H;
				insert_2_open(neighbor);					
			}
			free(neighbors);
			neighbors = NULL;
		}	
	}
}
*/
/*void   destroy_Astar(struct A_star_procedure **_astar)
{
	struct A_star_procedure *astar = *_astar;
	hash_map_iter it = hash_map_begin(astar->mnode_2_pnode);
	hash_map_iter end = hash_map_end(astar->mnode_2_pnode);
	while(!IT_EQ(it,end))
	{
		struct path_node *tmp = IT_GET_VAL(struct path_node*,it);
		free(tmp);
		IT_NEXT(it);
	}
	minheap_destroy(&astar->open_list);
	hash_map_destroy(&astar->mnode_2_pnode);
	free(astar);
	*_astar = NULL;
}*/
