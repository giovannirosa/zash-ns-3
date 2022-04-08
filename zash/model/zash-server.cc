/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author:  Giovanni Rosa (giovanni_rosa4@hotmail.com)
 */
#include "zash-server.h"
#include "ns3/address-utils.h"
#include "ns3/address.h"
#include "ns3/boolean.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv6-packet-info-tag.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/socket.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/tcp-socket.h"
#include "ns3/trace-source-accessor.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ZashServer");

NS_OBJECT_ENSURE_REGISTERED(ZashServer);

TypeId ZashServer::GetTypeId(void) {
  static TypeId tid = TypeId("ns3::ZashServer")
                          .SetParent<ZashPacketSink>()
                          .SetGroupName("Applications")
                          .AddConstructor<ZashServer>();
  return tid;
}

ZashServer::ZashServer() { NS_LOG_FUNCTION(this); }

ZashServer::~ZashServer() { NS_LOG_FUNCTION(this); }

//----------------------------------------------------------------------------------
// ZASH Application Logic
//----------------------------------------------------------------------------------

void ZashServer::SetDeviceComponent(DeviceComponent *dc) {
  NS_LOG_FUNCTION(this << dc);
  deviceComponent = dc;
}

} // Namespace ns3
