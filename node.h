#ifndef _node
#define _node

#include <new>
#include <iostream>
#include <deque>
#include <vector>

class RoutingMessage;
class Table;
class Link;
class SimulationContext;

#include "table.h"

using namespace std;

struct compare{
  bool operator()(pair<unsigned, double> f, pair<unsigned, double> s){
    return f.second>s.second;
  }
};
class Node {
 private:
  unsigned number;
  SimulationContext    *context;
  double   bw;
  double   lat;

#if defined(LINKSTATE)
  Table *table;
  public:
   vector<unsigned> getAllNodes();
   double findCost(unsigned src, unsigned dest);
#endif

#if defined(DISTANCEVECTOR)
  Table *route_table;
  public:
      virtual deque<Link*> *GetOutgoingLinks();
#endif

  // students will add protocol-specific data here

 public:
  Node(const unsigned n, SimulationContext *c, double b, double l);
  Node();
  Node(const Node &rhs);
  Node & operator=(const Node &rhs);
  virtual ~Node();

  virtual bool Matches(const Node &rhs) const;

  virtual void SetNumber(const unsigned n);
  virtual unsigned GetNumber() const;

  virtual void SetLatency(const double l);
  virtual double GetLatency() const;
  virtual void SetBW(const double b);
  virtual double GetBW() const;

  virtual void SendToNeighbors(const RoutingMessage *m);
  virtual void SendToNeighbor(const Node *n, const RoutingMessage *m);
  virtual deque<Node*> *GetNeighbors();
  virtual void SetTimeOut(const double timefromnow);

  //
  // Students will WRITE THESE
  //
  virtual void LinkHasBeenUpdated(const Link *l);
  virtual void ProcessIncomingRoutingMessage(const RoutingMessage *m);
  virtual void TimeOut();
  //TODO: deleted const
  virtual Node *GetNextHop(const Node *destination);
  virtual Table *GetRoutingTable() const;

  virtual ostream & Print(ostream &os) const;
  

};

inline ostream & operator<<(ostream &os, const Node &n) { return n.Print(os);}


#endif
