#ifndef P2P_SIMULATION_SIMULATOR_HELPER_H
#define P2P_SIMULATION_SIMULATOR_HELPER_H

#include "configuration.h"

#include "ns3/bulk-send-helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/traffic-control-helper.h"

class SimulatorHelper
{
  public:
    SimulatorHelper(const Configuration& conf);

    void Setup();
    void Run();

  private:
    void SetupNodes();
    void SetupSenderChannel();
    void SetupReceiverChannel();
    void SetupRouterQueue();
    void SetupSenderApplications();
    void SetupReceiverApplications();
    void SetupTracing();

  private:
    const uint32_t m_port;
    const Configuration& m_conf;
    bool m_isInitialized;
    ns3::NodeContainer m_senders;
    ns3::NodeContainer m_receivers;
    ns3::NodeContainer m_gateway;
    ns3::Ipv4AddressHelper m_ipv4Helper;
    ns3::PointToPointHelper m_s_pointToPoint;
    ns3::PointToPointHelper m_r_pointToPoint;
};

#endif /* P2P_SIMULATION_SIMULATOR_HELPER_H */