#ifndef _table
#define _table


#include <iostream>
#include <map>
#include <limits>
#include "link.h"
#include "messages.h"
#include "table.h"
#include "node.h"
#include <vector>

using namespace std;

#if defined(GENERIC)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif


#if defined(LINKSTATE)
class Table {
  // Students should write this class
 public:
   map<unsigned,map<unsigned, pair<double, int> > > srcDesLatVer;
   ostream & Print(ostream &os) const;
   bool updateTableGivenLink(const Link *l);
   bool updateTableGivenMessage(const RoutingMessage *m);
   int getTableSize();
   bool updateTable(unsigned src, unsigned dest, double lat, int ver);

};
#endif

#if defined(DISTANCEVECTOR)

#include <deque>
class Link;
class Node;
class Table {
 //row means destiantion, column means the cost of path by passing through this node.
 //dvTable[i][j], the total cost of current node to node i if choosing the path passing through node j.
 private:
  vector<vector<double> > dvTable;
  vector<double> nextHop;
 public:
  Table();
  Table(Node myself);
  ostream & Print(ostream &os) const;
  bool updateNeighbor(unsigned dest, unsigned intermediateNode, double lat);
  double getValue(unsigned dest, unsigned intermediateNode);
  unsigned next(unsigned dest);
  bool isTableChanged(unsigned dest, unsigned intermediateNode, double lat);
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif
