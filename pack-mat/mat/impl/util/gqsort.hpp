/*
 * gqsort.hpp
 *
 *  Created on: Sep 23, 2012
 *      Author: gustavo
 */

#ifndef GQSORT_HPP_
#define GQSORT_HPP_

#include <stdlib.h>
#include <string.h>

typedef int (*GQSortCompareFunction) ( const void * d1, const void * d2);

static void __copy(void * e1, void * e2, long int size) {
	long int i;

	char * b1 = (char *) e1;
	char * b2 = (char *) e2;

	for (i = 0; i < size; i++) {
		*b1 = *b2;
		b1++;
		b2++;
	}
}
static void __gqsort(void * __data, int lo, int ho, long int unit_size, GQSortCompareFunction compare ) {

	/* Elements */
	char * data = (char *) __data;
	char * pivot;
	char * left;
	char * right;
	char * tmp;

	/* Indexes */
	long int l, h, pi;

	tmp = (char *) calloc(1, unit_size);

	l = lo;
	h = ho;

	if (ho > lo) {

		pi = (lo + ho) >> 1;
		pivot = data + (pi * unit_size);

		left = data + (l * unit_size);
		while ((l < ho) && (compare(left, pivot) < 0)) {
			l++;
			left = data + (l * unit_size);
		}

		right = data + (h * unit_size);
		while ((h > lo) && (compare(right, pivot) > 0)) {
			h--;
			right = data + (h * unit_size);
		}

		if (l <= h) {
			__copy(tmp, left, unit_size);
			__copy(left, right, unit_size);
			__copy(right, tmp, unit_size);
			l++;
			h--;
		}
		free(tmp);
		if (lo < h) __gqsort(data, lo, h, unit_size, compare);
		if (l < ho) __gqsort(data, l, ho, unit_size, compare);
	}
}
/* ========================================================================== */

void gqsort(void * base, long int total_elements, long int unit_size, GQSortCompareFunction compare) {

	long int lo;
	long int ho;

	lo = 0;
	ho = total_elements - 1;

	__gqsort(base, lo, ho, unit_size, compare);

}
/* ========================================================================== */

static void quicksort(float * data, int first, int last) {
	int pivot, j, i;
	float temp;

	if (first < last) {
		pivot = first;
		i = first;
		j = last;

		while (i < j) {
			while (data[i] <= data[pivot] && i < last)
				i++;
			while (data[j] > data[pivot])
				j--;
			if (i < j) {
				temp = data[i];
				data[i] = data[j];
				data[j] = temp;
			}
		}

		temp = data[pivot];
		data[pivot] = data[j];
		data[j] = temp;
		quicksort(data, first, j - 1);
		quicksort(data, j + 1, last);

	}
}

#endif /* GQSORT_HPP_ */
