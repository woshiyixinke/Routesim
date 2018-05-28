#include "table.h"
#include "node.h"
#include <vector>
#include <map>
#include "link.h"

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)

#endif

#if defined(DISTANCEVECTOR)
Table::Table(){}
Table::Table(Node myself){
	deque<Link*> *links = myself.GetOutgoingLinks();
        
        for(deque<Link*>::iterator ite = links->begin(); ite != links->end(); ite++){
                unsigned dest = (*ite)->GetDest();
                while(dvTable.size() <= dest){
                        dvTable.push_back(*(new(vector<double>)));
                }
                while(dvTable[dest].size() <= dest){
                        dvTable[dest].push_back((double) -1);
                }
                dvTable[dest][dest] = (*ite)->GetLatency();
        }
        delete links;
}
Table::Table(const Table &rhs):dvTable(rhs.dvTable){}


	
bool
Table::updateNeighbor(unsigned dest,unsigned intermediateNode, double lat){

	while(dvTable.size() <= dest){
            dvTable.push_back(*(new(vector<double>)));
        }

        while(dvTable[dest].size() <= intermediateNode){
        	dvTable[dest].push_back((double) -1);
        }

        dvTable[dest][intermediateNode] = lat;
        return isTableChanged(dest, intermediateNode, lat);

}

unsigned
Table::getValue(unsigned dest, unsigned intermediateNode){
      if(dvTable.size() <= dest || dvTable[dest].size() <= intermediateNode) return -1;
      return dvTable[dest][intermediateNode];
}



bool
Table::isTableChanged(unsigned dest, unsigned intermediateNode, double lat){
		if(getValue(dest, intermediateNode) == (unsigned)-1 || getValue(dest, intermediateNode) > lat) return 1;
        return 0;
}

unsigned
Table::next(unsigned dest){
    if(dvTable.size() <= dest) return -1;

    unsigned res = 0, cnt = 0;
    for (vector<double>::const_iterator i = dvTable[dest].begin(); i != dvTable[dest].end(); ++i) {
      if ((*i) != -1 && ((*i) < dvTable[dest][res] || dvTable[dest][res] == -1))
          res = cnt;

      cnt++;
  }
  return res;

}

#endif
