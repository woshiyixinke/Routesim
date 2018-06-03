#include "node.h"
#include "context.h"
#include "error.h"

#include<climits>
#include<set>
#include<vector>
#include<queue>

Node::Node(const unsigned n, SimulationContext *c, double b, double l) :
    number(n), context(c), bw(b), lat(l)
{
  #if defined(LINKSTATE)
  if(c!=NULL) this->table = new Table();
  #endif
        
  #if defined(DISTANCEVECTOR)
    if (c != NULL) {
      this->route_table = new Table(*this);
      //cerr << "now: " << endl << *this;
    }
  #endif
}

Node::Node()
{ throw GeneralException(); }

#if defined(LINKSTATE)
Node::Node(const Node &rhs) :
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat), table(rhs.table){}
#endif

#if defined(GENERIC)
Node::Node(const Node &rhs) :
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat){}
#endif

#if defined(DISTANCEVECTOR)
Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}
#endif

Node & Node::operator=(const Node &rhs)
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(const unsigned n)
{ number=n;}

unsigned Node::GetNumber() const
{ return number;}

void Node::SetLatency(const double l)
{ lat=l;}

double Node::GetLatency() const
{ return lat;}

void Node::SetBW(const double b)
{ bw=b;}

double Node::GetBW() const
{ return bw;}

Node::~Node()
{}

// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper time
#if defined(LINKSTATE)
void Node::SendToNeighbors(const RoutingMessage *m)
{
  cout<<"SendToNeighbors called"<<endl;
  // iterate through each neighbor
  deque<Node*> *neighbors = GetNeighbors();
  deque<Node*>::iterator iter = neighbors->begin();
  while(iter!=neighbors->end()){
    Node *n = *iter;
    // make a new message for each neighbor
    const RoutingMessage *nm = new RoutingMessage(*m);
    SendToNeighbor(n,nm);
    iter++;
  }
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{
  cout<<"SendToNeighbor called"<<endl;
  Link *l = new Link();
  l->SetSrc(this->number);
  l->SetDest(n->number);
  cout<<"send to neighbor with number "<<n->number<<endl;
  Link *l2 = context->FindMatchingLink(l);
  if(l2){
    double lat = context->GetTime()+l2->GetLatency();
    Node *newNode = new Node(*n);
    RoutingMessage *nm = new RoutingMessage(*m);
    Event *ev =  new Event(lat, ROUTING_MESSAGE_ARRIVAL, newNode, nm);
    context->PostEvent(ev);
  }
}
#endif

#if defined(DISTANCEVECTOR)
// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper time
deque<Link*> *Node::GetOutgoingLinks() { return context->GetOutgoingLinks(this); }
void Node::SendToNeighbors(const RoutingMessage *m)
{
        deque<Node*> *nodes = GetNeighbors();
        deque<Link*> *links = GetOutgoingLinks();
  
  for(deque<Link*>::iterator i = links->begin(); i != links->end(); ++i){
    for(deque<Node*>::iterator j = nodes->begin(); j != nodes->end(); ++j){
      if(Node((*i)->GetDest(), 0 ,0 ,0).Matches(**j)){
          //context->PostEvent(new Event(context->GetTime() + (*i)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, *j, new RoutingMessages(*m)));
          context->PostEvent(new Event(context->GetTime() + (*i)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, *j, new RoutingMessage(*m)));
          
          break;
      }
    }
  } 
 
  delete links;
  delete nodes;
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{    
     cout<<"Node:"<<*n<<"\n";
     const Link *targetLink = new Link(this->number, n->number, NULL, 0, 0);
     //targetLink.Set
     Link *l = context->FindMatchingLink(targetLink);
     if(l != 0){
          context->PostEvent(new Event(context->GetTime() + l->GetLatency(), ROUTING_MESSAGE_ARRIVAL, new Node(*n), new RoutingMessage(*m)));
     }
     delete targetLink;
}
#endif
deque<Node*> *Node::GetNeighbors()
{
  return context->GetNeighbors(this);
}

void Node::SetTimeOut(const double timefromnow)
{
  context->TimeOut(this,timefromnow);
}


bool Node::Matches(const Node &rhs) const
{
  return number==rhs.number;
}


#if defined(GENERIC)
void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this << " got a link update: "<<*l<<endl;
  //Do Something generic:
  SendToNeighbors(new RoutingMessage);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination)
{
  return 0;
}

Table *Node::GetRoutingTable() const
{
  return new Table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}

#endif

#if defined(LINKSTATE)


void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this<<": Link Update: "<<*l<<endl;
  cout<<"LinkHasBeenUpdated called"<<endl;
  // update table
  bool updated = table->updateTableGivenLink(l);
  if(!updated) return;
  // send further messages to neighbors
  unsigned src = l->GetSrc();
  unsigned dest = l->GetDest();
  double lat = l->GetLatency();
  int ver = table->srcDesLatVer[src][dest].second;
  RoutingMessage *m = new RoutingMessage(src, dest, lat, ver);
  SendToNeighbors(m);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " Routing Message: "<<*m;
  cout<<"ProcessIncomingRoutingMessage called"<<endl;
  cout<<"incoming message has src = "<<m->src<<" dest "<<m->dest<<" lat = "<<m->lat<<" ver = "<<m->ver<<endl;
  // update table
  bool updated = table->updateTableGivenMessage(m);
  // send message to all neighbors
  if(updated) SendToNeighbors(m);
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
  //TODO: maybe?
}


// TODO: deleted const
Node *Node::GetNextHop(const Node *destination)
{
  cout<<"___________________"<<endl;
  cout<<"GetNextHop called"<<endl;
  cout<<"src number is "<<this->number<<" and des number is "<<destination->GetNumber()<<endl;
  if(this->number == destination->GetNumber()){
    cout<<"destination is source"<<endl;
    return this;
  }
  map<unsigned, double> dist;// from src to curr node
  map<unsigned, unsigned> prev;// prev of curr node
  set<unsigned> visited; // visited nodes

  //for each vertex v in Graph
  vector<unsigned> allNodes = getAllNodes();
  int numNodes = allNodes.size();
  for(int i = 0; i< numNodes; i++){
    unsigned node = allNodes[i];
    // dist[v] = INF
    dist[node] = UINT_MAX;
    // prev[v] = NULL
    prev[node] = UINT_MAX;
  }
  // dist[source] = 0
  dist[this->number] = 0;

  // Q=vertices.copy()
  priority_queue<pair<unsigned, double>, vector<pair<unsigned, double> >, compare> pq;
  // map<unsigned, pair<double,int> > temp = table->srcDesLatVer[this->number];
  // map<unsigned, pair<double,int> >::iterator iter3 = temp.begin();
  // while(iter3!=temp.end()){
  //   unsigned n = iter3->first;
  //   double lat = findCost(this->number,n);
  //   dist[n]=lat;
  //   pq.push(make_pair(n,lat));
  //   iter3++;
  // }
  pq.push(make_pair(this->number,0));

  //while Q.size() > 0
  while(!pq.empty()){
    pair<unsigned, double> curr = pq.top();
    unsigned currNum = curr.first;
    double lat = curr.second;
    cout<<"currNum is "<<currNum<<" and lat is "<<lat<<endl;
    if(currNum == destination->GetNumber()){
      cout<<"destination found and lat is "<<lat<<endl;;
      break;
    }
    visited.insert(currNum);
    pq.pop();

    map<unsigned, pair<double,int> > neighbors = table->srcDesLatVer[currNum];
    map<unsigned, pair<double,int> >::iterator iter = neighbors.begin();
    while(iter!=neighbors.end()){
      unsigned neighborNum = iter->first;
      cout<<"neighbor of node "<<currNum<<" is "<<neighborNum<<endl;

      double newEdgeDist = findCost(currNum, neighborNum);
      double newDist = dist[currNum] + newEdgeDist;
      cout<<"uptolast distance is "<<dist[currNum]<<endl;
      cout<<"new edge dist is "<<newEdgeDist<<endl;
      cout<<"new sum cost is "<<newDist<<endl;
      if(newDist < dist[neighborNum]){
        dist[neighborNum] = newDist;
        prev[neighborNum] = currNum;
        cout<<"new sum cost is less than recorded before"<<endl;
        cout<<"neighborNum = "<<neighborNum<<endl;
        cout<<"dist[neighborNum] = "<<dist[neighborNum]<<endl;
        cout<<"prev[neighborNum] = "<<prev[neighborNum]<<endl;
      }
      if(visited.find(neighborNum)!=visited.end()){
        iter++;
        continue;
      }
      cout<<"this node has not been visited"<<endl;
      cout<<neighborNum<<" "<<dist[neighborNum]<<" added to pq"<<endl;
      pq.push(make_pair(neighborNum, dist[neighborNum]));
      // visited.insert(neighborNum);
      iter++;
      cout<<endl;
    }
  }
  cout<<"finish looking for shortest path"<<endl;
  unsigned currNum = destination->GetNumber();
  cout<<"currNum is "<<currNum<<endl;
  unsigned prevNum = prev[currNum];
  cout<<"prevNum is "<<prevNum<<endl;
  while(prevNum != this->number){
    cout<<"prevNum is "<<prevNum<<" and currNum is "<<currNum<<endl;
    currNum = prevNum;
    prevNum = prev[currNum];
  }
  cout<<"next hop is "<<currNum<<endl;
  deque<Node*> *neighbors = GetNeighbors();
  deque<Node*>::iterator iter = neighbors->begin();
  while(iter!=neighbors->end()){
    Node *n = *iter;
    if(n->GetNumber() == currNum){
      cout<<"returning next node"<<endl;
      cout<<"___________________"<<endl;
      Node *newNode = new Node(*n);
      return newNode;
    }
    iter++;
  }

  return 0;
}

Table *Node::GetRoutingTable() const
{
  // get a copy of current routing table
  // Table copy = new Table(*table);
  // return copy;
  cout<<"GetRoutingTable called"<<endl;

  //not strictly necessary according to Piazza
  return 0;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}

vector<unsigned>  Node::getAllNodes(){
  vector<unsigned> allNodes;
  map<unsigned, map<unsigned,pair<double, int> > >::iterator iter = table->srcDesLatVer.begin();
  while(iter != table->srcDesLatVer.end()){
    unsigned src = iter->first;
    allNodes.push_back(src);
    iter++;
  }
  return allNodes;
}

double Node::findCost(unsigned src, unsigned dest){
  if(table->srcDesLatVer.find(src)!=table->srcDesLatVer.end()){
    if(table->srcDesLatVer[src].find(dest)!=table->srcDesLatVer[src].end()){
      return table->srcDesLatVer[src][dest].first;
    }
  }
  return -1.0;
}


#endif


#if defined(DISTANCEVECTOR)

void Node::LinkHasBeenUpdated(const Link *l)
{
  // update our table
  // send out routing mesages
  std::cout << "this is updating links\n";
  cerr << *this<<": Link Update: "<<*l<<endl;
  route_table->updateNeighbor(l->GetDest(), l->GetDest(), l->GetLatency());
  
  RoutingMessage *newMessage = new RoutingMessage(GetNumber(), l->GetDest(), l->GetLatency());
  SendToNeighbors(newMessage);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
    Link *l = context->FindMatchingLink(new Link(GetNumber(), m->GetSrc(), 0, 0, 0));
    if(route_table->isTableChanged(m->GetDest(), m->GetSrc(), l->GetLatency() + m->GetLatency())){
          route_table->updateNeighbor(m->GetDest(), m->GetSrc(), l->GetLatency() + m->GetLatency());
          RoutingMessage *newMessage = new RoutingMessage(this->GetNumber(), l->GetDest(), l->GetLatency());
          SendToNeighbors(newMessage);
    }
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}


Node *Node::GetNextHop(const Node *destination)
{
  deque<Node*> *nodes = this->GetNeighbors();
  unsigned res = route_table->next(destination->GetNumber());
  cout<<"res:"<<res<<"\n";
  for (deque<Node*>::const_iterator i = nodes->begin(); i != nodes->end(); ++i) {
      if ((Node(res, 0, 0, 0).Matches(**i))) {//if the node exists, return node
          return new Node(**i); 
      }
  }

  return 0;
}

Table *Node::GetRoutingTable() const
{
     return route_table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw;
  return os;
}
#endif


  
  
