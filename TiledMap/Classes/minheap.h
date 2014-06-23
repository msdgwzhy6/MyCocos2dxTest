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
#ifndef _MINHEAP_H
#define _MINHEAP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct heapele
{
	int index;//index in minheap;
};

struct minheap{
private:
	int size;
	int max_size;
	typedef bool (*less)(struct heapele*l,struct heapele*r);
	typedef void (*clear)(struct heapele*); 
	less cmp_function;
	clear clear_function;
	heapele **elements;

private:
	void swap(int idx1,int idx2)
	{
		heapele *ele = elements[idx1];
		elements[idx1] = elements[idx2];
		elements[idx2] = ele;
		elements[idx1]->index = idx1;
		elements[idx2]->index = idx2;
	}

	int parent(int idx){
		return idx/2;
	}

	int left(int idx){
		return idx*2;
	}

	inline int right(int idx){
		return idx*2+1;
	}

	void up(int idx){
		int p = parent(idx);
		while(p)
		{
			assert(elements[idx]);
			assert(elements[p]);
			if(cmp_function(elements[idx],elements[p]))
			{
				swap(idx,p);
				idx = p;
				p = parent(idx);
			}
			else
				break;
		}
	}

	void down(int idx){

		int l = left(idx);
		int r = right(idx);
		int min = idx;
		if(l <= size)
		{
			assert(elements[l]);
			assert(elements[idx]);
		}

		if(l <= size && cmp_function(elements[l],elements[idx]))
			min = l;

		if(r <= size)
		{
			assert(elements[r]);
			assert(elements[min]);
		}

		if(r <= size && cmp_function(elements[r],elements[min]))
			min = r;

		if(min != idx)
		{
			swap(idx,min);
			down(min);
		}		
	}


public:
	minheap(int size,less cmp_func,clear clear_func):size(0),max_size(size),cmp_function(cmp_func),clear_function(clear_func){
		elements = (heapele **)calloc(size,sizeof(*elements));
	}

	~minheap(){
		free(elements);
	}

	void change(heapele *e)
	{
		int idx = e->index;
		down(idx);
		if(idx == e->index)
			up(idx);
	}

	void insert(heapele *e)
	{
		if(e->index)
			return change(e);
		if(size >= max_size-1)
		{
			//expand the heap
			unsigned int new_size = max_size*2;
			heapele** tmp = (heapele**)calloc(new_size,sizeof(*tmp));
			if(!tmp) return;
			memcpy(tmp,elements,max_size*sizeof(*tmp));
			free(elements);
			elements = tmp;
			max_size = new_size;
		}	
		++size;
		elements[size] = e;
		e->index = size;
		up(e->index);	
	}

	/*void remove(heapele *e)
	{
		heapele **back = (heapele**)calloc(1,sizeof(*back)*(size-1));
		int i = 1;
		int c = 1;
		for( ; i <= size;++i)
		{
			elements[i]->index = 0;
			if(elements[i] != e)
				back[c++] = elements[i];
		}
		memset(&(elements[0]),0,sizeof(*back)*max_size);
		size = 0;
		i = 1;
		for(; i < c; ++i) insert(back[i]);
		free(back);
	}*/



	heapele* minheap_min()
	{
		if(!size)	return NULL;
		return elements[1];
	}

	//return the min element and remove it
	heapele* popmin()
	{
		if(size)
		{
			heapele *e = elements[1];
			swap(1,size);
			elements[size] = NULL;
			--size;
			down(1);
			e->index = 0;
			return e;
		}
		return NULL;
	}

	void Clear()
	{
		for(int i = 1; i <= size; ++i){
			elements[i]->index = 0;
			clear_function(elements[i]);
		}
		size = 0;
	}
	

};

#endif
