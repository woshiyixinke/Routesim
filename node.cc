#include "node.h"
#include "context.h"
#include "error.h"


Node::Node(const unsigned n, SimulationContext *c, double b, double l) : 
    number(n), context(c), bw(b), lat(l) 
{}

Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}

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

deque<Link*> *Node::GetOutgoingLinks() { return context->GetOutgoingLinks(this); }

// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper time
void Node::SendToNeighbors(const RoutingMessage *m)
{
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{

}

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

Node *Node::GetNextHop(const Node *destination) const
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
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " Routing Message: "<<*m;
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  // WRITE
  return 0;
}

Table *Node::GetRoutingTable() const
{
  // WRITE
  return 0;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}
#endif


#if defined(DISTANCEVECTOR)

void Node::LinkHasBeenUpdated(const Link *l)
{
  // update our table
  // send out routing mesages
  cerr << *this<<": Link Update: "<<*l<<endl;
  this->route_table->updateNeighbor(l->GetDest(), l->GetDest(), l->GetLatency());
  deque<Node*> *nodes = this->GetNeighbors();
  deque<Link*> *links = this->GetOutgoingLinks();
  //deque<Node*> nodes = context->GetNeighbors();
  RoutingMessage *message = new RoutingMessage(this->GetNumber(), l->GetDest(), l->GetLatency());

  for(deque<Link*>::iterator i = links->begin(); i != links->end(); i++){
    for(deque<Node*>::iterator j = nodes->begin(); j != nodes->end(); j++){
      if(Node((*i)->GetDest(), 0 ,0 ,0).Matches(**j)){
          context->PostEvent(new Event(context->GetTime() + (*i)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, *j, message));
          break;
      }
    }
  } 
  delete links;
  delete nodes;
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
    Link *l = context->FindMatchingLink(new Link(this->GetNumber(), m->GetSrc(), 0, 0, 0));
    if(this->route_table->isTableChanged(m->GetDest(), m->GetSrc(), l->GetLatency() + m->GetLatency())){
        this->route_table->updateNeighbor(m->GetDest(), m->GetSrc(), l->GetLatency() + m->GetLatency());
        deque<Link*> *links = context->GetOutgoingLinks(this);
        deque<Node*> *nodes = context->GetNeighbors(this);
        RoutingMessage *newMessage = new RoutingMessage(this->GetNumber(), m->GetDest(), l->GetLatency() + m->GetLatency());

        for(deque<Link*>::iterator i = links->begin(); i != links->end(); i++){
          for(deque<Node*>::iterator j = nodes->begin(); j != nodes->end(); j++){
            if(Node((*i)->GetDest(), 0 ,0 ,0).Matches(**j)){
                context->PostEvent(new Event(context->GetTime() + (*i)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, *j, newMessage));
                break;
            }
          }
        } 
        delete links;
        delete nodes;
    }
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}


Node *Node::GetNextHop(const Node *destination) const
{
  deque<Node*> *nodes = context->GetNeighbors(this);
  unsigned res = route_table->next(destination->GetNumber());
  for (deque<Node*>::const_iterator i = nodes->begin(); i != nodes->end(); ++i) {
      if ((Node(res, 0, 0, 0).Matches(**i))) {//if the node exists, return node
          return new Node(**i); 
      }
  }

  return 0;
}

Table *Node::GetRoutingTable() const
{
     return this->route_table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw;
  return os;
}
#endif
