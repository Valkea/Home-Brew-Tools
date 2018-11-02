/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 07.04.2011
	Description : This class unpacks tuples, and transmits unpacked arguments using the provided member
				  method or functor.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- tUnpack					[1] 1 byte
	
///// ETA /////////////////////////////////////////////////////////////////////////////////////////
	
	** Bugs			- It seems that functions are using the functor template instead of function
					one. Does it matter ?
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_TUPLE_UNPACK
#define HEADER_TUPLE_UNPACK

#include <tuple>							// std::get
#include <functional>					// std::ref

namespace hbt
{
//*************************************************************************************************/
///// tUnpack /////////////////////////////////////////////////////////////////////////////////////

	/** This static class unpacks tuples, and transmits unpacked arguments using the provided member method, function or functor.
		@tparam i The number of arguments to unpack. @tparam Args The argument type list to use once the tuple is unpacked.
		@tparam ArgsValue The argument value list to use once the tuple is unpacked. @tparam Tr The return type the function, functor or method called once the tuple is unpacked
		@ingroup signalandslot */

	template< unsigned int i >
	class tUnpack
	{
		public:
			template< class Tr, class... Args, class... ArgsValue >
			static Tr unpack( Tr(*func)(Args...), std::tuple<Args...> &tuple, ArgsValue... argsVal)
			{
				tUnpack< i-1 >::unpack( func, tuple, std::ref(std::get<i-1>(tuple)), argsVal... );
			}
			///< Unpacks the tuple and transmits unpacked arguments to the provided function.
			/// @param func The function pointer to call with unpacked arguments. @param tuple The tuple to unpack. @param argsVal The values to add to those unpacked (also used to unpack tuple).
			
			template< class C, class Tr, class... Args, class... ArgsValue >
			static Tr unpack( C *instance, Tr(C::*func)(Args...), std::tuple<Args...> &tuple, ArgsValue... argsVal)
			{
				tUnpack< i-1 >::unpack( instance, func, tuple, std::ref(std::get<i-1>(tuple)), argsVal... );
			}
			///< Unpacks the tuple and transmits unpacked arguments to the provided member method.
			/// @tparam C The type of the class instance used to call the registered method. @param instance The class instance to call with unpacked arguments. @param func The member method pointer to call with unpacked arguments. @param tuple The tuple to unpack. @param argsVal The values to add to those unpacked (also used to unpack tuple).
			
			template< class C, class... Args, class... ArgsValue >
			static void unpack( C* functor, std::tuple<Args...> &tuple, ArgsValue... argsVal)
			{
				tUnpack< i-1 >::unpack( functor, tuple, std::ref(std::get<i-1>(tuple)), argsVal... );
			}
			///< Unpacks the tuple and transmits unpacked arguments to the provided functor.
			/// @tparam C The type of functor. @param functor The functor to call with the unpacked arguments. @param tuple The tuple to unpack. @param argsVal The values to add to those unpacked (also used to unpack tuple).
	};

	template<>
	class tUnpack<0>
	{
		public:
			template< class Tr, class... Args, class... ArgsValue >
			static Tr unpack( Tr(*func)(Args...), std::tuple<Args...> &tuple, ArgsValue... argsVal)
			{
				return (*func)( argsVal... );
			}
			
			template< class C, class Tr, class... Args, class... ArgsValue >
			static Tr unpack( C *instance, Tr(C::*func)(Args...), std::tuple<Args...> &tuple, ArgsValue... argsVal)
			{
				return (instance->*func)( argsVal... );
			}
			
			template< class C, class... Args, class... ArgsValue >
			static void unpack( C* functor, std::tuple<Args...> &tuple, ArgsValue... argsVal)
			{
				(*functor)( argsVal... );
			}
	};
}

//*************************************************************************************************/
///// Examples ////////////////////////////////////////////////////////////////////////////////////
/*! @class hbt::tUnpack
@code
	==> This is an example of how to use the tuple unpacker. <==

	#include "hbt/SignalSlot/TupleUnpack.h"
			
	//== Defines some functions, method or functors
		void theFunction ( int ) { ... };
		class AnyClass	{ void theMethod ( int ) { ... } };
		class AnyFunctor { void operator () ( int ) { ... } };
	
	//== Initializes variables
		AnyClass theClassInstance;
		AnyFunctor theFunctorInstance;
		std::string strA = "tuples !";
		
		std::tuple tupleA( 10, 20.5, "Hello ", strA );
		std::tuple tupleB( 100, 200.5, "Hi " ); //< Only 3 arguments instead of 4 !
	
	//== Unpacks tuple by calling function, method of functor
		tUnpack::unpack< 4 >( &theClassInstance, tupleA );				 		// will fire theFunction with 4 arguments
		tUnpack::unpack< 4 >( &theClass, &AnyClass::theMethod, tupleA );		// will fire theFunction with 4 arguments
		tUnpack::unpack< 4 >( &theFunctorInstance, tupleA );					// will fire theFunction with 4 arguments
	
		tUnpack::unpack< 3 >( &theClassInstance, tupleB, strA );				// will fire theFunction with 4 arguments
		tUnpack::unpack< 3 >( &theClass, &AnyClass::theMethod, tupleB, strA );	// will fire theFunction with 4 arguments
		tUnpack::unpack< 3 >( &theFunctorInstance, tupleB, strA );				// will fire theFunction with 4 arguments
@endcode */

#endif

