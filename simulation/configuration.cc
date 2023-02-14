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
    Config::SetDefault("ns3::TcpSocket::TcpNoDelay", BooleanValue(!conf.nagle));
    // Enable SACK
    Config::SetDefault("ns3::TcpSocketBase::Sack", BooleanValue(conf.sack));
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
    cmd.AddValue("s_buf_size", "Sender buffer size (bytes)", conf.snd_buf_size);
    cmd.AddValue("r_buf_size", "Receiver buffer size (bytes)", conf.rcv_buf_size);
    cmd.AddValue("cwnd", "Initial congestion window (segments)", conf.initial_cwnd);
    cmd.AddValue("ssthresh", "Initial slow start threshold (segments)", conf.initial_ssthresh);
    cmd.AddValue("mtu", "Size of IP packets to send (bytes)", conf.mtu_bytes);
    cmd.AddValue("sack", "Enable SACK", conf.sack);
    cmd.AddValue("nagle", "Enable Nagle's algorithm", conf.nagle);
    cmd.AddValue("error_p", "Packet error rate", conf.error_p);
    cmd.AddValue("s_bandwidth", "Sender link bandwidth", conf.s_bandwidth);
    cmd.AddValue("s_delay", "Sender link delay", conf.s_delay);
    cmd.AddValue("r_bandwidth", "Receiver link bandwidth", conf.r_bandwidth);
    cmd.AddValue("r_delay", "Receiver link delay", conf.r_delay);
    cmd.AddValue("tcp_queue_size", "TCP queue size (packets)", conf.tcp_queue_size);
    cmd.AddValue("run", "Run id", conf.run);
    cmd.AddValue("n_flows", "Number of flows", conf.n_flows);
    cmd.AddValue("duration", "Duration of the simulation (s)", conf.duration);
    cmd.AddValue("max_mbytes_to_send",
                 "Maximum number of megabytes to send (MB)",
                 conf.max_mbytes_to_send);
    cmd.AddValue("prefix_file_name", "Prefix file name", conf.prefix_file_name);
    cmd.AddValue("ascii_tracing", "Enable ASCII tracing", conf.ascii_tracing);
    cmd.AddValue("pcap_tracing", "Enable Pcap tracing", conf.pcap_tracing);
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
