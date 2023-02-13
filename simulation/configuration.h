#ifndef P2P_SIMULATION_CONFIGURATION_H
#define P2P_SIMULATION_CONFIGURATION_H

#include "tcp-tahoe.h"

#include "ns3/core-module.h"
#include "ns3/ipv4-header.h"

struct Configuration
{
    /*********************************
     *TCP Configuration.
     *********************************/
    /**
     * @brief Tcp Variant.
     * Either TcpTahoe or TcpReno.
     */
    std::string transport_prot = "TcpTahoe";
    /** @brief Send buffer size. */
    uint32_t snd_buf_size = 131072;
    /** @brief Receive buffer size. */
    uint32_t rcv_buf_size = 131072;
    /** @brief Initial congestion window. */
    uint32_t initial_cwnd = 1;
    /** @brief Initial slow start threshold. */
    uint32_t initial_ssthresh = 65535;
    bool sack = true;
    /** @brief Whether to disable Nagle's algorithm. */
    bool disable_nagle = false;
    /*********************************
     *Channel Configuration.
     *********************************/
    /** @brief Error rate of the channel. */
    double error_p = 0.0;
    std::string bandwidth = "2Mbps";
    std::string delay = "0.01ms";
    std::string access_bandwidth = "10Mbps";
    std::string access_delay = "45ms";
    uint64_t mbytes_to_send = 0;
    uint32_t mtu_bytes = 400;
    /** @brief Duration of the simulation in seconds. */
    double duration = 3.0;
    /*********************************
     * Simulation Configuration.
     *********************************/
    /** @brief Identifier of the run. It is used as the randomness seed. */
    uint32_t run = 0;
    /** @brief Number of flows to simulate. */
    uint16_t num_flows = 1;
    /*********************************
     * Tracing Configuration.
     *********************************/
    /** @brief Prefix of the output trace file. */
    std::string prefix_file_name = "P2P-project";
    /** @brief Enable or disable PCAP tracing. */
    bool pcap_tracing = false;
    /** @brief Enable or disable ASCII tracing. */
    bool ascii_tracing = false;
};

std::ostream& operator<<(std::ostream& os, const Configuration& conf);

void ParseConsoleArgs(Configuration& conf, int argc, char* argv[]);
void InitializeDefaultConfiguration(const Configuration& configuration);

#endif // P2P_SIMULATION_CONFIGURATION_H