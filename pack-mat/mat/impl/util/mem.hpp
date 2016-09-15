/*
 * mem.hpp
 *
 *  Created on: Aug 1, 2013
 *      Author: gtl
 */

#ifndef MEM_HPP_
#define MEM_HPP_

namespace impl {
	namespace util {
		namespace mem {

			template<class C> inline C * alloc(unsigned int size = 1) {
				return new C[size];
			}
			template<class C> inline void free(C * pointer) {
				delete[] pointer;
			}

			inline void copy(char * src, char * dest, int nbytes) {
				while (nbytes) {
					*dest = *src;
					dest++;
					src++;
					nbytes--;
				}
			}
			inline void set(char * m, unsigned int nbytes, char v) {
				while (nbytes) {
					*m = v;
					++m;
					nbytes--;
				}
			}
			inline void clear(char * m, unsigned int nbytes) {
				set(m, nbytes, 0);
			}

			namespace access {

				template<typename T> inline static T& rw(const T& x) {
					return const_cast<T&>(x);
				}
				template<typename T> inline static T*& rwp(const T* const & x) {
					return const_cast<T*&>(x);
				}
				template<typename T> inline static T*& rwp(T* const & x) {
					return const_cast<T*&>(x);
				}
				template<typename T> inline static T*& rwp(const T* & x) {
					return const_cast<T*&>(x);
				}

			}
		}
	}
}
#endif /* MEM_HPP_ */
