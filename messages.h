#ifndef _messages
#define _messages

#include <iostream>

#include "node.h"
#include "link.h"

#if defined(GENERIC)
class RoutingMessage {
 public:
  ostream & Print(ostream &os) const;
};
#endif

#if defined(LINKSTATE)
class RoutingMessage {
public:
  RoutingMessage();
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);
  RoutingMessage(unsigned src, unsigned dest, double lat, int ver);
  ostream & Print(ostream &os) const;
  unsigned src;
  unsigned dest;
  double lat;
  int ver;
  // unsigned getSrc();
  // unsigned getDest();
  // double getLat();
  // int getVer();
};
#endif

#if defined(DISTANCEVECTOR)
class RoutingMessage {
 public:
  unsigned src;
  unsigned dest;
  unsigned lat;
  RoutingMessage();
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage(unsigned s, unsigned d, unsigned l);
  RoutingMessage &operator=(const RoutingMessage &rhs);
  virtual unsigned GetSrc() const;
  virtual unsigned GetDest() const;
  virtual unsigned GetLatency() const;
  ostream & Print(ostream &os) const;
};
#endif


inline ostream & operator<<(ostream &os, const RoutingMessage &m) { return m.Print(os);}
#endif
