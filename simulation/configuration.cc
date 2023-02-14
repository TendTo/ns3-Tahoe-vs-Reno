#include "configuration.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("P2P-Project-Configuration");

static uint32_t
GetTcpSegmentSize(const Configuration& conf)
{
    Header* temp_header = new Ipv4Header();
    uint32_t ip_header = temp_header->GetSerializedSize();
    NS_LOG_LOGIC("IP Header size is: " << ip_header);
    delete temp_header;
    temp_header = new TcpHeader();
    uint32_t tcp_header = temp_header->GetSerializedSize();
    NS_LOG_LOGIC("TCP Header size is: " << tcp_header);
    delete temp_header;
    uint32_t tcp_adu_size = conf.mtu_bytes - 20 - (ip_header + tcp_header);
    NS_LOG_LOGIC("TCP ADU size is: " << tcp_adu_size);
    return tcp_adu_size;
}

static void
SetTcpVariant(const Configuration& conf)
{
    // Set the tcp variant to use
    TypeIdValue tcp_variant, recovery;
    if (conf.transport_prot.compare("TcpTahoe") == 0)
    {
        tcp_variant = TypeIdValue(TcpTahoe::GetTypeId());
        recovery = TypeIdValue(TypeId::LookupByName("ns3::TcpClassicRecovery"));
    }
    else if (conf.transport_prot.compare("TcpReno") == 0)
    {
        // tcp_variant = TypeIdValue(TcpReno::GetTypeId());
        recovery = TypeIdValue(TypeId::LookupByName("ns3::TcpClassicRecovery"));
    }
    else
    {
        NS_LOG_ERROR("Invalid transport protocol: " << conf.transport_prot << std::endl
                                                    << "Expected: TcpTahoe, TcpReno");
        exit(1);
    }
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", tcp_variant);
    Config::SetDefault("ns3::TcpL4Protocol::RecoveryType", recovery);
}

static void
SetTcpAttributes(const Configuration& conf)
{
    // The maximum transmit buffer size
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(conf.snd_buf_size));
    // The maximum receive buffer size
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(conf.rcv_buf_size));
    // The maximum segment size
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(conf.adu_bytes));
    // The initial congestion window
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(conf.initial_cwnd));
    // The initial slow start threshold
    Config::SetDefault("ns3::TcpSocket::InitialSlowStartThreshold",
                       UintegerValue(conf.initial_ssthresh));
    // Disable Nagle's algorithm
    Config::SetDefault("ns3::TcpSocket::TcpNoDelay", BooleanValue(conf.disable_nagle));
}

static void
SetQueueAttributes(const Configuration& conf)
{
    // The maximum number of packets that can be queued
    Config::SetDefault("ns3::RedQueueDisc::MaxSize",
                       StringValue(std::to_string(conf.tcp_queue_size) + "p"));
}

std::ostream&
operator<<(std::ostream& os, const Configuration& conf)
{
    return os << "Configuration: {" << std::endl
              << "\tTransport protocol: " << conf.transport_prot << std::endl
              << "\tError probability: " << conf.error_p << std::endl
              << "\tSender bandwidth: " << conf.s_bandwidth << std::endl
              << "\tSender delay " << conf.s_delay << std::endl
              << "\tReceiver bandwidth: " << conf.r_bandwidth << std::endl
              << "\tReceiver delay: " << conf.r_delay << std::endl
              << "\tTracing: " << conf.ascii_tracing << std::endl
              << "\tPrefix file name: " << conf.prefix_file_name << std::endl
              << "\tMegabytes to send (MB): " << conf.max_mbytes_to_send << std::endl
              << "\tMTU (bytes): " << conf.mtu_bytes << std::endl
              << "\tNumber of flows: " << conf.n_flows << std::endl
              << "\tDuration (s): " << conf.duration << std::endl
              << "\tRun: " << conf.run << std::endl
              << "\tSack: " << conf.sack << std::endl
              << "\tPcap: " << conf.pcap_tracing << std::endl
              << "}" << std::endl;
}

void
ParseConsoleArgs(Configuration& conf, int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.AddValue("transport_prot",
                 "Transport protocol to use: TcpTahoe, TcpReno",
                 conf.transport_prot);
    cmd.AddValue("error_p", "Packet error rate", conf.error_p);
    cmd.AddValue("s_bandwidth", "Sender link bandwidth", conf.s_bandwidth);
    cmd.AddValue("s_delay", "Sender link delay", conf.s_delay);
    cmd.AddValue("r_bandwidth", "Receiver link bandwidth", conf.r_bandwidth);
    cmd.AddValue("r_delay", "Receiver link delay", conf.r_delay);
    cmd.AddValue("prefix_name", "Prefix of output trace file", conf.prefix_file_name);
    cmd.AddValue("data", "Max number of Megabytes of data to transmit", conf.max_mbytes_to_send);
    cmd.AddValue("mtu", "Size of IP packets to send (bytes)", conf.mtu_bytes);
    cmd.AddValue("num_flows", "Number of flows", conf.n_flows);
    cmd.AddValue("duration", "Duration of the simulation (sec)", conf.duration);
    cmd.AddValue("run", "Run index (for setting repeatable seeds)", conf.run);
    cmd.AddValue("pcap_tracing", "Enable or disable PCAP tracing", conf.pcap_tracing);
    cmd.AddValue("ascii_tracing", "Flag to enable/disable tracing", conf.ascii_tracing);
    cmd.AddValue("sack", "Enable or disable SACK option", conf.sack);
    cmd.AddValue("device_queue_size", "Size of the device queue", conf.device_queue_size);
    cmd.AddValue("tcp_queue_size", "Size of the tcp queue", conf.tcp_queue_size);
    cmd.Parse(argc, argv);

    conf.adu_bytes = GetTcpSegmentSize(conf);

    NS_LOG_INFO(conf);
}

void
InitializeDefaultConfiguration(const Configuration& conf)
{
    // Set the random generation. If the run id is the same, the randomness will be the same
    SeedManager::SetSeed(1);
    SeedManager::SetRun(conf.run);

    SetTcpVariant(conf);
    SetTcpAttributes(conf);
    SetQueueAttributes(conf);
}
