#include "messages.h"


#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const
{
  os << "RoutingMessage()";
  return os;
}
#endif


#if defined(LINKSTATE)

ostream &RoutingMessage::Print(ostream &os) const
{
  return os;
}

RoutingMessage::RoutingMessage()
{}


RoutingMessage::RoutingMessage(const RoutingMessage &rhs)
{}

#endif


#if defined(DISTANCEVECTOR)

ostream &RoutingMessage::Print(ostream &os) const
{
  return os;
}

RoutingMessage::RoutingMessage()
{}


RoutingMessage::RoutingMessage(const RoutingMessage &rhs)
{
	this->src = rhs.src;
	this->dest = rhs.dest;
	this->lat = rhs.lat;
}

RoutingMessage::RoutingMessage(unsigned src, unsigned dest, unsigned lat){
	this->src = src;
	this->dest = dest;
	this->lat = lat;
}

unsigned
RoutingMessage::GetSrc() const{
	return this->src;
}

unsigned 
RoutingMessage::GetDest() const{
	return this->dest;
}

unsigned
RoutingMessage::GetLatency() const{
	return this->lat;
}

#endif

