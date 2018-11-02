/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 07.04.2011
	Description : This class should provide an easy way to register method or function pointers
				  under a unique type.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- IDelegate				[4] 8 bytes
	- MethodDelegate		[4] 20 bytes
	- FunctionDelegate		[4] 12 bytes
	- FunctorDelegate		[4] 12 bytes
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_FUNC_DELGATE
#define HEADER_FUNC_DELGATE

#include <iostream>					// cout
#include <typeinfo>					// typeid

#include "PortableTypes.h" 			// define F16, F32, U16, U32 etc...

namespace hbt
{

//*************************************************************************************************/
///// IDelegate ///////////////////////////////////////////////////////////////////////////////////

	/** This abstract class aggregates method, functions and functor pointers under a unique type.
		@description Class member method pointers, functor pointers or classical function pointers can be registered using
		inheritors of this abstract class (MethodDelegate, FunctionDelegate and FunctorDelegate), whatever they are virtual,
		static and or const as long as they are waiting for the same arguments list (same order, same types, same number)...
		@ingroup delegate
		@tparam Args The argument list of all MethodDelegate, FunctionDelegate and FunctorDelegate using this IDelegate. */
	
	template < class... Args >
	struct IDelegate
	{
		IDelegate() : count(0) {}
		virtual ~IDelegate() {}
		
		virtual bool operator () ( Args&... args ) const = 0;
		///< Dynamically calls the functor method of the inheritors.
		/// @param args The arguments to transmit to the dynamically selected functor.
		
		virtual bool operator == ( const IDelegate& obj ) const = 0;
		///< Dynamically calls the operator == of the inheritors.
		/// @param obj The IDelegate to test equality with.
		
												// VIRTUAL		//[4] 4 bytes
		U32 count;												//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 8 bytes
	
	
//*************************************************************************************************/
///// MethodDelegate //////////////////////////////////////////////////////////////////////////////

	/** This class registers class member method pointers.
		@ingroup delegate
		@tparam C The type of the class instance used to call the registered method. @tparam Tr The return type of the registered method. @tparam Args The argument list of the registered method. */
	
	template < class C, class Tr, class... Args >
	struct MethodDelegate : IDelegate<Args...>
	{
		typedef Tr(C::*eventMethod)(Args...);
		virtual ~MethodDelegate(){}
		
		MethodDelegate( C& target, eventMethod method ) : target(target), method(method) {}
		///< A constructor.
		/// @param target The class member instance used to call the method. @param method The class member method pointer to register.
		
		bool operator () ( Args&... args ) const
		{
			(target.*(method))(args...);
		}
		///< Invokes the registered method with the provided arguments.
		/// @param args The arguments to transmit to the invoked method.
		
		bool operator == ( const IDelegate<Args...>& obj ) const
		{
			if( typeid(*this) == typeid(obj) )
			{
				const MethodDelegate<C,Tr,Args...>& tmpCast = static_cast<const MethodDelegate<C,Tr,Args...>&> (obj);
				return ((method == tmpCast.method) && (&target == &tmpCast.target)) ? true : false;
			}
			else
				return false;
		}
		///< Tests equality with an %IDelegate instance.
		/// @param obj The %IDelegate instance to compare with.
		
		private:								// IDelegate	//[4] 8 bytes
			eventMethod method;									//[4] 8 bytes
			C& target;											//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 20 bytes
	
	
//*************************************************************************************************/
///// FunctionDelegate ////////////////////////////////////////////////////////////////////////////
	
	/** This class registers function pointers.
		@ingroup delegate
		@tparam Tr The return type of the registered function. @tparam Args The argument list of the registered function.*/

	template < class Tr, class... Args >
	struct FunctionDelegate : IDelegate<Args...>
	{	
		typedef Tr(*eventFunction)(Args...);
		virtual ~FunctionDelegate(){}
		
		FunctionDelegate( eventFunction funcPtr ) : funcPtr( funcPtr ){}
		///< A constructor.
		/// @param funcPtr The function pointer to register.
		
		bool operator () ( Args&... args ) const
		{
			(funcPtr)(args...);
		}
		///< Invokes the registered function with the provided arguments.
		/// @param args The arguments to transmit to the invoked function.
		
		bool operator == ( const IDelegate<Args...>& obj ) const
		{
			if( typeid(*this) == typeid(obj) )
			{
				const FunctionDelegate<Tr,Args...>& tmpCast = static_cast<const FunctionDelegate<Tr,Args...>&> (obj);
				return (funcPtr == tmpCast.funcPtr) ? true : false;
			}
			else
				return false;
		}
		///< Tests equality with an %IDelegate instance.
		/// @param obj The %IDelegate instance to compare with.
		
		private:								// IDelegate	//[4] 8 bytes
			eventFunction funcPtr;								//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 12 bytes
	
	
//*************************************************************************************************/
///// FunctorDelegate /////////////////////////////////////////////////////////////////////////////

	/** This class registers functor pointers.
		@ingroup delegate
		@tparam C The type of the functor. @tparam Args The argument list of the registered functor. */
	
	template < class C, class... Args >
	struct FunctorDelegate : IDelegate<Args...>
	{
		virtual ~FunctorDelegate(){}
		
		FunctorDelegate( C& functor ) : functor(functor) {}
		///< A constructor.
		/// @param functor The functor instance used to call the method param.
		
		bool operator () ( Args&... args ) const
		{
			functor(args...);
		}
		///< Invokes the registered functor with the provided arguments.
		/// @param args The arguments to transmit to the invoked functor.
		
		bool operator == ( const IDelegate<Args...>& obj ) const
		{
			if( typeid(*this) == typeid(obj) )
			{
				const FunctorDelegate<C,Args...>& tmpCast = static_cast<const FunctorDelegate<C,Args...>&> (obj);
				return (&functor == &tmpCast.functor) ? true : false;
			}
			else
				return false;
		}
		///< Tests equality with an %IDelegate instance.
		/// @param obj The %IDelegate instance to compare with.
		
		private:								// IDelegate	//[4] 8 bytes
			C& functor;											//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 12 bytes

}
#endif
