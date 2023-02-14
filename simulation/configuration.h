#ifndef P2P_SIMULATION_CONFIGURATION_H
#define P2P_SIMULATION_CONFIGURATION_H

#include "tcp-tahoe-loss-recovery.h"
#include "tcp-tahoe.h"

#include "ns3/core-module.h"
#include "ns3/error-model.h"
#include "ns3/ipv4-header.h"

using namespace ns3;

/**
 * @brief Struct to store the configuration of the simulation.
 */
struct Configuration
{
    /*********************************
     *TCP Configuration.
     *********************************/
    std::string transport_prot = "TcpTahoe"; //!< Transport protocol.
    uint32_t snd_buf_size = 131072;          //!< Send buffer size.
    uint32_t rcv_buf_size = 131072;          //!< Receive buffer size.
    uint32_t initial_cwnd = 1;               //!< Initial congestion window.
    uint32_t initial_ssthresh = 65535;       //!< Initial slow start threshold.
    uint32_t mtu_bytes = 1500;               //!< MTU in bytes.
    uint32_t adu_bytes = 0;                  //!< Actual segment size (ADU) in bytes.
    bool sack = true;                        //!< Whether to enable Tcp SACK.
    bool nagle = false;                      //!< Whether to disable Nagle's algorithm.
    /*********************************
     *Channel Configuration.
     *********************************/
    double error_p = 0.0;               //!< Error rate of the channel.
    std::string s_bandwidth = "10Mbps"; //!< Bandwidth of the channel of the sender.
    std::string s_delay = "40ms";       //!< Delay of the channel of the sender.
    std::string r_bandwidth = "10Mbps"; //!< Bandwidth of the channel of the receiver.
    std::string r_delay = "40ms";       //!< Delay of the channel of the receiver.
    uint32_t tcp_queue_size = 25;       //!< Size of the queue at the TCP level.
    // https://groups.google.com/g/ns-3-users/c/e15_YvL-7v0
    // uint32_t device_queue_size = 100;
    /*********************************
     * Simulation Configuration.
     *********************************/
    uint32_t run = 0;                //!< Run identifier. Used to seed the random number generator.
    uint16_t n_flows = 1;            //!< Number of concurrent flows competing in the channel.
    double duration = 3.0;           //!< Duration of the simulation in seconds.
    uint64_t max_mbytes_to_send = 0; //!< Maximum number of megabytes to send. 0 means unlimited.
    /*********************************
     * Tracing Configuration.
     *********************************/
    std::string prefix_file_name = "P2P-project"; //!< Prefix of the output trace file.
    std::string graph_output = "png"; //!< Output format of the graph. Can be "png" or "svg".
    bool pcap_tracing = false;        //!< Enable or disable PCAP tracing.
    bool ascii_tracing = false;       //!< Enable or disable ASCII tracing.
};

/**
 * @brief Overload of the << operator to print the configuration.
 * @param os Output stream.
 * @param conf Configuration.
 * @return Output stream.
 */
std::ostream& operator<<(std::ostream& os, const Configuration& conf);

/**
 * @brief Parse the command line arguments and store them in the configuration.
 * @param conf Configuration object to store the values in.
 * @param argc Number of arguments.
 * @param argv Arguments.
 */
void ParseConsoleArgs(Configuration& conf, int argc, char* argv[]);
/**
 * @brief Initialize the default attributes of the simulation with the configuration.
 * @param configuration Configuration.
 */
void InitializeDefaultConfiguration(const Configuration& configuration);

#endif // P2P_SIMULATION_CONFIGURATION_H