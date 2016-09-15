
#include "common_macros.h"

#include "list.h"
/* ========================================================================== */

static ListIterator * list_iterator_new(void * data) {
	//static ListIterator * list_iterator_new(void * data) {
	ListIterator * it;

	//it = (ListIterator *) calloc(1, sizeof(ListIterator) );

	it = mem_alloc(ListIterator);
	it->next = NULL;
	it->prev = NULL;
	it->data = data;
	return it;
}
/* ========================================================================== */

List * list_new(void) {

	List * list;

	/* memory allocation */
	list = mem_alloc(List);
	__return_val_if_fail(list, NULL);

	/* constructor */
	list->size = 0;
	list->first = NULL;
	list->last = NULL;

	return list;
}
void list_delete_function(List * list) {
	__exit_if_fail(list);
	mem_free(list);
}
/* ========================================================================== */

long int list_get_size(List * list) {
	return list->size;
}
/* ========================================================================== */

static void * __first_insertion(List * list, void * data) {

	ListIterator * it_new;

	it_new = list_iterator_new(data);
	it_new->next = NULL;
	it_new->prev = NULL;
	list->size = 1;
	list->first = it_new;
	list->last = it_new;

	return data;
}
void * list_prepend(List * list, void * data) {

	/* Insere no início
	 */
	ListIterator * it_first;
	ListIterator * it_new;

	__exit_if_fail(list);
	//_warning_if_fail(data);

	if (list->first == NULL) return __first_insertion(list, data);

	it_first = list->first;

	it_new = list_iterator_new(data);
	it_new->next = it_first;
	it_new->prev = NULL;

	/* link at begin */
	it_first->prev = it_new;
	list->first = it_new;

	list->size++;

	return data;
}
void * list_append(List * list, void * data) {

	/*
	 * Insere no final
	 */

	ListIterator * it_last;
	ListIterator * it_new;

	__exit_if_fail(list);
	//_warning_if_fail(data);

	if (list->first == NULL) return __first_insertion(list, data);

	it_last = list->last;

	it_new = list_iterator_new(data);
	it_new->next = NULL;
	it_new->prev = it_last;

	/* link at end */
	it_last->next = it_new;
	list->last = it_new;

	list->size++;

	return data;
}
void * list_insert_sorted(List * list, void * data, ListDataCompareFunction compare_data) {

	ListIterator * it;
	ListIterator * it_new;

	__exit_if_fail(list);
	__exit_if_fail(compare_data);
	//_warning_if_fail(data);

	if (list->first == NULL) return __first_insertion(list, data);

#ifdef AVOID_SAME_INSERTION

	/* Avoid same insertion */
	it = list->first;
	do {
		if (it->data == data) return data;
		it = it->next;
	} while (it != list->first);

#endif

	it = list->first;

	int r = 0;
	while (it)  {
		r = compare_data(data, it->data);
		
		if( r < 0 ) {
			break;
		}
		
		it = it->next;
	}

	if (it == list->first) {
		// insere no inicio
		it_new = list_iterator_new(data);

		it_new->prev = NULL;
		it_new->next = it;
		it->prev = it_new;
		list->first = it_new;
		list->size++;
		return data;
	}

	if (it == NULL) {
		// insere no final
		it_new = list_iterator_new(data);
		it_new->next = NULL;
		it_new->prev = list->last;
		list->last->next = it_new;
		list->last = it_new;
		list->size++;
		return data;
	}

	it_new = list_iterator_new(data);
	it_new->next = it;
	it_new->prev = it->prev;

	it->prev->next = it_new;
	it->prev = it_new;
	list->size++;

	return data;
}
/* ========================================================================== */

ListIterator * list_get_first(List * list) {
	__exit_if_fail(list);
	return list->first;
}
ListIterator * list_get_last(List * list) {
	__exit_if_fail(list);
	return list->last;
}
void * list_get_data(List * list, ListDataCompareFunction data_compare, void * data_arg) {

	ListIterator * it;

	__exit_if_fail(list);
	__exit_if_fail(data_compare);

	it = list->first;
	while (it) {
		if (data_compare(it->data, data_arg) == 0) return it->data;
		it = it->next;
	}

	return NULL;
}
void * list_get_data_at_index(List * list, long int idx) {

	long int c;
	ListIterator * it;

	__exit_if_fail(list);
	__return_val_if_fail(list->size > idx, NULL);

	it = list->first;

	c = 0;
	while (it) {

		if (c == idx) return it->data;

		it = it->next;
		c++;
	}

	return NULL;
}
void * list_have_data(List * list, void * data) {

	ListIterator * it;

	__exit_if_fail(list);
	__exit_if_fail(data);

	it = list->first;
	while (it) {
		if (it->data == data) return it->data;
		it = it->next;
	}

	return NULL;
}
/* ========================================================================== */

void * list_remove_data(List * list, void * data) {

	ListIterator * it;

	__exit_if_fail(list);
	//_warning_if_fail(data);

	if (list->size == 0) return NULL;

	it = list->first;
	while (it) {
		if (it->data == data) break;
		it = it->next;
	}

	/* Not found */
	if (it == NULL) return NULL;
	if (it->data != data) return NULL;

	/* If list have only one element */
	if (list->size == 1) {

		list->first = NULL;
		list->last = NULL;
		list->size--;

		mem_free(it);
		return data;
	}

	/* If remove at start */
	if (list->first == it && it->prev == NULL) {

		list->first = it->next;
		list->first->prev = NULL;
		list->size--;

		mem_free(it);
		return data;
	}

	/* If remove at end */
	if (it->next == NULL) {
		list->last = it->prev;
		list->last->next = NULL;
		list->size--;

		mem_free(it);
		return data;
	}

	/* If remove at middle */
	if (it->prev != NULL && it->next != NULL) {
		ListIterator * left;
		ListIterator * right;

		left = it->prev;
		right = it->next;

		left->next = right;
		right->prev = left;
		list->size--;

		mem_free(it);
		return data;
	}

	//msg_warning("Ooops! List inconsistence.\n");
	return NULL;

}
void * list_remove_data_full(List * list, void * data, ListDataDeleteFunction data_free) {

	if (list_remove_data(list, data)) data_free(data);
	else return NULL;

	return data;
}
void * _old_list_remove(List * list, void * data) {

	ListIterator * it;

	__exit_if_fail(list);
	//_warning_if_fail(data);

	/* Empty list */
	if (list->first == NULL) return NULL;

	/* remove at start */
	if (data == list->first->data) {
		it = list->first;
		list->first = list->first->next;
		if (list->first) list->first->prev = NULL;
		list->size--;

		mem_free(it);
		return data;
	}

	/* remove at end */
	if (data == list->last->data) {
		it = list->last;
		list->last = list->last->prev;
		if (list->last) list->last->next = NULL;
		list->size--;

		mem_free(it);
		return data;
	}

	/* Find data */
	it = list->first;
	while (it) {
		if (it->data == data) break;
		it = it->next;
	}

	/* Not found */
	if (it == NULL) return NULL;

	/* Found! */

	if (list->size == 1) {
		list->size = 0;
		list->first = NULL;
		list->last = NULL;
	} else {
		it->next->prev = it->prev;
		it->prev->next = it->next;
		if (it == list->first) list->first = it->next;
	}

	list->size--;
	mem_free(it);

	return data;
}
void list_remove_all_data(List * list) {

	ListIterator * it;
	ListIterator * ant;

	it = list->first;
	while (it) {
		ant = it;
		it = it->next;
		mem_free(ant);
	}

	list->size = 0;
	list->first = NULL;
	list->last = NULL;
}
void list_remove_all_data_full(List * list, ListDataDeleteFunction data_free) {

	ListIterator * it;
	ListIterator * ant;

	__exit_if_fail(list);
	__exit_if_fail(data_free);

	it = list->first;
	while (it) {

		ant = it;
		it = it->next;

		if (data_free) data_free(ant->data);
		mem_free(ant);
	}

	list->size = 0;
	list->first = NULL;
	list->last = NULL;
}

/* ========================================================================== */



