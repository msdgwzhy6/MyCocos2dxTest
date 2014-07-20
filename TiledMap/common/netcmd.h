#ifndef _NETCMD_H
#define _NETCMD_H


enum{
	//client <-> agent
	CMD_CA_BEGIN = 0,
	CMD_CA_LOGIN,
	CMD_CA_END,

	CMD_AC_BEGIN = 100,
	CMD_AC_END,

	//client <-> game
	CMD_CS_BEGIN = 200,
	CMD_CS_MOV,
	CMD_CS_END,

	CMD_SC_BEGIN = 300,
	CMD_SC_ENTERMAP,
	CMD_SC_ENTERSEE,
	CMD_SC_LEAVESEE,
	CMD_SC_MOV,
	CMD_SC_MOV_ARRI,
	CMD_SC_MOV_FAILED,
	CMD_SC_END,

	//client <-> group
	CMD_CG_BEGIN = 400,
	CMD_CG_CREATE,
	CMD_CG_ENTERMAP,  //��������ͼ
	CMD_CG_END,

	CMD_GC_BEGIN = 500,
	CMD_GC_CREATE,
	CMD_GC_BEGINPLY,
	CMD_GC_ERROR,
	CMD_GC_END,

	//gate <-> group
	CMD_AG_BEGIN = 600,
	CMD_AG_LOGIN,                     //gateserver���̵�½��group
	CMD_AG_PLYLOGIN,
	CMD_AG_CLIENT_DISCONN,            //�ͻ������ӶϿ� 
	CMD_AG_END,

	CMD_GA_BEGIN = 700,
	CMD_GA_NOTIFYGAME,
	CMD_GA_BUSY,
	CMD_GA_PLY_INVAILD,
	CMD_GA_CREATE,
	CMD_GA_END,

	//game <-> group

	CMD_GAMEG_BEGIN = 800,
	CMD_GAMEG_LOGIN,                  //gameserver���̵�½��group 
	CMD_GAMEG_END,

	CMD_GGAME_BEGIN = 900,
	//CMD_GGAME_ENTERMAP,
	//CMD_GGAME_LEAVEMAP,
	//CMD_GGAME_DESTROYMAP,
	CMD_GGAME_END,

	//game <-> gate

	CMD_AGAME_BEGIN = 1000,
	CMD_AGAME_LOGIN,                 //gateserver���̵�½��game 
	CMD_AGAME_CLIENT_DISCONN,        //�ͻ������ӶϿ�   
	CMD_AGAME_END,

	CMD_GAMEA_BEGIN = 1100,
	CMD_GAMEA_LOGINRET,              //gameserver��gate login����Ӧ
	CMD_GAMEA_END,

	//dummy cmd
	DUMMY_ON_GATE_DISCONNECTED = 1200,
	DUMMY_ON_GAME_DISCONNECTED,
	
	//rpc
	CMD_RPC_CALL = 1300,
	CMD_RPC_RESPONSE,
};



/*enum{
	//�ͻ��˵������
	CMD_CS_BEGPLY = 1,//�����������ͼ����,
	CMD_CS_MOV,       //�����ƶ�

	//����˵��ͻ���
	CMD_SC_BEGPLY,    //֪ͨ��ҽ����ͼ�����ɹ������Դ�����ͼ������
	CMD_SC_ENTERSEE,  //�������������Ұ
	CMD_SC_LEVSEE,    //�����뿪������Ұ
	CMD_SC_ENDPLY,    //�����뿪������ͼ
	CMD_SC_MOV,       //�����ƶ�

};*/





#endif
