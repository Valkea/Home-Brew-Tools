#include "EventTypes.h"

namespace hbt
{

//*************************************************************************************************/
///// EventType ///////////////////////////////////////////////////////////////////////////////////
	
	std::multimap< std::string, EventType* > EventType::eventTypeList;

	EventType::EventType( U32 id, std::string name ) : id(id), name( name )
	{ if( POUT_INITIALIZED ) EVET_DEBUG( "EventType::EventType(" << name << ")" ); }
	
	EventType::~EventType()
	{ EVET_DEBUG( "EventType::~EventType(" << name << ")" ); }
	
	
//== Public functions ===============================

	EventType& EventType::get( std::string name )
	{
		static U32 index = 0;
		static EventType* tmpEvtT = 0;
		tmpEvtT = getInstanceByName( name );
		
		if( POUT_INITIALIZED ) EVET_DEBUG( "----------EventType::get(" << name << ")" );
		
		if( !tmpEvtT )
		{
			T_EventTypeList::iterator itInsert;
			itInsert = eventTypeList.insert( std::make_pair( name, new EventType( ++index, name ) ) );
			return *((*itInsert).second);
		}
		return *tmpEvtT;
	}
	
	void EventType::free( std::string name )
	{
		T_EventTypeList::iterator it = eventTypeList.find(name);
		if( it != eventTypeList.end() )
		{
			delete it->second;
			eventTypeList.erase( it );
		}
	}
	
	void EventType::free( EventType& ref )
	{
		T_EventTypeList::iterator it;
		for( it = eventTypeList.begin(); it != eventTypeList.end(); ++it )
			if( ref == *(it->second) )
			{
				delete it->second;
				eventTypeList.erase( it );
				break;
			}
	}
	
	void EventType::freeAll()
	{
		T_EventTypeList::iterator it;
		for( it = eventTypeList.begin(); it != eventTypeList.end(); ++it )
			delete it->second;
			
		eventTypeList.clear();
	}
		
	EventType* EventType::getInstanceByName( std::string name )
	{
		T_EventTypeList::iterator it = eventTypeList.find(name);
		if( it != eventTypeList.end() )	return it->second;
		return 0;
	}
	
	EventType* EventType::getInstanceById( U32 id )
	{
		T_EventTypeList::iterator it;
		for( it = eventTypeList.begin(); it != eventTypeList.end(); ++it )
			if( id == it->second->id ) return it->second;
		return 0;
	}
	
	EventType::operator U32() const
	{
		return id;
	}
	
	EventType::operator std::string() const
	{
		return name;
	}
	
	std::ostream& operator<<( std::ostream& os, const EventType& evt)
	{
		os << "EventType[ name:" << evt.name << ", id: " << evt.id << ", address: " << &evt << "]";
		return os;
	}

} // hbt end


