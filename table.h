#ifndef _table
#define _table


#include <iostream>
#include <vector>
#include "table.h"
#include "node.h"
#include <vector>
#include <map>
#include "link.h"

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
  ostream & Print(ostream &os) const;
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
 public:
  Table();
  Table(Node myself);
  Table(const Table &rhs);
  ostream & Print(ostream &os) const;
  bool updateNeighbor(unsigned dest, unsigned intermediateNode, double lat);
  unsigned getValue(unsigned dest, unsigned intermediateNode);
  unsigned next(unsigned dest);
  bool isTableChanged(unsigned dest, unsigned intermediateNode, double lat);
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif
