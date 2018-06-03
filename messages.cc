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


// RoutingMessage::RoutingMessage(const RoutingMessage &rhs){
//   this->src = rhs.src;
//   this->dest = rhs.dest;
//   this->lat = rhs.lat;
//   this->ver = rhs.ver;
// }

RoutingMessage::RoutingMessage(const RoutingMessage &rhs):src(rhs.src), dest(rhs.dest), lat(rhs.lat), ver(rhs.ver) {}

// RoutingMessage::RoutingMessage(unsigned src, unsigned dest, double lat, int ver){
//   this->src = src;
//   this->dest = dest;
//   this->lat = lat;
//   this->ver = ver;
// }

RoutingMessage::RoutingMessage(unsigned src, unsigned dest, double lat, int ver):src(src), dest(dest), lat(lat), ver(ver) {}

// unsigned RoutingMessage::getSrc(){
//   return this->src
// }
// unsigned RoutingMessage::getDest();
// double RoutingMessage::getLat();
// int RoutingMessage::getVer();

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
	src = rhs.src;
	dest = rhs.dest;
        lat = rhs.lat;
}

RoutingMessage::RoutingMessage(unsigned s, unsigned d, unsigned l){
	src = s;
	dest = d;
	lat = l;
}

unsigned
RoutingMessage::GetSrc() const{
	return src;
}

unsigned 
RoutingMessage::GetDest() const{
	return dest;
}

unsigned
RoutingMessage::GetLatency() const{
	return lat;
}


#endif

