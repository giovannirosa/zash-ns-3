/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author:  Giovanni Rosa (giovanni_rosa4@hotmail.com)
 */
#include "zash-packet-sink.h"
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

NS_LOG_COMPONENT_DEFINE("ZashPacketSink");

NS_OBJECT_ENSURE_REGISTERED(ZashPacketSink);

TypeId ZashPacketSink::GetTypeId(void) {
  static TypeId tid =
      TypeId("ns3::ZashPacketSink")
          .SetParent<Application>()
          .SetGroupName("Applications")
          .AddConstructor<ZashPacketSink>()
          .AddAttribute("Local", "The Address on which to Bind the rx socket.",
                        AddressValue(),
                        MakeAddressAccessor(&ZashPacketSink::m_local),
                        MakeAddressChecker())
          .AddAttribute("Protocol",
                        "The type id of the protocol to use for the rx socket.",
                        TypeIdValue(TcpSocketFactory::GetTypeId()),
                        MakeTypeIdAccessor(&ZashPacketSink::m_tid),
                        MakeTypeIdChecker())
          .AddAttribute(
              "EnableSeqTsSizeHeader",
              "Enable optional header tracing of SeqTsSizeHeader",
              BooleanValue(false),
              MakeBooleanAccessor(&ZashPacketSink::m_enableSeqTsSizeHeader),
              MakeBooleanChecker())
          .AddTraceSource("Rx", "A packet has been received",
                          MakeTraceSourceAccessor(&ZashPacketSink::m_rxTrace),
                          "ns3::Packet::AddressTracedCallback")
          .AddTraceSource(
              "RxWithAddresses", "A packet has been received",
              MakeTraceSourceAccessor(&ZashPacketSink::m_rxTraceWithAddresses),
              "ns3::Packet::TwoAddressTracedCallback")
          .AddTraceSource(
              "RxWithSeqTsSize",
              "A packet with SeqTsSize header has been received",
              MakeTraceSourceAccessor(&ZashPacketSink::m_rxTraceWithSeqTsSize),
              "ns3::ZashPacketSink::SeqTsSizeCallback")
          .AddAttribute("Router", "If application is part of a router",
                        BooleanValue(false),
                        MakeBooleanAccessor(&ZashPacketSink::z_router),
                        MakeBooleanChecker());
  return tid;
}

ZashPacketSink::ZashPacketSink() {
  NS_LOG_FUNCTION(this);
  m_socket = 0;
  m_totalRx = 0;
}

ZashPacketSink::~ZashPacketSink() { NS_LOG_FUNCTION(this); }

uint64_t ZashPacketSink::GetTotalRx() const {
  NS_LOG_FUNCTION(this);
  return m_totalRx;
}

Ptr<Socket> ZashPacketSink::GetListeningSocket(void) const {
  NS_LOG_FUNCTION(this);
  return m_socket;
}

std::list<Ptr<Socket>> ZashPacketSink::GetAcceptedSockets(void) const {
  NS_LOG_FUNCTION(this);
  return m_socketList;
}

void ZashPacketSink::DoDispose(void) {
  NS_LOG_FUNCTION(this);
  m_socket = 0;
  m_socketList.clear();

  // chain up
  Application::DoDispose();
}

// Application Methods
void ZashPacketSink::StartApplication() // Called at time specified by Start
{
  NS_LOG_FUNCTION(this);
  NS_LOG_INFO("Starting zash packet sink...");
  // Create the socket if not already
  if (!m_socket) {
    m_socket = Socket::CreateSocket(GetNode(), m_tid);
    if (m_socket->Bind(m_local) == -1) {
      NS_FATAL_ERROR("Failed to bind socket");
    }
    m_socket->Listen();
    m_socket->ShutdownSend();
  }

  if (InetSocketAddress::IsMatchingType(m_local)) {
    m_localPort = InetSocketAddress::ConvertFrom(m_local).GetPort();
  } else if (Inet6SocketAddress::IsMatchingType(m_local)) {
    m_localPort = Inet6SocketAddress::ConvertFrom(m_local).GetPort();
  } else {
    m_localPort = 0;
  }
  m_socket->SetRecvCallback(MakeCallback(&ZashPacketSink::HandleRead, this));
  m_socket->SetRecvPktInfo(true);
  m_socket->SetAcceptCallback(
      MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
      MakeCallback(&ZashPacketSink::HandleAccept, this));
  m_socket->SetCloseCallbacks(
      MakeCallback(&ZashPacketSink::HandlePeerClose, this),
      MakeCallback(&ZashPacketSink::HandlePeerError, this));
}

void ZashPacketSink::StopApplication() // Called at time specified by Stop
{
  NS_LOG_FUNCTION(this);
  NS_LOG_INFO("Stopping zash packet sink...");
  while (!m_socketList.empty()) // these are accepted sockets, close them
  {
    Ptr<Socket> acceptedSocket = m_socketList.front();
    m_socketList.pop_front();
    acceptedSocket->Close();
  }
  if (m_socket) {
    m_socket->Close();
    m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
  }
}

void ZashPacketSink::HandleRead(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
  NS_LOG_INFO("Handling read zash packet sink...");
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  string newBuffer;
  while ((packet = socket->RecvFrom(from))) {
    if (packet->GetSize() == 0) { // EOF
      break;
    }

    newBuffer.clear();
    uint8_t *buffer = new uint8_t[packet->GetSize()];
    packet->CopyData(buffer, packet->GetSize());
    newBuffer = (char *)buffer;
    size_t endOfMsg = newBuffer.find("]");
    newBuffer = newBuffer.substr(0, endOfMsg + 1).c_str();

    NS_LOG_INFO("Received packet from "
                << InetSocketAddress::ConvertFrom(from).GetIpv4()
                << " with message = " << newBuffer);

    m_totalRx += packet->GetSize();
    if (InetSocketAddress::IsMatchingType(from)) {
      NS_LOG_INFO("At time "
                  << Simulator::Now().As(Time::S) << " packet sink received "
                  << packet->GetSize() << " bytes from "
                  << InetSocketAddress::ConvertFrom(from).GetIpv4() << " port "
                  << InetSocketAddress::ConvertFrom(from).GetPort()
                  << " total Rx " << m_totalRx << " bytes");
    } else if (Inet6SocketAddress::IsMatchingType(from)) {
      NS_LOG_INFO("At time "
                  << Simulator::Now().As(Time::S) << " packet sink received "
                  << packet->GetSize() << " bytes from "
                  << Inet6SocketAddress::ConvertFrom(from).GetIpv6() << " port "
                  << Inet6SocketAddress::ConvertFrom(from).GetPort()
                  << " total Rx " << m_totalRx << " bytes");
    }

    if (!m_rxTrace.IsEmpty() || !m_rxTraceWithAddresses.IsEmpty() ||
        (!m_rxTraceWithSeqTsSize.IsEmpty() && m_enableSeqTsSizeHeader)) {
      Ipv4PacketInfoTag interfaceInfo;
      Ipv6PacketInfoTag interface6Info;
      if (packet->RemovePacketTag(interfaceInfo)) {
        localAddress =
            InetSocketAddress(interfaceInfo.GetAddress(), m_localPort);
      } else if (packet->RemovePacketTag(interface6Info)) {
        localAddress =
            Inet6SocketAddress(interface6Info.GetAddress(), m_localPort);
      } else {
        socket->GetSockName(localAddress);
      }
      m_rxTrace(packet, from);
      m_rxTraceWithAddresses(packet, from, localAddress);

      if (!m_rxTraceWithSeqTsSize.IsEmpty() && m_enableSeqTsSizeHeader) {
        PacketReceived(packet, from, localAddress);
      }
    }
  }
  if (z_router) {
    HandlePacket(newBuffer);
  }
}

void ZashPacketSink::PacketReceived(const Ptr<Packet> &p, const Address &from,
                                    const Address &localAddress) {
  SeqTsSizeHeader header;
  Ptr<Packet> buffer;

  auto itBuffer = m_buffer.find(from);
  if (itBuffer == m_buffer.end()) {
    itBuffer = m_buffer.insert(std::make_pair(from, Create<Packet>(0))).first;
  }

  buffer = itBuffer->second;
  buffer->AddAtEnd(p);
  buffer->PeekHeader(header);

  NS_ABORT_IF(header.GetSize() == 0);

  while (buffer->GetSize() >= header.GetSize()) {
    NS_LOG_DEBUG("Removing packet of size " << header.GetSize()
                                            << " from buffer of size "
                                            << buffer->GetSize());
    Ptr<Packet> complete =
        buffer->CreateFragment(0, static_cast<uint32_t>(header.GetSize()));
    buffer->RemoveAtStart(static_cast<uint32_t>(header.GetSize()));

    complete->RemoveHeader(header);

    m_rxTraceWithSeqTsSize(complete, from, localAddress, header);

    if (buffer->GetSize() > header.GetSerializedSize()) {
      buffer->PeekHeader(header);
    } else {
      break;
    }
  }
}

void ZashPacketSink::HandlePeerClose(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
}

void ZashPacketSink::HandlePeerError(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
}

void ZashPacketSink::HandleAccept(Ptr<Socket> s, const Address &from) {
  NS_LOG_FUNCTION(this << s << from);
  s->SetRecvCallback(MakeCallback(&ZashPacketSink::HandleRead, this));
  m_socketList.push_back(s);
}

} // Namespace ns3
