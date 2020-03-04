/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Universita' degli Studi di Napoli Federico II
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Pasquale Imputato <p.imputato@gmail.com>
 *          Stefano Avallone <stefano.avallone@unina.it>
*/

#include "ns3/test.h"
#include "ns3/simulator.h"
#include "ns3/fq-codel-queue-disc.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-packet-filter.h"
#include "ns3/ipv4-queue-disc-item.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-packet-filter.h"
#include "ns3/ipv6-queue-disc-item.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include <fstream>

using namespace ns3;
int32_t pkt_hash;
NS_LOG_COMPONENT_DEFINE ("FqCoDelQueueDisc2");
/**
 * Simple test packet filter able to classify IPv4 packets
 *
 */
class Ipv4TestPacketFilter : public Ipv4PacketFilter
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  Ipv4TestPacketFilter ();
  virtual ~Ipv4TestPacketFilter ();

private:
  virtual int32_t DoClassify (Ptr<QueueDiscItem> item) const;
  virtual bool CheckProtocol (Ptr<QueueDiscItem> item) const;
};

TypeId
Ipv4TestPacketFilter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Ipv4TestPacketFilter")
                          .SetParent<Ipv4PacketFilter> ()
                          .SetGroupName ("Internet")
                          .AddConstructor<Ipv4TestPacketFilter> ();
  return tid;
}

Ipv4TestPacketFilter::Ipv4TestPacketFilter ()
{
}

Ipv4TestPacketFilter::~Ipv4TestPacketFilter ()
{
}

int32_t
Ipv4TestPacketFilter::DoClassify (Ptr<QueueDiscItem> item) const
{
  NS_LOG_DEBUG ("DoClassifyInTest ");
  return pkt_hash;
}

bool
Ipv4TestPacketFilter::CheckProtocol (Ptr<QueueDiscItem> item) const
{
  NS_LOG_DEBUG ("Inside CheckProtocol ");
  return true;
}


/**
* This test is used to determine the collisions in a real world scenario by enqueuing large number of packets 
* with different Hash. DoClassify and CheckProtocol is modified so that the hash returned could be controlled for each packet
* For set associative when the set is filled a new flow arrives to the same set then it is a collision. 
* For the regular hash if a flow hashes into a queue which already contains a flow then it is a collision. 
* Randomized values of Hash was used to generate the hash for the packets and then the results were calculated. 
* These values are in the hashvalues.txt file. In the modified fq-codel-queue-disc.cc the program exits as soon
* as 2000 new flow limit is reached until then new hashes are inserted into the fq-codel and the collision as well
* as the number of new flows is incremented. 
**/


class FqCoDelQueueDiscCollision : public TestCase
{
public:
  FqCoDelQueueDiscCollision ();
  virtual ~FqCoDelQueueDiscCollision ();

private:
  virtual void DoRun (void);
  void AddPacket (Ptr<FqCoDelQueueDisc> queue, Ipv4Header hdr);
};

FqCoDelQueueDiscCollision::FqCoDelQueueDiscCollision ()
    : TestCase ("Test credits and flows status")
{
}

FqCoDelQueueDiscCollision::~FqCoDelQueueDiscCollision ()
{
}

void
FqCoDelQueueDiscCollision::AddPacket (Ptr<FqCoDelQueueDisc> queue, Ipv4Header hdr)
{
  Ptr<Packet> p = Create<Packet> (100);
  Address dest;
  Ptr<Ipv4QueueDiscItem> item = Create<Ipv4QueueDiscItem> (p, dest, 0, hdr);
  queue->Enqueue (item);
}



void
FqCoDelQueueDiscCollision::DoRun (void)
{
  Ptr<FqCoDelQueueDisc> queueDisc = CreateObjectWithAttributes<FqCoDelQueueDisc> ("SetAssociativeHash", BooleanValue (true));
  queueDisc->SetQuantum (90);
  queueDisc->Initialize ();

  Ptr<Ipv4TestPacketFilter> filter = CreateObject<Ipv4TestPacketFilter> ();
  queueDisc->AddPacketFilter (filter);

  Ipv4Header hdr;
  hdr.SetPayloadSize (100);
  hdr.SetSource (Ipv4Address ("10.10.1.1"));
  hdr.SetDestination (Ipv4Address ("10.10.1.2"));
  hdr.SetProtocol (7);
  // Add packets from the flow
  int i = 0;
  std::ifstream in("hashvalues.txt");
    if(!in) {
      std::cout << "Cannot open input file.\n";
    }
  while(1){
    char str[255];
    in.getline(str, 255);  // delim defaults to '\n'
      // if(in) std::cout << str << std::endl;
    unsigned int hash_from_file = static_cast<unsigned int>(std::stoul(std::string{str}));
    pkt_hash = hash_from_file;
    // std::cout<<"Hash"<<ui<<"\n";
    AddPacket (queueDisc, hdr);
    i++;
    
    

  }
  in.close();
  Simulator::Destroy ();
}

class FqCoDelQueueDiscTestSuite : public TestSuite
{
public:
  FqCoDelQueueDiscTestSuite ();
};

FqCoDelQueueDiscTestSuite::FqCoDelQueueDiscTestSuite () : TestSuite ("fq-codel-queue-disc", UNIT)
{
  AddTestCase (new FqCoDelQueueDiscCollision, TestCase::QUICK);
}

static FqCoDelQueueDiscTestSuite fqCoDelQueueDiscTestSuite;

