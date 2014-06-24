#ifndef _NETCMD_H
#define _NETCMD_H

enum{
	//客户端到服务端
	CMD_CS_BEGPLY = 1,//玩家请求进入地图场景,
	CMD_CS_MOV,       //主角移动

	//服务端到客户端
	CMD_SC_BEGPLY,    //通知玩家进入地图场景成功，可以创建地图和主角
	CMD_SC_ENTERSEE,  //对象进入主角视野
	CMD_SC_LEVSEE,    //对象离开主角视野
	CMD_SC_ENDPLY,    //主角离开场景地图
	CMD_SC_MOV,       //对象移动

};





#endif