#include "table.h"
#include <typeinfo>

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)
ostream & Table::Print(ostream &os) const{
  os << "------------------------------"<<endl;
  map<unsigned, map<unsigned, pair<double,int> > >::const_iterator iter = srcDesLatVer.begin();
  while(iter!=srcDesLatVer.end()){
    map<unsigned, pair<double,int> > layer2 = iter->second;
    map<unsigned, pair<double,int> >::const_iterator iter2 = layer2.begin();
    while(iter2!=layer2.end()){
      os<<"src is "<<iter->first<<" des is "<<iter2->first<<" lat is "<<iter2->second.first<<" ver is "<<iter2->second.second<<endl;
      iter2++;
    }
    iter++;
  }
  os << "------------------------------"<<endl;
  return os;
}

bool Table::updateTableGivenLink(const Link *l){
  cout<<"updateTableGivenLink called"<<endl;
  // TODO may need to modify
  unsigned src = l->GetSrc();
  unsigned dest = l->GetDest();
  double lat = l->GetLatency();
  if(this->srcDesLatVer.find(src)==this->srcDesLatVer.end()){
    this->srcDesLatVer[src] = map<unsigned, pair<double,int> >();
  }
  if(this->srcDesLatVer[src].find(dest)==this->srcDesLatVer[src].end()){
    this->srcDesLatVer[src][dest] = make_pair(numeric_limits<double>::max(),0);
  }
  int ver = this->srcDesLatVer[src][dest].second;

  return this->updateTable(src, dest, lat, ver+1);
}

bool Table::updateTableGivenMessage(const RoutingMessage *m){
  // TODO may need to modify
  cout<<"updateTableGivenMessagecalled"<<endl;
  unsigned src = m->src;
  unsigned dest = m->dest;
  double lat = m->lat;
  int ver = m->ver;
  return this->updateTable(src, dest, lat, ver);
}

int Table::getTableSize(){
  int size = srcDesLatVer.size();
  return size;
}

bool Table::updateTable(unsigned src, unsigned dest, double lat, int ver){
  cout<<"updateTable called"<<endl;
  cout<<"table before update"<<endl;
  Print(cout);
  cout<<"table size is "<<srcDesLatVer.size()<<endl;
  if(this->srcDesLatVer.count(src)==0){
    this->srcDesLatVer[src] = map<unsigned,pair<double,int> >();
  }
  if(this->srcDesLatVer[src].count(dest)==0){
    this->srcDesLatVer[src][dest] = make_pair(numeric_limits<double>::max(),0);
  }
  cout<<"table size is "<<srcDesLatVer.size()<<endl;
  unsigned prevVer = srcDesLatVer[src][dest].second;
  unsigned prevLat = srcDesLatVer[src][dest].first;
  if(prevVer >= ver and prevLat!=numeric_limits<double>::max()){
    cout<<"a message is not updated"<<endl;
    return false;
  }
  cout<<"table has been updated"<<endl;

  srcDesLatVer[src][dest].first = lat;
  srcDesLatVer[src][dest].second = ver;
  cout<<"table after update"<<endl;
  Print(cout);
  cout<<"table size is "<<srcDesLatVer.size()<<endl;
  return true;
}
#endif

#if defined(DISTANCEVECTOR)
Table::Table(){}
Table::Table(Node myself){
	deque<Link*> *links = myself.GetOutgoingLinks();
        
        for(deque<Link*>::iterator ite = links->begin(); ite != links->end(); ++ite){  
                int destination = (*ite)->GetDest();
		  	int latency = (*ite)->GetLatency();

		  	updateNeighbor(destination, destination, latency);
        }
        delete links;
}



	
bool
Table::updateNeighbor(unsigned dest,unsigned intermediateNode, double lat){

	while(dvTable.size() <= dest){
            dvTable.push_back(*(new(vector<double>)));
	    nextHop.push_back((double) -1);
        }

        while(dvTable[dest].size() <= intermediateNode){
        	dvTable[dest].push_back((double) -1);
        }
	bool res = isTableChanged(dest, intermediateNode, lat);
        dvTable[dest][intermediateNode] = lat;
        //bool res = isTableChanged(dest, intermediateNode, lat);
	
	
	     //if(dvTable.size() <= dest) return -1;
             unsigned n = 0, cnt = 0;
             for (vector<double>::const_iterator i = dvTable[dest].begin(); i != dvTable[dest].end(); ++i) {
               if ((*i) != -1 && ((*i) < dvTable[dest][n] || dvTable[dest][n] == -1))
               {  n = cnt;}
               cnt++;
            }
            
            nextHop[dest] = n;
        
        return res;

}

double
Table::getValue(unsigned dest, unsigned intermediateNode){
      if(dvTable.size() <= dest || dvTable[dest].size() <= intermediateNode) return -1;
      else return dvTable[dest][intermediateNode];
}



bool
Table::isTableChanged(unsigned dest, unsigned intermediateNode, double lat){
        if(getValue(dest, intermediateNode) == -1 || getValue(dest, intermediateNode) > lat) return 1;
        else return 0;
}

unsigned
Table::next(unsigned dest){
    if(dvTable.size() <= dest) return -1;
    return nextHop[dest];
}

#endif

