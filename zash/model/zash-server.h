/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author:  Giovanni Rosa (giovanni_rosa4@hotmail.com)
 */

#ifndef ZASH_SERVER_H
#define ZASH_SERVER_H

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ptr.h"
#include "ns3/seq-ts-size-header.h"
#include "ns3/traced-callback.h"
#include <unordered_map>

#include "ns3/zash-packet-sink.h"

namespace ns3 {

class Address;
class Socket;
class Packet;

/**
 * \ingroup applications
 * \defgroup packetsink ZashServer
 *
 * This application was written to complement OnOffApplication, but it
 * is more general so a ZashServer name was selected.  Functionally it is
 * important to use in multicast situations, so that reception of the layer-2
 * multicast frames of interest are enabled, but it is also useful for
 * unicast as an example of how you can write something simple to receive
 * packets at the application layer.  Also, if an IP stack generates
 * ICMP Port Unreachable errors, receiving applications will be needed.
 */

/**
 * \ingroup packetsink
 *
 * \brief Receive and consume traffic generated to an IP address and port
 *
 * This application was written to complement OnOffApplication, but it
 * is more general so a ZashServer name was selected.  Functionally it is
 * important to use in multicast situations, so that reception of the layer-2
 * multicast frames of interest are enabled, but it is also useful for
 * unicast as an example of how you can write something simple to receive
 * packets at the application layer.  Also, if an IP stack generates
 * ICMP Port Unreachable errors, receiving applications will be needed.
 *
 * The constructor specifies the Address (IP address and port) and the
 * transport protocol to use.   A virtual Receive () method is installed
 * as a callback on the receiving socket.  By default, when logging is
 * enabled, it prints out the size of packets and their address.
 * A tracing source to Receive() is also available.
 */
class ZashServer : public ZashPacketSink {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId(void);
  ZashServer();

  virtual ~ZashServer();

  //----------------------------------------------------------------------------------
  // ZASH Application Logic
  //----------------------------------------------------------------------------------

  void SetDeviceComponent(DeviceComponent *dc);

private:
  DeviceComponent *deviceComponent;
};

} // namespace ns3

#endif /* ZASH_SERVER_H */
