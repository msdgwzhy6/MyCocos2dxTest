/*8码问题测试用例*/
#include "astar.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "dlist.h"
#include <stdlib.h>
#include <string.h>
#include <map>

struct _8puzzle_map;

struct _8puzzle_node:public map_node,public lnode
{
	_8puzzle_map *_map;
	int puzzle[3][3];
	int zero_x;
	int zero_y;
};

int direction[4][2] = {
	{0,-1},//上
	{0,1},//下
	{-1,0},//左
	{1,0},//右
};

static inline uint64_t puzzle_value(int p[3][3])
{
	uint64_t pv = 0;
	static int factor[9] = {
		100000000,
		10000000,
		1000000,
		100000,
		10000,
		1000,
		100,
		10,
		1,		
	};
	int c = 0;
	int i = 0;
	int j = 0;
	for( ; i < 3; ++i)
		for(j = 0;j < 3;++j)
			pv += p[i][j]*factor[c++];
	return pv;
}

struct _8puzzle_map
{	
	llist mnodes;
	std::map<uint64_t,_8puzzle_node*> puzzle_2_mnode;

	struct _8puzzle_node* getnode_by_pv(int p[3][3],int zero_x,int zero_y)
	{
		uint64_t pv = puzzle_value(p);
		_8puzzle_node* pnode = NULL;
		
		std::map<uint64_t,_8puzzle_node*>::iterator it = puzzle_2_mnode.find(pv);
		if(it != puzzle_2_mnode.end())
			pnode = it->second;
		else{
			pnode = (_8puzzle_node*)calloc(1,sizeof(*pnode));
			memcpy(pnode->puzzle,p,sizeof(int)*9);
			pnode->_map = this;
			mnodes.push_back(pnode);
			puzzle_2_mnode[pv] = pnode;
			if(zero_x >= 0 && zero_y >= 0)
			{
				pnode->zero_x = zero_x;
				pnode->zero_y = zero_y;
				return pnode;
			}
			int i = 0;
			int j = 0;
			for( ; i < 3; ++i)
			{
				for(j = 0; j < 3; ++j)
					if(p[i][j] == 0)
					{
						pnode->zero_x = j;
						pnode->zero_y = i;
						return pnode;
					}
			}

		}
		return pnode;
	}
};



static inline void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

map_node** _8_get_neighbors(map_node *mnode)
{
	map_node **ret = (map_node **)calloc(5,sizeof(*ret));
	_8puzzle_node *__8puzzle_node = (_8puzzle_node*)mnode;
	_8puzzle_map *_puzzle_map = __8puzzle_node->_map;
	int p[3][3];
	int32_t i = 0;
	int32_t c = 0;
	for( ; i < 4; ++i)
	{
		int x = __8puzzle_node->zero_x + direction[i][0];
		int y = __8puzzle_node->zero_y + direction[i][1];
		if(x < 0 || x >=3 || y < 0 || y >= 3)
			continue;
		memcpy(p,__8puzzle_node->puzzle,sizeof(int)*9);
		swap(&p[y][x],&p[__8puzzle_node->zero_y][__8puzzle_node->zero_x]);		
		_8puzzle_node *tmp = _puzzle_map->getnode_by_pv(p,x,y);
		ret[c++] = (map_node*)tmp;
	}
	ret[c] = NULL;
	return ret;
}

double _8_cost_2_neighbor(path_node *from,path_node *to)
{
	return 1.0f;
} 

double _8_cost_2_goal(path_node *from,path_node *to)
{
	_8puzzle_node *_from = (_8puzzle_node*)from->_map_node;
	_8puzzle_node *_to = (_8puzzle_node*)to->_map_node;
	int *tmp_from = (int*)_from->puzzle;
	int *tmp_to = (int*)_to->puzzle;
	int i = 0;
	double sum = 0.0f;
	for( ; i < 9; ++i)
		if(tmp_from[i] != tmp_to[i])
			++sum;	
	return sum*4.66f;
}

int8_t check(int *a,int *b)
{
	uint32_t _a = 0;
	uint32_t _b = 0;
	int i = 0;
	int j = 0;
	for( ; i < 9; ++i)
	{
		for( j = i+1;j<9;++j)
			if((a[i]!=0 && a[j]) != 0 && (a[i] > a[j]))
				++_a;
	}
	for( i=0; i < 9; ++i)
	{
		for( j = i+1;j<9;++j)
			if((b[i]!=0 && b[j]) != 0 && (b[i] > b[j]))
				++_b;
	}
	return _a%2 == _b%2;
}


void show_8puzzle(_8puzzle_map *map,A_star_procedure *astar,int _from[3][3],int _to[3][3])
{
	if(!check(&_from[0][0],&_to[0][0]))
	{
		printf("no way\n");
	}	
	int path_count = 0;
	map_node *from = (map_node*)map->getnode_by_pv(_from,-1,-1);
	map_node *to = (map_node*)map->getnode_by_pv(_to,-1,-1);
	path_node *path = astar->find_path(from,to);
	if(!path)
		printf("no way\n");
	while(path)
	{
		_8puzzle_node *mnode = (_8puzzle_node *)path->_map_node;
		int i = 0;
		int j = 0;
		for( ; i < 3; ++i)
		{
			for(j = 0; j < 3; ++j)
			{
				if(mnode->puzzle[i][j] == 0)
					printf(" ");
				else
					printf("%d",mnode->puzzle[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		path = path->parent;
		++path_count;
	}
	printf("path_count:%d\n",path_count);
}

int main()
{
	int f0[3][3] = {
		{2,3,4},
		{1,8,5},
		{0,7,6},
	};
	int t0[3][3] = {
		{1,2,3},
		{8,0,4},
		{7,6,5},
	};
	int f1[3][3] = {
		{1,2,3},
		{4,5,6},
		{7,8,0},
	};
	int t1[3][3] = {
		{8,7,6},
		{5,4,3},
		{2,1,0},
	};			
	_8puzzle_map *map = new _8puzzle_map();
	A_star_procedure *astar = new A_star_procedure(_8_get_neighbors,_8_cost_2_neighbor,_8_cost_2_goal);
	show_8puzzle(map,astar,f0,t0);
	show_8puzzle(map,astar,f1,t1);
	getchar();
	return 0;
}



