#include "Event.h"

namespace hbt
{
//****************************************************************************************************/
///// Global Events //////////////////////////////////////////////////////////////////////////////////

	const EventType& Event::DISPATCH = EventType::get("EVENT_DISPATCH");
	const EventType& Event::REGISTER = EventType::get("EVENT_REGISTER");
	const EventType& Event::UNREGISTER = EventType::get("EVENT_UNREGISTER");
	
//*************************************************************************************************/
///// Event ///////////////////////////////////////////////////////////////////////////////////////

	Event::Event( const EventType& evtType ) : type( evtType )
	{ EVET_DEBUG( "INIT Event" ); }
	
	Event::~Event()
	{ EVET_DEBUG( "CLOSE Event" ); }

	//-- copy sementics
	Event::Event ( const Event& obj ) : type( obj.type )
	{ EVET_DEBUG( "Event COPY | Deep copy " << this << " from " << &obj ); }
	
	/*Event& Event::operator= ( const Event& obj )
	{ EVET_DEBUG( "Event COPY assignement | operator = " );	
	
		if (this != &obj) const_cast<EventType&>(type) = obj.type;
		return *this;
	}*/
	
	//-- move sementics
	Event::Event ( Event&& obj ) : type( std::ref(obj.type) )
	{ EVET_DEBUG( "Event MOVE" ); }
	
	/*Event& Event::operator= ( Event&& obj )
	{ EVET_DEBUG( "Event MOVE assignement | operator = " );	
		
		const_cast<EventType&>(type) = std::move( obj.type ); //std::swap(functor, obj.functor);
		return *this;
	}*/

	std::ostream& operator<<( std::ostream& os, const Event& evt)
	{
		// remplacer par une fonction virtual afin que l'on puisse modifier le texte en fonction de l'héritier
		os << "Event[ type: " << evt.type.name << ", hashId: " << evt.type.id << ", address: " << &evt /*<< ", EvtT address: " << &evt.type.reference*/ << "]";
		return os;
	}

} // hbt end

