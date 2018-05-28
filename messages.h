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

  RoutingMessage();
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);

  ostream & Print(ostream &os) const;
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
  RoutingMessage(unsigned src, unsigned dest, unsigned lat);
  RoutingMessage &operator=(const RoutingMessage &rhs);
  virtual unsigned GetSrc() const;
  virtual unsigned GetDest() const;
  virtual unsigned GetLatency() const;
  ostream & Print(ostream &os) const;
};
#endif


inline ostream & operator<<(ostream &os, const RoutingMessage &m) { return m.Print(os);}

#endif
