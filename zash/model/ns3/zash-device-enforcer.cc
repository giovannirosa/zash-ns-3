
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author:  Giovanni Rosa (giovanni_rosa4@hotmail.com)
 */

#include "zash-device-enforcer.h"
#include "ns3/address.h"
#include "ns3/boolean.h"
#include "ns3/data-rate.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet.h"
#include "ns3/pointer.h"
#include "ns3/random-variable-stream.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/socket.h"
#include "ns3/string.h"
#include "ns3/tag.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ZashDeviceEnforcer");

NS_OBJECT_ENSURE_REGISTERED(ZashDeviceEnforcer);

TypeId ZashDeviceEnforcer::GetTypeId(void) {
  static TypeId tid =
      TypeId("ns3::ZashDeviceEnforcer")
          .SetParent<Application>()
          .SetGroupName("Applications")
          .AddConstructor<ZashDeviceEnforcer>()
          .AddAttribute("DataRate", "The data rate in on state.",
                        DataRateValue(DataRate("500kb/s")),
                        MakeDataRateAccessor(&ZashDeviceEnforcer::m_cbrRate),
                        MakeDataRateChecker())
          .AddAttribute("PacketSize", "The size of packets sent in on state",
                        UintegerValue(512),
                        MakeUintegerAccessor(&ZashDeviceEnforcer::m_pktSize),
                        MakeUintegerChecker<uint32_t>(1))
          .AddAttribute("Remote", "The address of the destination",
                        AddressValue(),
                        MakeAddressAccessor(&ZashDeviceEnforcer::m_peer),
                        MakeAddressChecker())
          .AddAttribute("Local",
                        "The Address on which to bind the socket. If not set, "
                        "it is generated automatically.",
                        AddressValue(),
                        MakeAddressAccessor(&ZashDeviceEnforcer::m_local),
                        MakeAddressChecker())
          .AddAttribute(
              "MaxBytes",
              "The total number of bytes to send. Once these bytes are sent, "
              "no packet is sent again, even in on state. The value zero means "
              "that there is no limit.",
              UintegerValue(0),
              MakeUintegerAccessor(&ZashDeviceEnforcer::m_maxBytes),
              MakeUintegerChecker<uint64_t>())
          .AddAttribute("Protocol",
                        "The type of protocol to use. This should be "
                        "a subclass of ns3::SocketFactory",
                        TypeIdValue(TcpSocketFactory::GetTypeId()),
                        MakeTypeIdAccessor(&ZashDeviceEnforcer::m_tid),
                        // This should check for SocketFactory as a parent
                        MakeTypeIdChecker())
          .AddAttribute(
              "EnableSeqTsSizeHeader",
              "Enable use of SeqTsSizeHeader for sequence number and timestamp",
              BooleanValue(false),
              MakeBooleanAccessor(&ZashDeviceEnforcer::m_enableSeqTsSizeHeader),
              MakeBooleanChecker())
          .AddTraceSource("Tx", "A new packet is created and is sent",
                          MakeTraceSourceAccessor(&ZashDeviceEnforcer::m_txTrace),
                          "ns3::Packet::TracedCallback")
          .AddTraceSource(
              "TxWithAddresses", "A new packet is created and is sent",
              MakeTraceSourceAccessor(&ZashDeviceEnforcer::m_txTraceWithAddresses),
              "ns3::Packet::TwoAddressTracedCallback")
          .AddTraceSource(
              "TxWithSeqTsSize", "A new packet is created with SeqTsSizeHeader",
              MakeTraceSourceAccessor(&ZashDeviceEnforcer::m_txTraceWithSeqTsSize),
              "ns3::PacketSink::SeqTsSizeCallback")
          .AddAttribute("Message", "The message to be sent", StringValue(),
                        MakeStringAccessor(&ZashDeviceEnforcer::z_message),
                        MakeStringChecker())
          .AddTraceSource("Traces", "Messages from node",
                          MakeTraceSourceAccessor(&ZashDeviceEnforcer::m_traces),
                          "ns3::ZashDeviceEnforcer::TracedCallback");
  return tid;
}

ZashDeviceEnforcer::ZashDeviceEnforcer()
    : m_socket(0), m_connected(false), m_residualBits(0),
      m_lastStartTime(Seconds(0)), m_totBytes(0), m_unsentPacket(0) {
  NS_LOG_FUNCTION(this);
}

ZashDeviceEnforcer::~ZashDeviceEnforcer() { NS_LOG_FUNCTION(this); }

void ZashDeviceEnforcer::SetMaxBytes(uint64_t maxBytes) {
  NS_LOG_FUNCTION(this << maxBytes);
  m_maxBytes = maxBytes;
}

Ptr<Socket> ZashDeviceEnforcer::GetSocket(void) const {
  NS_LOG_FUNCTION(this);
  return m_socket;
}

void ZashDeviceEnforcer::DoDispose(void) {
  NS_LOG_FUNCTION(this);

  CancelEvents();
  m_socket = 0;
  m_unsentPacket = 0;
  // chain up
  Application::DoDispose();
}

// Application Methods
void ZashDeviceEnforcer::StartApplication() // Called at time specified by Start
{
  NS_LOG_INFO("=======================================================");
  NS_LOG_FUNCTION(this);
  NS_LOG_INFO("Starting " << z_device->name << " @"
                          << Simulator::Now().As(Time::S));

  // Create the socket if not already
  if (!m_socket) {
    m_socket = Socket::CreateSocket(GetNode(), m_tid);
    int ret = -1;

    if (InetSocketAddress::IsMatchingType(m_peer)) {
      NS_LOG_INFO("Socket bind "
                  << InetSocketAddress::ConvertFrom(m_local).GetIpv4() << " to "
                  << InetSocketAddress::ConvertFrom(m_peer).GetIpv4());
    } else {
      NS_LOG_INFO("Socket bind "
                  << Inet6SocketAddress::ConvertFrom(m_local).GetIpv6()
                  << " to "
                  << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6());
    }

    if (!m_local.IsInvalid()) {
      NS_ABORT_MSG_IF((Inet6SocketAddress::IsMatchingType(m_peer) &&
                       InetSocketAddress::IsMatchingType(m_local)) ||
                          (InetSocketAddress::IsMatchingType(m_peer) &&
                           Inet6SocketAddress::IsMatchingType(m_local)),
                      "Incompatible peer and local address IP version");
      ret = m_socket->Bind(m_local);
    } else {
      if (Inet6SocketAddress::IsMatchingType(m_peer)) {
        ret = m_socket->Bind6();
      } else if (InetSocketAddress::IsMatchingType(m_peer) ||
                 PacketSocketAddress::IsMatchingType(m_peer)) {
        ret = m_socket->Bind();
      }
    }

    if (ret == -1) {
      NS_FATAL_ERROR("Failed to bind socket = " << m_socket->GetErrno());
    }

    ret = m_socket->Connect(m_peer);
    m_socket->SetAllowBroadcast(true);
    if (InetSocketAddress::IsMatchingType(m_peer)) {
      NS_LOG_INFO("Socket connect "
                  << InetSocketAddress::ConvertFrom(m_peer).GetIpv4()
                  << " return " << ret);

    } else {
      NS_LOG_INFO("Socket connect "
                  << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6()
                  << " return " << ret);
    }
    m_traces(m_local, m_peer, "Socket connect");
    // m_socket->ShutdownRecv();

    m_socket->SetConnectCallback(
        MakeCallback(&ZashDeviceEnforcer::ConnectionSucceeded, this),
        MakeCallback(&ZashDeviceEnforcer::ConnectionFailed, this));

    m_socket->SetRecvCallback(MakeCallback(&ZashDeviceEnforcer::HandleRead, this));
    m_socket->SetRecvPktInfo(true);
    m_socket->SetAcceptCallback(
        MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
        MakeCallback(&ZashDeviceEnforcer::HandleAccept, this));
    m_socket->SetCloseCallbacks(
        MakeCallback(&ZashDeviceEnforcer::HandlePeerClose, this),
        MakeCallback(&ZashDeviceEnforcer::HandlePeerError, this));
  }
  m_cbrRateFailSafe = m_cbrRate;

  // Insure no pending event
  CancelEvents();
  // If we are not yet connected, there is nothing to do here
  // The ConnectionComplete upcall will start timers at that time
  // if (!m_connected) return;
  // StartSending();
}

void ZashDeviceEnforcer::StopApplication() // Called at time specified by Stop
{
  NS_LOG_FUNCTION(this);
  NS_LOG_INFO("Stopping " << z_device->name << " @"
                          << Simulator::Now().As(Time::S));

  CancelEvents();
  if (m_socket != 0) {
    int ret = m_socket->Close();
    if (InetSocketAddress::IsMatchingType(m_peer)) {
      NS_LOG_INFO("Socket closed "
                  << InetSocketAddress::ConvertFrom(m_peer).GetIpv4()
                  << " return " << ret);
    } else {
      NS_LOG_INFO("Socket closed "
                  << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6()
                  << " return " << ret);
    }
  } else {
    NS_LOG_WARN("ZashDeviceEnforcer found null socket to close in StopApplication");
  }
}

void ZashDeviceEnforcer::CancelEvents() {
  NS_LOG_FUNCTION(this);

  if (m_sendEvent.IsRunning() &&
      m_cbrRateFailSafe == m_cbrRate) { // Cancel the pending send packet event
    // Calculate residual bits since last packet sent
    Time delta(Simulator::Now() - m_lastStartTime);
    int64x64_t bits = delta.To(Time::S) * m_cbrRate.GetBitRate();
    m_residualBits += bits.GetHigh();
  }
  m_cbrRateFailSafe = m_cbrRate;
  Simulator::Cancel(m_sendEvent);
  Simulator::Cancel(m_startStopEvent);
  // Canceling events may cause discontinuity in sequence number if the
  // SeqTsSizeHeader is header, and m_unsentPacket is true
  if (m_unsentPacket) {
    NS_LOG_DEBUG("Discarding cached packet upon CancelEvents ()");
  }
  m_unsentPacket = 0;
}

// Event handlers
void ZashDeviceEnforcer::StartSending(string message) {
  NS_LOG_INFO("=======================================================");
  NS_LOG_FUNCTION(this);
  SetMessage(message);
  m_lastStartTime = Simulator::Now();
  ScheduleNextTx(); // Schedule the send packet event
}

// Private helpers
void ZashDeviceEnforcer::ScheduleNextTx() {
  NS_LOG_FUNCTION(this);

  if (m_maxBytes == 0 || m_totBytes < m_maxBytes) {
    NS_ABORT_MSG_IF(m_residualBits > m_pktSize * 8,
                    "Calculation to compute next send time will overflow");
    uint32_t bits = m_pktSize * 8 - m_residualBits;
    NS_LOG_LOGIC("bits = " << bits);
    Time nextTime(Seconds(
        bits /
        static_cast<double>(m_cbrRate.GetBitRate()))); // Time till next packet
    NS_LOG_LOGIC("nextTime = " << nextTime.As(Time::S));
    m_sendEvent =
        Simulator::Schedule(nextTime, &ZashDeviceEnforcer::SendPacket, this);
  } else { // All done, cancel any pending events
    // StopApplication();
  }
}

void ZashDeviceEnforcer::SendPacket() {
  NS_LOG_FUNCTION(this);

  NS_ASSERT(m_sendEvent.IsExpired());

  Ptr<Packet> packet;
  if (m_unsentPacket) {
    packet = m_unsentPacket;
  } else if (m_enableSeqTsSizeHeader) {
    Address from, to;
    m_socket->GetSockName(from);
    m_socket->GetPeerName(to);
    SeqTsSizeHeader header;
    header.SetSeq(m_seq++);
    header.SetSize(m_pktSize);
    NS_ABORT_IF(m_pktSize < header.GetSerializedSize());
    packet = Create<Packet>(m_pktSize - header.GetSerializedSize());
    // Trace before adding header, for consistency with PacketSink
    m_txTraceWithSeqTsSize(packet, from, to, header);
    packet->AddHeader(header);
  } else {
    NS_LOG_INFO("Creating packet with " << z_message.size() << " size and '"
                                        << z_message << "' message");
    packet = Create<Packet>(
        reinterpret_cast<const uint8_t *>(z_message.c_str()), z_message.size());
  }

  if (InetSocketAddress::IsMatchingType(m_peer)) {
    NS_LOG_INFO("Sending packet from "
                << InetSocketAddress::ConvertFrom(m_local).GetIpv4() << " to "
                << InetSocketAddress::ConvertFrom(m_peer).GetIpv4());
  } else {
    NS_LOG_INFO("Sending packet from "
                << Inet6SocketAddress::ConvertFrom(m_local).GetIpv6() << " to "
                << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6());
  }

  int actual = m_socket->Send(packet);
  z_reqTime = Simulator::Now().ToDouble(Time::MS);
  if ((unsigned)actual == m_pktSize) {
    m_txTrace(packet);
    m_totBytes += m_pktSize;
    m_unsentPacket = 0;
    Address localAddress;
    m_socket->GetSockName(localAddress);
    m_traces(m_local, m_peer, z_message);
    if (InetSocketAddress::IsMatchingType(m_peer)) {
      NS_LOG_INFO("At time " << Simulator::Now().As(Time::S)
                             << " on-off application sent " << packet->GetSize()
                             << " bytes to "
                             << InetSocketAddress::ConvertFrom(m_peer).GetIpv4()
                             << " port "
                             << InetSocketAddress::ConvertFrom(m_peer).GetPort()
                             << " total Tx " << m_totBytes << " bytes");
      m_txTraceWithAddresses(packet, localAddress,
                             InetSocketAddress::ConvertFrom(m_peer));
    } else if (Inet6SocketAddress::IsMatchingType(m_peer)) {
      NS_LOG_INFO("At time "
                  << Simulator::Now().As(Time::S) << " on-off application sent "
                  << packet->GetSize() << " bytes to "
                  << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6()
                  << " port "
                  << Inet6SocketAddress::ConvertFrom(m_peer).GetPort()
                  << " total Tx " << m_totBytes << " bytes");
      m_txTraceWithAddresses(packet, localAddress,
                             Inet6SocketAddress::ConvertFrom(m_peer));
    }
  } else {
    NS_LOG_DEBUG("Unable to send packet; actual "
                 << actual << " size " << m_pktSize
                 << "; caching for later attempt");
    m_unsentPacket = packet;
  }
  m_residualBits = 0;
  m_lastStartTime = Simulator::Now();
}

void ZashDeviceEnforcer::ConnectionSucceeded(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
  NS_LOG_INFO(z_device->name << " connected @" << Simulator::Now().As(Time::S));
  m_connected = true;
  m_traces(m_peer, m_local, "Socket connected");
}

void ZashDeviceEnforcer::ConnectionFailed(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
  NS_FATAL_ERROR(z_device->name << " can't connect " << socket->GetErrno()
                                << " @" << Simulator::Now().As(Time::S));
}

void ZashDeviceEnforcer::HandleRead(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
  NS_LOG_INFO("Handling read zash device...");
  Ptr<Packet> packet;
  Address from;
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
    m_traces(from, m_local, newBuffer);

    if (InetSocketAddress::IsMatchingType(from)) {
      NS_LOG_INFO("Received packet from "
                  << InetSocketAddress::ConvertFrom(from).GetIpv4()
                  << " with message = " << newBuffer);
      NS_LOG_INFO("At time "
                  << Simulator::Now().As(Time::S) << " packet sink received "
                  << packet->GetSize() << " bytes from "
                  << InetSocketAddress::ConvertFrom(from).GetIpv4() << " port "
                  << InetSocketAddress::ConvertFrom(from).GetPort());
    } else if (Inet6SocketAddress::IsMatchingType(from)) {
      NS_LOG_INFO("Received packet from "
                  << Inet6SocketAddress::ConvertFrom(from).GetIpv6()
                  << " with message = " << newBuffer);
      NS_LOG_INFO("At time "
                  << Simulator::Now().As(Time::S) << " packet sink received "
                  << packet->GetSize() << " bytes from "
                  << Inet6SocketAddress::ConvertFrom(from).GetIpv6() << " port "
                  << Inet6SocketAddress::ConvertFrom(from).GetPort());
    }

    z_respTime = Simulator::Now().ToDouble(Time::MS);

    double acrt = z_respTime - z_reqTime;

    z_auditModule->accessControlRT =
        (z_auditModule->accessControlRT + acrt) / 2.0;
    z_auditModule->spatialTemporalLocality =
        z_auditModule->accessControlDistance * z_auditModule->accessControlRT;
  }

  if (newBuffer == "[Accepted]") {
    if (InetSocketAddress::IsMatchingType(m_peer)) {
      NS_LOG_INFO(z_device->name
                  << "(" << InetSocketAddress::ConvertFrom(m_local).GetIpv4()
                  << ") has changed!");
    } else {
      NS_LOG_INFO(z_device->name
                  << "(" << Inet6SocketAddress::ConvertFrom(m_local).GetIpv6()
                  << ") has changed!");
    }

  } else if (newBuffer == "[Refused]") {
    if (InetSocketAddress::IsMatchingType(m_peer)) {
      NS_LOG_INFO(z_device->name
                  << "(" << InetSocketAddress::ConvertFrom(m_local).GetIpv4()
                  << ") has NOT changed!");
    } else {
      NS_LOG_INFO(z_device->name
                  << "(" << Inet6SocketAddress::ConvertFrom(m_local).GetIpv6()
                  << ") has NOT changed!");
    }
  }
}

void ZashDeviceEnforcer::HandlePeerClose(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
}

void ZashDeviceEnforcer::HandlePeerError(Ptr<Socket> socket) {
  NS_LOG_FUNCTION(this << socket);
}

void ZashDeviceEnforcer::HandleAccept(Ptr<Socket> s, const Address &from) {
  NS_LOG_FUNCTION(this << s << Inet6SocketAddress::ConvertFrom(from).GetIpv6());
  s->SetRecvCallback(MakeCallback(&ZashDeviceEnforcer::HandleRead, this));
}

//----------------------------------------------------------------------------------
// ZASH Application Logic
//----------------------------------------------------------------------------------

void ZashDeviceEnforcer::SetDevice(Device *d) {
  NS_LOG_FUNCTION(this << d);
  z_device = d;
}

void ZashDeviceEnforcer::SetMessage(string msg) {
  NS_LOG_FUNCTION(this << msg);
  m_pktSize = msg.size();
  z_message = msg;
}

void ZashDeviceEnforcer::SetAuditModule(AuditComponent *a) {
  NS_LOG_FUNCTION(this << a);
  z_auditModule = a;
}

} // Namespace ns3
