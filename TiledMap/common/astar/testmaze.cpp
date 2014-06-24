/*A*迷宫测试用例*/
#include "astar.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
struct maze_map;
struct maze_node : public map_node
{
	maze_map *_map;
	char value;
	/*在地图中的x,y坐标*/
	int x; 
	int y;	
};

#define BLOCK '#'//阻挡

int direction[8][2] = {
	{0,-1},//上
	{0,1},//下
	{-1,0},//左
	{1,0},//右
	{-1,-1},//左上
	{1,-1},//右上
	{-1,1},//左下
	{1,1},//右下
};

struct maze_map
{
	maze_node **_maze_map;
	int    max_x;
	int    max_y;

	maze_node *get_mazenode_by_xy(int x,int y)
	{
		if(x < 0 || x >= max_x || y < 0 || y >= max_y)
			return NULL;
		return _maze_map[y*max_x+x];
	}

	maze_map(char *array,int max_x,int max_y):max_x(max_x),max_y(max_y)
	{
		_maze_map = (maze_node**)calloc(max_x*max_y,sizeof(maze_node*));
		int i = 0;
		int j = 0;
		for( ; i < max_y; ++i)
		{
			for(j = 0; j < max_x;++j)
			{		
				_maze_map[i*max_x+j] = (maze_node*)calloc(1,sizeof(maze_node));
				maze_node *tmp = _maze_map[i*max_x+j];
				tmp->_map = this;
				tmp->x = j;
				tmp->y = i;
				tmp->value = array[i*max_x+j];
				if(tmp->value == 2) tmp->value = '#';
			}
		}
	}

	~maze_map(){
		free(_maze_map);
	}
};



//获得当前maze_node的8个临近节点,如果是阻挡点会被忽略
map_node** maze_get_neighbors(map_node *mnode)
{
	map_node **ret = (map_node **)calloc(9,sizeof(*ret));
	maze_node *_maze_node = (maze_node*)mnode;
	maze_map *_maze_map = _maze_node->_map;
	int32_t i = 0;
	int32_t c = 0;
	for( ; i < 8; ++i)
	{
		int x = _maze_node->x + direction[i][0];
		int y = _maze_node->y + direction[i][1];
		struct maze_node *tmp = _maze_map->get_mazenode_by_xy(x,y);
		if(tmp && tmp->value != BLOCK)
			ret[c++] = (map_node*)tmp;
	}
	ret[c] = NULL;
	return ret;
}

//计算到达相临节点需要的代价
double maze_cost_2_neighbor(path_node *from,path_node *to)
{
	maze_node *_from = (maze_node*)from->_map_node;
	maze_node *_to = (maze_node*)to->_map_node;
	int delta_x = _from->x - _to->x;
	int delta_y = _from->y - _to->y;
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

double maze_cost_2_goal(path_node *from,path_node *to)
{
	maze_node *_from = (maze_node*)from->_map_node;
	maze_node *_to = (maze_node*)to->_map_node;
	int delta_x = abs(_from->x - _to->x);
	int delta_y = abs(_from->y - _to->y);
	return (delta_x * 50.0f) + (delta_y * 50.0f);
}


void show_maze(maze_map *map,A_star_procedure *astar,int fx,int fy,int tx,int ty)
{
	map_node *from = (map_node*)map->get_mazenode_by_xy(fx,fy);
	map_node *to = (map_node*)map->get_mazenode_by_xy(tx,ty);
	path_node *path = astar->find_path(from,to);
	printf("\n");
	if(!path)
		printf("no way\n");
	while(path)
	{
		maze_node *mnode = (maze_node*)path->_map_node;
		mnode->value = '.';
		path = path->parent;
	}	
	//重新打印地图和路径
	int i = 0;
	int j = 0;
	for(; i < map->max_y; ++i)
	{
		for(j = 0; j < map->max_x; ++j)
		{
			maze_node *tmp = map->get_mazenode_by_xy(j,i);
			if(tmp->value != 0)
			{
				if(i == fy && j == fx)
					printf("B");
				else if(i == ty && j == tx)
					printf("E");
				else
					printf("%c",tmp->value);
				if(tmp->value != '#')
					tmp->value = 0;
			}
			else
				printf(" ");
		}
		printf("\n");
	}
}

int main()
{
	char array[15][25] =
	{
		{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
		{2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{2,0,0,2,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,2},
		{2,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2},
		{2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2},
		{2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2},
		{2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2},
		{2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2},
		{2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2},
		{2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2},
		{2,0,0,2,2,2,2,0,0,2,2,2,2,0,0,0,0,0,0,2,2,2,2,0,2},
		{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	};
	maze_map *map = new maze_map((char*)array,25,15);
	A_star_procedure *astar = new A_star_procedure(maze_get_neighbors,maze_cost_2_neighbor,maze_cost_2_goal);
	show_maze(map,astar,1,1,4,10);
	show_maze(map,astar,1,1,11,10);
	show_maze(map,astar,1,1,4,1);
	getchar();
	return 0;
}
