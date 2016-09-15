/*
 * ReturnFactory.hpp
 *
 *  Created on: Feb 21, 2013
 *      Author: gustavo
 */

#ifndef RETURNFACTORY_HPP_
#define RETURNFACTORY_HPP_

#include "MatrixFunctionBase.hpp"

namespace mat {

	// =====================================================================
	// =====================================================================

	template<class Derived> class ReturnBase {

		public:
			ReturnBase() {
			}
			Derived& get_derived() {
				return static_cast<Derived&>( *this );
			}
	};

	// =====================================================================
	// =====================================================================

	template<class C1> class Ret1 {

		public:
			C1& o1;

		public:
			Ret1( C1& v1 ) :
					o1( v1 ) {
			}
//				template<class MF> void operator =( const MF& mf ) {
//					o1 = mf;
//				}
			template<class MF> void operator =( const MatrixFunctionBase<MF>& mf ) {
				mf.get_derived().apply( o1 );
			}
	};

	// =====================================================================
	// =====================================================================

	template<class C1, class C2> class Ret2: public ReturnBase<Ret2<C1, C2> > {

		public:
			C1& o1;
			C2& o2;

		public:
			Ret2( C1& v1, C2& v2 ) :
					o1( v1 ), o2( v2 ) {
			}
//				template<class MF> void operator =( const MF& mf ) {
//					o1 = mf;
//					o2 = mf;
//				}
			template<class MF> void operator =( const MatrixFunctionBase<MF>& mf ) {
				mf.get_derived().apply( o1, o2 );
			}

	};

	// =====================================================================
	// =====================================================================

	template<class C1, class C2, class C3> class Ret3: public ReturnBase<Ret3<C1, C2, C3> > {

		public:
			C1& o1;
			C2& o2;
			C3& o3;

		public:
			Ret3( C1& v1, C2& v2, C3& v3 ) :
					o1( v1 ), o2( v2 ), o3( v3 ) {
			}
//				template<class MF> void operator =( const MF& mf ) {
//					o1 = mf;
//					o2 = mf;
//					o3 = mf;
//				}
			template<class MF> void operator =( const MatrixFunctionBase<MF>& mf ) {
				mf.get_derived().apply( o1, o2, o3 );
			}

	};

	// =====================================================================
	// =====================================================================

	template<class C1, class C2, class C3, class C4> class Ret4: public ReturnBase<Ret4<C1, C2, C3, C4> > {

		public:
			C1& o1;
			C2& o2;
			C3& o3;
			C4& o4;

		public:
			Ret4( C1& v1, C2& v2, C3& v3, C4& v4 ) :
					o1( v1 ), o2( v2 ), o3( v3 ), o4( v4 ) {
			}
//				template<class MF> void operator =( const MF& mf ) {
//					o1 = mf;
//					o2 = mf;
//					o3 = mf;
//					o4 = mf;
//				}
			template<class MF> void operator =( const MatrixFunctionBase<MF>& mf ) {
				mf.get_derived().apply( o1, o2, o3, o4 );
			}

	};

	template<class C1, class C2, class C3, class C4, class C5> class Ret5: public ReturnBase<Ret5<C1, C2, C3, C4, C5> > {

		public:
			C1& o1;
			C2& o2;
			C3& o3;
			C4& o4;
			C5& o5;

		public:
			Ret5( C1& v1, C2& v2, C3& v3, C4& v4, C5& o5 ) :
					o1( v1 ), o2( v2 ), o3( v3 ), o4( v4 ), o5( o5 ) {
			}
//				template<class MF> void operator =( const MF& mf ) {
//					o1 = mf;
//					o2 = mf;
//					o3 = mf;
//					o4 = mf;
//					o5 = mf;
//				}
			template<class MF> void operator =( const MatrixFunctionBase<MF>& mf ) {
				mf.get_derived().apply( o1, o2, o3, o4, o5 );
			}

	};

	template<class C1, class C2, class C3, class C4, class C5, class C6> class Ret6: public ReturnBase<Ret6<C1, C2, C3, C4, C5, C6> > {

		public:
			C1& o1;
			C2& o2;
			C3& o3;
			C4& o4;
			C5& o5;
			C5& o6;

		public:
			Ret6( C1& v1, C2& v2, C3& v3, C4& v4, C5& o5, C6& o6 ) :
					o1( v1 ), o2( v2 ), o3( v3 ), o4( v4 ), o5( o5 ), o6( o6 ) {
			}
//				template<class MF> void operator =( const MF& mf ) {
//					o1 = mf;
//					o2 = mf;
//					o3 = mf;
//					o4 = mf;
//					o5 = mf;
//				}
			template<class MF> void operator =( const MatrixFunctionBase<MF>& mf ) {
				mf.get_derived().apply( o1, o2, o3, o4, o5, o6 );
			}

	};

	// =====================================================================
	// =====================================================================
	// =====================================================================

	class __rf {
		public:

			template<class C1> static Ret1<C1> r( C1& o1 ) {
				return Ret1<C1>( o1 );
			}

			template<class C1, class C2> static Ret2<C1, C2> r( C1& o1, C2& o2 ) {
				return Ret2<C1, C2>( o1, o2 );
			}

			template<class C1, class C2, class C3> static Ret3<C1, C2, C3> r( C1& o1, C2& o2, C3& o3 ) {
				return Ret3<C1, C2, C3>( o1, o2, o3 );
			}

			template<class C1, class C2, class C3, class C4> static Ret4<C1, C2, C3, C4> r( C1& o1, C2& o2, C3& o3, C4& o4 ) {
				return Ret4<C1, C2, C3, C4>( o1, o2, o3, o4 );
			}

			template<class C1, class C2, class C3, class C4, class C5> static Ret5<C1, C2, C3, C4, C5> r( C1& o1, C2& o2, C3& o3, C4& o4, C5& o5 ) {
				return Ret5<C1, C2, C3, C4, C5>( o1, o2, o3, o4, o5 );
			}

			template<class C1, class C2, class C3, class C4, class C5, class C6> static Ret6<C1, C2, C3, C4, C5, C6> r( C1& o1, C2& o2, C3& o3, C4& o4, C5& o5, C6& o6 ) {
				return Ret6<C1, C2, C3, C4, C5, C6>( o1, o2, o3, o4, o5, o6 );
			}

	};
}

#endif /* RETURNFACTORY_HPP_ */
