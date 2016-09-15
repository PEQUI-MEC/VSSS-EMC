/*
 * File:   list.h
 * Author: gustavo
 *
 * Created on July 14, 2011, 1:27 PM
 */

#ifndef IMPL_LIST_H
#define	IMPL_LIST_H


#include "mem.h"
#include "common_macros.h"


typedef int (*ListDataCompareFunction)(void * d1, void * d2);
typedef void (*ListDataDeleteFunction)(void * d);

typedef struct _list_iterator ListIterator;

struct _list_iterator {
		void * data;
		ListIterator * next;
		ListIterator * prev;
};

typedef struct _list List;

struct _list {
		long int size;
		ListIterator * first;
		ListIterator * last;
};

List * list_new(void);
long int list_get_size(List * list);

ListIterator * list_get_first(List * list);
ListIterator * list_get_last(List * list);

#define list_insert		list_append
void * list_append(List * list, void * data);
void * list_prepend(List * list, void * data);
void * list_insert_sorted(List * list, void * data, ListDataCompareFunction compare_data);

void * list_get_data(List * list, ListDataCompareFunction data_compare, void * arg);
void * list_get_data_at_index(List * list, long int idx);
void * list_have_data(List * list, void * data);

void * list_remove_data(List * list, void * data);
void * list_remove_data_full(List * list, void * data, ListDataDeleteFunction delete_function);
void list_remove_all_data(List * list);
void list_remove_all_data_full(List * list, ListDataDeleteFunction delete_function);

#define list_delete( list ) { __exit_if_fail(list); mem_free(list); list = NULL;}

#endif	/* IMPL_LIST_H */

