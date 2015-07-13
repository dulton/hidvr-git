#ifndef __PORT_MANAGE_H__
#define __PORT_MANAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

int PORT_MANAGE_init(unsigned int min,unsigned int max);
int PORT_MANAGE_destroy();
//
int PORT_MANAGE_add_port(unsigned int port);
/*apply 1 port*/
int PORT_MANAGE_apply1_port(unsigned int * const port);
/* apply 2 continuous ports */
int PORT_MANAGE_apply2_port(unsigned int * const port1);
/*
 以下两个接口申请端口时按递增方式申请，同时如果某个端口释放后并不
 会最近使用 (这是上面两个接口的策略，总是从最小值处开始分配)。
*/
int PORT_MANAGE_apply1_port2(unsigned int * const port);
int PORT_MANAGE_apply2_port2(unsigned int * const port1);
//
int PORT_MANAGE_apply1_port3(unsigned int * const port);
int PORT_MANAGE_apply2_port3(unsigned int * const port1);


int PORT_MANAGE_free_port(unsigned int port);
//

#ifdef __cplusplus
}
#endif
#endif


