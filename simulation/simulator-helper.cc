#include "simulator-helper.h"

NS_LOG_COMPONENT_DEFINE("SimulatorHelper");

SimulatorHelper::SimulatorHelper(const Configuration& conf, Tracer& tracer)
    : m_port(9),
      m_conf(conf),
      m_isInitialized(false),
      m_tracer(tracer)
{
    m_ipv4Helper.SetBase("10.0.1.0", "255.255.255.0");
}

void
SimulatorHelper::Setup()
{
    SetupNodes();
    SetupSenderChannel();
    SetupReceiverChannel();

    NS_LOG_INFO("Initialize Global Routing.");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    SetupSenderApplications();
    SetupReceiverApplications();
    SetupTracing();

    m_isInitialized = true;
}

void
SimulatorHelper::Run()
{
    NS_LOG_FUNCTION(this);

    if (!m_isInitialized)
    {
        NS_LOG_WARN("SimulatorHelper is not initialized");
        return;
    }

    NS_LOG_INFO("Running simulation");
    Simulator::Stop(Seconds(m_conf.duration));
    Simulator::Run();
    Simulator::Destroy();
}

void
SimulatorHelper::SetupNodes()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Create nodes");
    m_senders.Create(m_conf.n_tcp_tahoe + m_conf.n_tcp_reno);
    m_receivers.Create(1);
    m_gateway.Create(1);

    InternetStackHelper internet;
    internet.InstallAll();
}

void
SimulatorHelper::SetupSenderChannel()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Create sender channel");
    m_s_pointToPoint.SetDeviceAttribute("DataRate", StringValue(m_conf.s_bandwidth));
    m_s_pointToPoint.SetChannelAttribute("Delay", StringValue(m_conf.s_delay));
    for (uint32_t i = 0; i < m_senders.GetN(); i++)
    {
        NetDeviceContainer devices = m_s_pointToPoint.Install(m_senders.Get(i), m_gateway.Get(0));
        m_ipv4Helper.NewNetwork();
        m_ipv4Helper.Assign(devices);
    }

    for (uint32_t i = 0; i < m_conf.n_tcp_tahoe; i++)
    {
        Config::Set("/NodeList/" + std::to_string(i) + "/$ns3::TcpL4Protocol/SocketType",
                    TypeIdValue(TcpTahoe::GetTypeId()));
        Config::Set("/NodeList/" + std::to_string(i) + "/$ns3::TcpL4Protocol/RecoveryType",
                    TypeIdValue(TcpTahoeLossRecovery::GetTypeId()));
    }
    for (uint32_t i = m_conf.n_tcp_tahoe; i < m_conf.n_tcp_tahoe + m_conf.n_tcp_reno; i++)
    {
        Config::Set("/NodeList/" + std::to_string(i) + "/$ns3::TcpL4Protocol/SocketType",
                    TypeIdValue(TypeId::LookupByName("ns3::TcpLinuxReno")));
        Config::Set("/NodeList/" + std::to_string(i) + "/$ns3::TcpL4Protocol/RecoveryType",
                    TypeIdValue(TypeId::LookupByName("ns3::TcpClassicRecovery")));
    }
}

void
SimulatorHelper::SetupReceiverChannel()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("Create error model");
    Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
    Ptr<RateErrorModel> error_model = CreateObject<RateErrorModel>();
    error_model->SetRandomVariable(uv);
    error_model->SetUnit(RateErrorModel::ERROR_UNIT_PACKET);
    error_model->SetRate(m_conf.error_p);

    NS_LOG_INFO("Create receiver channel");
    m_r_pointToPoint.SetDeviceAttribute("DataRate", StringValue(m_conf.r_bandwidth));
    m_r_pointToPoint.SetChannelAttribute("Delay", StringValue(m_conf.r_delay));
    m_r_pointToPoint.SetDeviceAttribute("ReceiveErrorModel", PointerValue(error_model));

    NetDeviceContainer devices = m_r_pointToPoint.Install(m_gateway.Get(0), m_receivers.Get(0));
    m_ipv4Helper.NewNetwork();
    m_ipv4Helper.Assign(devices);

    TrafficControlHelper tch;
    tch.SetRootQueueDisc("ns3::RedQueueDisc");
    tch.Uninstall(devices);
    QueueDiscContainer qDiscs = tch.Install(devices);
    qDiscs.Get(0)->TraceConnectWithoutContext("PacketsInQueue",
                                              MakeCallback(&Tracer::TcpQueueTracer, &m_tracer));
}

void
SimulatorHelper::SetupSenderApplications()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Create sender applications");
    Ipv4Address remoteAddress = m_receivers.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(remoteAddress, m_port));
    source.SetAttribute("SendSize", UintegerValue(m_conf.adu_bytes));
    source.SetAttribute("MaxBytes", UintegerValue(m_conf.max_mbytes_to_send * 1000000));
    source.SetAttribute("StartTime", TimeValue(Seconds(0)));
    source.SetAttribute("StopTime", TimeValue(Seconds(m_conf.duration)));

    ApplicationContainer sourceApps = source.Install(m_senders);
}

void
SimulatorHelper::SetupReceiverApplications()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Create receiver applications");
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                          InetSocketAddress(Ipv4Address::GetAny(), m_port));
    sink.SetAttribute("StartTime", TimeValue(Seconds(0)));
    sink.SetAttribute("StopTime", TimeValue(Seconds(m_conf.duration)));
    ApplicationContainer sinkApps = sink.Install(m_receivers);
}

void
SimulatorHelper::SetupTracing()
{
    Simulator::Schedule(NanoSeconds(1), MakeCallback(&Tracer::ScheduleTracing, &m_tracer));
    Simulator::ScheduleDestroy(MakeCallback(&Tracer::PrintGraphDataToFile, &m_tracer));

    // Set up tracing if enabled
    if (m_conf.ascii_tracing)
    {
        AsciiTraceHelper ascii;
        m_s_pointToPoint.EnableAsciiAll(ascii.CreateFileStream(m_conf.prefix_file_name + ".tr"));
        m_r_pointToPoint.EnableAsciiAll(ascii.CreateFileStream(m_conf.prefix_file_name + ".tr"));
    }
    if (m_conf.pcap_tracing)
    {
        m_s_pointToPoint.EnablePcapAll(m_conf.prefix_file_name, false);
        m_r_pointToPoint.EnablePcapAll(m_conf.prefix_file_name, false);
    }
}
