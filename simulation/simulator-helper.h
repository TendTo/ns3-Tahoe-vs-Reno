#ifndef P2P_SIMULATION_SIMULATOR_HELPER_H
#define P2P_SIMULATION_SIMULATOR_HELPER_H

#include "configuration.h"
#include "tracer.h"

#include "ns3/bulk-send-helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/traffic-control-helper.h"

using namespace ns3;

/**
 * @brief SimulatorHelper class.
 * It is used to setup and run the simulation.
 * It creates nodes, channels, applications, configures them and initializes tracing.
 * Using the private properties, it can easily be extended be used to run multiple simulations with
 * different configurations.
 * 
 * The simulation creates the following network configuration:
 * 
 * n_1 ----
 *         \
 * n_2 ---- g ---- r
 *         /
 * n_n ----
 * 
 * n_i: sender node
 * g: gateway node
 * r: receiver node
 * 
 * Each sender node is connected to the gateway node with a point-to-point channel.
 * The gateway node is connected to the receiver node with a point-to-point channel.
 * Both gateway and receiver nodes use a RED queue and may drop packets.
 */
class SimulatorHelper
{
  public:
    /**
     * @brief SimulatorHelper constructor.
     * @param conf Simulation configuration.
     * @param tracer Simulation tracer.
     */
    SimulatorHelper(const Configuration& conf, Tracer& tracer);

    /**
     * @brief Setup the simulation.
     * It creates nodes, channels, applications, configures them and initializes tracing.
     */
    void Setup();
    /**
     * @brief Start the simulation.
     * Can be called only after Setup().
     */
    void Run();

  private:
    /**
     * @brief Creates the nodes.
     * It creates n_flows senders, 1 receiver and 1 gateway.
     */
    void SetupNodes();
    /**
     * @brief Creates the sender channel.
     * It creates a point-to-point channel between each sender and the gateway.
     * Sets the data rate and delay of the channel.
     */
    void SetupSenderChannel();
    /**
     * @brief Creates the receiver channel.
     * It creates a point-to-point channel between the gateway and the receiver.
     * Sets the data rate and delay of the channel, as well as the error rate error_p, and the
     * number of packets the RED queue will accept at most.
     */
    void SetupReceiverChannel();
    /**
     * @brief Creates the sender applications.
     * It creates a BulkSendApplication for each sender, all sending towards the receiver.
     * Sets the packet size, the number of bytes to send, the start time and the stop time of the
     * application.
     */
    void SetupSenderApplications();
    /**
     * @brief Creates the receiver applications.
     * It creates a PacketSinkApplication for the receiver, accepting connections from all senders.
     * Sets the start time and the stop time of the application.
     */
    void SetupReceiverApplications();
    /**
     * @brief Enables tracing.
     * It schedules the tracing methods used to track the trace sources.
     * It also initializes both ascii and pcap tracing for the sender and receiver channels, if
     * enabled.
     */
    void SetupTracing();

  private:
    const uint32_t m_port;               //!< Port used by the receiver application.
    const Configuration& m_conf;         //!< Simulation configuration.
    bool m_isInitialized;                //!< True if the simulation has been initialized.
    Tracer m_tracer;                     //!< Simulation tracer.
    NodeContainer m_senders;             //!< Senders nodes.
    NodeContainer m_receivers;           //!< Receiver node.
    NodeContainer m_gateway;             //!< Gateway node.
    Ipv4AddressHelper m_ipv4Helper;      //!< Ipv4 address generator.
    PointToPointHelper m_s_pointToPoint; //!< Sender channel helper.
    PointToPointHelper m_r_pointToPoint; //!< Receiver channel helper.
};

#endif /* P2P_SIMULATION_SIMULATOR_HELPER_H */