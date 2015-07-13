/******************************************************************************

  Copyright (C), 2014-, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : stack.h
  Version       : Initial Draft
  Author        : kejiazhw@gmail.com(kaga)
  Created       : 2014/3/21
  Last Modified : 2014/3/21
  Description   : stack
 	
  History       : 
  1.Date        : 2014/3/21
    	Author      : kaga
 	Modification: Created file	
******************************************************************************/

#ifndef __JSTACK_H__
#define __JSTACK_H__

#include "stdinc.h"
#include "cross.h"
#include "cross_lock.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef void *StackItem_t;
typedef struct node * PNode;
typedef int (*fItemCompare)(StackItem_t A, StackItem_t B);

typedef struct node
{
	StackItem_t data;
	PNode down;
}Node;

typedef struct stack
{
	PNode top;
	int size;
	int item_size;
	fItemCompare f_comp;
	HRWLOCK mutex;
}JStack_t;

JStack_t *STACK_init();
JStack_t *STACK_dup(JStack_t *ps);
void STACK_destroy(JStack_t *ps);
void STACK_set_comp(JStack_t *ps, fItemCompare comp);
void STACK_clear(JStack_t *ps);
bool STACK_is_empty(JStack_t *ps);
int STACK_get_size(JStack_t *ps);
PNode STACK_get_top(JStack_t *ps,StackItem_t item);
PNode STACK_push(JStack_t *ps,StackItem_t item); // push to top of stack
PNode STACK_push_to_bottom(JStack_t *ps,StackItem_t item); //push to bottom of stack
PNode STACK_push_by_inc(JStack_t *ps,StackItem_t item, bool allow_same ); //push to bottom of stack
int STACK_pop(JStack_t *ps,StackItem_t pitem); // pop a item from top of stack
int STACK_del(JStack_t *ps,StackItem_t pitem);  // delete a specific item

void STACK_traverse(JStack_t *ps,void (*visit)());
bool STACK_find(JStack_t *ps, StackItem_t pitem);
int STACK_find2(JStack_t *ps, StackItem_t pitem); // find with given compare function, return how many copy

#ifdef __cplusplus
}
#endif


#endif

