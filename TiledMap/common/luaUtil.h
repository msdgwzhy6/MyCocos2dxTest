#ifndef _LUAUTIL_H
#define _LUAUTIL_H

extern "C"{
#include <lua.h>  
#include <lauxlib.h>  
#include <lualib.h>
}

//luaPush
template<typename T>
class luaPush
{
public:
	luaPush(lua_State *L,const T &arg)
	{
		lua_pushnumber(L,(lua_Number)arg);
	}
};

//对指针类型的特化
template<typename T>
class luaPush<T*>
{
public:
	luaPush(lua_State *L,const T* arg)
	{
		if(!arg)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,(T*)arg);
	}
};

template<>
class luaPush<char*>
{
public:
	luaPush(lua_State *L,const char *value)
	{
		lua_pushstring(L,value);
	}
};

template<>
class luaPush<std::string>
{
public:
	luaPush(lua_State *L,const std::string &value)
	{
		lua_pushstring(L,value.c_str());
	}
};

//luaTo
template<typename T>
inline T luaTo(lua_State *L,int index)
{
	return lua_tonumber(L,index);
}

template<>
inline std::string luaTo(lua_State *L,int index)
{
	return lua_tostring(L,index);
}

class luaObject{
public:
	luaObject(lua_State *L,int idx){
		if(L){
			this->L = L;
			lua_pushvalue(L,idx);
			this->rindex = luaL_ref(L,LUA_REGISTRYINDEX);
			if(LUA_REFNIL == this->rindex)
			{
				this->L = NULL;
			}
			if(L) counter = new int(1);
		}
	}

	luaObject &operator = (const luaObject & other)
	{
		if(this != &other)
		{
			if(!counter)
			{
				counter = other.counter;
				++(*counter);
				this->rindex = other.rindex;
				this->L = other.L;
			}
			else
			{
				if(--(*counter) <= 0)
				{
					//原来指向的对象现在已经没有其它对象在引用了,可以从lua中释放掉了
					luaL_unref(this->L,LUA_REGISTRYINDEX,this->rindex);
					delete counter;
				}
				counter = other.counter;
				++(*counter);
				this->rindex = other.rindex;
				this->L = other.L;
			}
		}
		return *this;
	}

	luaObject(const luaObject &other)
		:rindex(other.rindex),L(other.L),counter(other.counter)
	{
		++(*counter);
	}

	~luaObject(){
		if(counter){
			if(!(--(*counter)))
			{
				luaL_unref(L,LUA_REGISTRYINDEX,rindex);
				delete counter;
			}
		}
	}

	lua_State *GetLState(){
		return L;
	}

	template<typename RET,typename KEY>
	RET Get(KEY key){
		lua_rawgeti(L,LUA_REGISTRYINDEX,rindex);
		luaPush<KEY>(L,key);
		lua_gettable(L,-2);
		RET ret = luaTo<RET>(L,-1);
		lua_pop(L,2);
		return ret;
	}

	int GetIndex(){
		return rindex;
	}

private:
	lua_State     *L;
	int 		   rindex;
	int*           counter;
};

template<>
inline luaObject luaTo(lua_State *L,int index)
{
	return luaObject(L,index);
}

static bool lObjNext(luaObject &o){
	int __result = lua_next(o.GetLState(),-2);
	if(!__result){
		lua_pop(o.GetLState(),1);
		return false;
	}
	return true;
} 

#define LUAOBJECT_ENUM(OBJ)\
	for(lua_rawgeti(OBJ.GetLState(),LUA_REGISTRYINDEX,OBJ.GetIndex()),lua_pushnil(OBJ.GetLState());\
	lObjNext(OBJ);lua_pop(OBJ.GetLState(),1))

#define KEY(TYPE,L) luaTo<TYPE>(L,-2)

#define VALUE(TYPE,L) luaTo<TYPE>(L,-1) 

#endif