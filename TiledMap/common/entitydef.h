#ifndef _ENTITYDEF_H
#define _ENTITYDEF_H

//客户端服务器共用定义

//场景对象类型定义
enum{
	class_Character, //玩家
	class_Monster,   //怪物
	class_Pickup,    //地表可拾取物
};

//对象属性
enum{
	ATTR_HP = 0,    //当前血量
	ATTR_MAXHP,     //最大血量
	ATTR_SPEED,     //移动速度
	ATTR_MAX,
};

#endif

