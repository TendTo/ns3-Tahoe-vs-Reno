#ifndef P2P_SIMULATION_CONFIGURATION_H
#define P2P_SIMULATION_CONFIGURATION_H

#include "tcp-tahoe.h"

#include "ns3/core-module.h"
#include "ns3/error-model.h"
#include "ns3/ipv4-header.h"

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
    bool sack = true;                        //!< Whether to enable Tcp SACK.
    bool disable_nagle = false;              //!< Whether to disable Nagle's algorithm.
    uint32_t mtu_bytes = 400;                //!< MTU in bytes.
    uint32_t adu_bytes = 0;                  //!< Actual segment size (ADU) in bytes.
    /*********************************
     *Channel Configuration.
     *********************************/
    double error_p = 0.0;                    //!< Error rate of the channel.
    double delay = 0.01;                     //!< Delay of the channel in milliseconds.
    std::string access_bandwidth = "10Mbps"; //!< Bandwidth of the channel.
    std::string access_delay = "45ms";       //!< Delay of the channel in milliseconds.
    std::string bandwidth = "2Mbps";         //!< Bandwidth of the channel.
    uint64_t max_mbytes_to_send = 0; //!< Maximum number of megabytes to send. 0 means unlimited.
    double duration = 3.0;           //!< Duration of the simulation in seconds.
    /*********************************
     * Simulation Configuration.
     *********************************/
    uint32_t run = 0;     //!< Run identifier. Used to seed the random number generator.
    uint16_t n_flows = 1; //!< Number of concurrent flows competing in the channel.
    /*********************************
     * Tracing Configuration.
     *********************************/
    std::string prefix_file_name = "P2P-project"; //!< Prefix of the output trace file.
    bool pcap_tracing = false;                    //!< Enable or disable PCAP tracing.
    bool ascii_tracing = false;                   //!< Enable or disable ASCII tracing.
};

std::ostream& operator<<(std::ostream& os, const Configuration& conf);

void ParseConsoleArgs(Configuration& conf, int argc, char* argv[]);
void InitializeDefaultConfiguration(const Configuration& configuration);

#endif // P2P_SIMULATION_CONFIGURATION_H