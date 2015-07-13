/*
 * cgi_ipc.h
 *
 *  Created on: 2011-11-14
 *      Author: root
 */

#ifndef CGI_IPC_H_
#define CGI_IPC_H_

#define CGI_SEM_PATH "/root/dvr_web/www/cgi-bin/"
#define CGI_SEM_COUNT (2) //信号灯总数

#define POSTER_CGI_INDEX (0) //poster.cgi信号灯索引
#define POSTER_CGI_NUM (1) //poster.cgi资源数量

#define GW_CGI_INDEX (1) //gw.cgi信号灯索引
#define GW_CGI_NUM (1) //gw.cgi资源数量


void CGI_IPC_init();
void CGI_IPC_destory();
void CGI_IPC_attach();
void CGI_IPC_P(int _sem_index);
void CGI_IPC_V(int _sem_index);
#endif /* CGI_IPC_H_ */
