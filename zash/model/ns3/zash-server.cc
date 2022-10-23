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
#include "ns3/zash-utils.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ZashServer");

NS_OBJECT_ENSURE_REGISTERED (ZashServer);

TypeId
ZashServer::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::ZashServer")
          .SetParent<Application> ()
          .SetGroupName ("Applications")
          .AddConstructor<ZashServer> ()
          .AddAttribute ("Local", "The Address on which to Bind the rx socket.", AddressValue (),
                         MakeAddressAccessor (&ZashServer::m_local), MakeAddressChecker ())
          .AddAttribute ("Protocol", "The type id of the protocol to use for the rx socket.",
                         TypeIdValue (TcpSocketFactory::GetTypeId ()),
                         MakeTypeIdAccessor (&ZashServer::m_tid), MakeTypeIdChecker ())
          .AddAttribute ("EnableSeqTsSizeHeader",
                         "Enable optional header tracing of SeqTsSizeHeader", BooleanValue (false),
                         MakeBooleanAccessor (&ZashServer::m_enableSeqTsSizeHeader),
                         MakeBooleanChecker ())
          .AddTraceSource ("Rx", "A packet has been received",
                           MakeTraceSourceAccessor (&ZashServer::m_rxTrace),
                           "ns3::Packet::AddressTracedCallback")
          .AddTraceSource ("RxWithAddresses", "A packet has been received",
                           MakeTraceSourceAccessor (&ZashServer::m_rxTraceWithAddresses),
                           "ns3::Packet::TwoAddressTracedCallback")
          .AddTraceSource ("RxWithSeqTsSize", "A packet with SeqTsSize header has been received",
                           MakeTraceSourceAccessor (&ZashServer::m_rxTraceWithSeqTsSize),
                           "ns3::ZashServer::SeqTsSizeCallback");
  return tid;
}

ZashServer::ZashServer ()
{
  // NS_LOG_FUNCTION(this);
  m_socket = 0;
  m_totalRx = 0;
}

ZashServer::~ZashServer ()
{
}

uint64_t
ZashServer::GetTotalRx () const
{
  // NS_LOG_FUNCTION(this);
  return m_totalRx;
}

Ptr<Socket>
ZashServer::GetListeningSocket (void) const
{
  // NS_LOG_FUNCTION(this);
  return m_socket;
}

std::list<Ptr<Socket>>
ZashServer::GetAcceptedSockets (void) const
{
  // NS_LOG_FUNCTION(this);
  return m_socketList;
}

void
ZashServer::DoDispose (void)
{
  // NS_LOG_FUNCTION(this);
  m_socket = 0;
  m_socketList.clear ();

  // chain up
  Application::DoDispose ();
}

// Application Methods
void
ZashServer::StartApplication () // Called at time specified by Start
{
  // NS_LOG_FUNCTION(this);
  deviceComponent->auditComponent->fileLog << "Starting Zash Server..." << endl;
  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      if (m_socket->Bind (m_local) == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }
      m_socket->Listen ();
      // m_socket->ShutdownSend();
    }

  if (InetSocketAddress::IsMatchingType (m_local))
    {
      m_localPort = InetSocketAddress::ConvertFrom (m_local).GetPort ();
    }
  else if (Inet6SocketAddress::IsMatchingType (m_local))
    {
      m_localPort = Inet6SocketAddress::ConvertFrom (m_local).GetPort ();
    }
  else
    {
      m_localPort = 0;
    }
  m_socket->SetRecvCallback (MakeCallback (&ZashServer::HandleRead, this));
  m_socket->SetRecvPktInfo (true);
  m_socket->SetAcceptCallback (MakeCallback (&ZashServer::HandleConnectRequest, this),
                               MakeCallback (&ZashServer::HandleAccept, this));
  m_socket->SetCloseCallbacks (MakeCallback (&ZashServer::HandlePeerClose, this),
                               MakeCallback (&ZashServer::HandlePeerError, this));
}

void
ZashServer::StopApplication () // Called at time specified by Stop
{
  // NS_LOG_FUNCTION(this);
  deviceComponent->auditComponent->fileLog << "Stopping Zash Server..." << endl;
  while (!m_socketList.empty ()) // these are accepted sockets, close them
    {
      Ptr<Socket> acceptedSocket = m_socketList.front ();
      m_socketList.pop_front ();
      acceptedSocket->Close ();
    }
  if (m_socket)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
    }
}

void
ZashServer::HandleRead (Ptr<Socket> socket)
{
  // NS_LOG_FUNCTION(this << socket);
  deviceComponent->auditComponent->fileLog << "Handling read Zash Server..." << endl;
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  string newBuffer;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        { // EOF
          break;
        }

      newBuffer.clear ();
      uint8_t *buffer = new uint8_t[packet->GetSize ()];
      packet->CopyData (buffer, packet->GetSize ());
      newBuffer = (char *) buffer;
      size_t endOfMsg = newBuffer.find ("]");
      newBuffer = newBuffer.substr (0, endOfMsg + 1).c_str ();

      if (InetSocketAddress::IsMatchingType (from))
        {
          deviceComponent->auditComponent->fileLog
              << "Received packet from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ()
              << " with message = " << newBuffer << endl;
        }
      else
        {
          deviceComponent->auditComponent->fileLog
              << "Received packet from " << Inet6SocketAddress::ConvertFrom (from).GetIpv6 ()
              << " with message = " << newBuffer << endl;
        }

      m_totalRx += packet->GetSize ();
      if (InetSocketAddress::IsMatchingType (from))
        {
          deviceComponent->auditComponent->fileLog
              << "At time " << Simulator::Now ().As (Time::S) << " server received "
              << packet->GetSize () << " bytes from "
              << InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port "
              << InetSocketAddress::ConvertFrom (from).GetPort () << " total Rx " << m_totalRx
              << " bytes" << endl;
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          deviceComponent->auditComponent->fileLog
              << "At time " << Simulator::Now ().As (Time::S) << " server received "
              << packet->GetSize () << " bytes from "
              << Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port "
              << Inet6SocketAddress::ConvertFrom (from).GetPort () << " total Rx " << m_totalRx
              << " bytes" << endl;
        }

      if (!m_rxTrace.IsEmpty () || !m_rxTraceWithAddresses.IsEmpty () ||
          (!m_rxTraceWithSeqTsSize.IsEmpty () && m_enableSeqTsSizeHeader))
        {
          Ipv4PacketInfoTag interfaceInfo;
          Ipv6PacketInfoTag interface6Info;
          if (packet->RemovePacketTag (interfaceInfo))
            {
              localAddress = InetSocketAddress (interfaceInfo.GetAddress (), m_localPort);
            }
          else if (packet->RemovePacketTag (interface6Info))
            {
              localAddress = Inet6SocketAddress (interface6Info.GetAddress (), m_localPort);
            }
          else
            {
              socket->GetSockName (localAddress);
            }
          m_rxTrace (packet, from);
          m_rxTraceWithAddresses (packet, from, localAddress);

          if (!m_rxTraceWithSeqTsSize.IsEmpty () && m_enableSeqTsSizeHeader)
            {
              PacketReceived (packet, from, localAddress);
            }
        }
    }
  HandlePacket (newBuffer, socket);
}

void
ZashServer::PacketReceived (const Ptr<Packet> &p, const Address &from, const Address &localAddress)
{
  SeqTsSizeHeader header;
  Ptr<Packet> buffer;

  auto itBuffer = m_buffer.find (from);
  if (itBuffer == m_buffer.end ())
    {
      itBuffer = m_buffer.insert (std::make_pair (from, Create<Packet> (0))).first;
    }

  buffer = itBuffer->second;
  buffer->AddAtEnd (p);
  buffer->PeekHeader (header);

  NS_ABORT_IF (header.GetSize () == 0);

  while (buffer->GetSize () >= header.GetSize ())
    {
      NS_LOG_DEBUG ("Removing packet of size " << header.GetSize () << " from buffer of size "
                                               << buffer->GetSize ());
      Ptr<Packet> complete = buffer->CreateFragment (0, static_cast<uint32_t> (header.GetSize ()));
      buffer->RemoveAtStart (static_cast<uint32_t> (header.GetSize ()));

      complete->RemoveHeader (header);

      m_rxTraceWithSeqTsSize (complete, from, localAddress, header);

      if (buffer->GetSize () > header.GetSerializedSize ())
        {
          buffer->PeekHeader (header);
        }
      else
        {
          break;
        }
    }
}

void
ZashServer::HandlePeerClose (Ptr<Socket> socket)
{
  // NS_LOG_FUNCTION(this << socket);
}

void
ZashServer::HandlePeerError (Ptr<Socket> socket)
{
  // NS_LOG_FUNCTION(this << socket);
}

bool
ZashServer::HandleConnectRequest (Ptr<Socket> socket, const Address &from)
{
  // if (InetSocketAddress::IsMatchingType(from)) {
  //   NS_LOG_FUNCTION(this << socket
  //                        << InetSocketAddress::ConvertFrom(from).GetIpv4());
  // } else {
  //   NS_LOG_FUNCTION(this << socket
  //                        << Inet6SocketAddress::ConvertFrom(from).GetIpv6());
  // }

  return true;
}

void
ZashServer::HandleAccept (Ptr<Socket> s, const Address &from)
{
  // if (InetSocketAddress::IsMatchingType(from)) {
  //   NS_LOG_FUNCTION(this << s
  //                        << InetSocketAddress::ConvertFrom(from).GetIpv4());
  // } else {
  //   NS_LOG_FUNCTION(this << s
  //                        << Inet6SocketAddress::ConvertFrom(from).GetIpv6());
  // }
  s->SetRecvCallback (MakeCallback (&ZashServer::HandleRead, this));
  m_socketList.push_back (s);
}

//----------------------------------------------------------------------------------
// ZASH Application Logic
//----------------------------------------------------------------------------------

void
ZashServer::SetDeviceComponent (DeviceComponent *dc)
{
  // NS_LOG_FUNCTION(this << dc);
  deviceComponent = dc;
}

void
ZashServer::HandlePacket (string buffer, Ptr<Socket> socket)
{
  // NS_LOG_FUNCTION(this << buffer);
  if (!buffer.empty ())
    {
      // deviceComponent->auditComponent->printDenProf ();
      // cout << buffer << endl;
      buffer = buffer.substr (1);
      buffer = buffer.substr (0, buffer.size () - 1);
      vector<string> tokens = strTokenize (buffer);

      Request *req;
      int reqId = stoi (tokens[0]);

      if (tokens[1] == "Proof")
        {
          req = requestQueue[reqId];
          if (tokens[2] == "valid")
            {
              deviceComponent->processProof (req, true);
              bool response = deviceComponent->listenRequest (req);

              string respStr = response ? "[Accepted]" : "[Refused]";

              Ptr<Packet> packet = Create<Packet> (
                  reinterpret_cast<const uint8_t *> (respStr.c_str ()), respStr.size ());

              socket->Send (packet);
            }
          else
            {
              deviceComponent->processProof (req, false);
              deviceComponent->authorizationComponent->processUnauthorized (req);

              if (req->attackId)
                {
                  deviceComponent->processAttack (req, false);
                }

              string respStr = "[Refused]";

              Ptr<Packet> packet = Create<Packet> (
                  reinterpret_cast<const uint8_t *> (respStr.c_str ()), respStr.size ());

              socket->Send (packet);
            }

          requestQueue.erase (reqId);
        }
      else
        {
          Device *device = deviceComponent->authorizationComponent->configurationComponent
                               ->devices[stoi (tokens[1])];
          User *user = deviceComponent->authorizationComponent->configurationComponent
                           ->users[stoi (tokens[2])];
          enums::Properties *props =
              deviceComponent->authorizationComponent->configurationComponent->props;
          Context *context = new Context (
              props->AccessWay.at (tokens[3]), props->Localization.at (tokens[4]),
              props->Group.at (tokens[5]),
              props->TimeClass.at (props->timeClasses[props->timeClasses.size () - 1]));
          enums::Enum *action = props->Action.at (tokens[6]);
          int attackId = stoi (tokens[8]);
          time_t currentDate = strToTime (tokens[7].c_str ());

          Request *req = new Request (reqId, device, user, context, action, attackId, currentDate);
          bool response = deviceComponent->listenRequest (req);

          deviceComponent->auditComponent->storeRequestMetrics (req, props);

          if (!response && !req->user->blocked)
            {
              requestQueue.insert ({reqId, req});
              string respStr = "[" + to_string (reqId) + ",Proof]";
              Ptr<Packet> packet = Create<Packet> (
                  reinterpret_cast<const uint8_t *> (respStr.c_str ()), respStr.size ());

              socket->Send (packet);
              return;
            }

          if (req->attackId)
            {
              deviceComponent->processAttack (req, response);
            }

          string respStr =
              response ? "[Accepted]" : (req->user->blocked ? "[Blocked]" : "[Refused]");

          Ptr<Packet> packet = Create<Packet> (reinterpret_cast<const uint8_t *> (respStr.c_str ()),
                                               respStr.size ());

          socket->Send (packet);
        }
    }
}

} // Namespace ns3
