/*
 * MatRef.hpp
 *
 *  Created on: May 3, 2013
 *      Author: gustavo
 */

#ifndef MATREF_HPP_
#define MATREF_HPP_

namespace mat {

	class MatRef {
			////
		private:
			int * ref;
			void __init() {
				ref = new int;
				(*ref) = 0;
			}

		public:
			MatRef() {
				__init();
			}

			inline bool isRef() {
				return ((*ref) > 0);
			}

			~MatRef() {
				if (isRef()) {
					(*ref)--;
				} else {
					delete ref;
					ref = 0;
				}
			}

			bool unRef() {
				if ((*ref) > 0) {
					(*ref)--;
					__init();
					return true;
				}
				return false;
			}

			void makeReferenceTo( const MatRef * r ) {

				if (isRef()) {
					(*ref)--;
					//__init();
				} else {
					delete this->ref;
					ref = 0;
				}

				ref = r->ref;
				(*ref)++;
			}

			inline void makeReferenceTo( const MatRef& r ) {
				makeReferenceTo( &r );
			}

	};
}

#endif /* MATREF_HPP_ */
