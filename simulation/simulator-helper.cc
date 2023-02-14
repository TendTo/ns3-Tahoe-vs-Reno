#include "simulator-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("P2P-Project-SimulatorHelper");

static void
DevicePacketsInQueueTrace(std::string ctx, uint32_t oldval, uint32_t newval)
{
    if (newval == 0 || oldval == 0)
        return;
    NS_LOG_UNCOND("DeviceQueue:\t" << ctx << "\t" << oldval << "\t" << newval);
}

static void
TcPacketsInQueueTrace(std::string ctx, uint32_t oldval, uint32_t newval)
{
    if (newval == 0 || oldval == 0)
        return;
    NS_LOG_UNCOND("TcpQueue:\t" << ctx << "\t" << oldval << "\t" << newval);
}

SimulatorHelper::SimulatorHelper(const Configuration& conf)
    : m_port(9),
      m_conf(conf),
      m_isInitialized(false)
{
    m_ipv4Helper.SetBase("10.0.1.0", "255.255.255.0");
}

void
SimulatorHelper::Setup()
{
    SetupNodes();
    SetupSenderChannel();
    SetupReceiverChannel();
    SetupRouterQueue();

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
    m_senders.Create(m_conf.n_flows);
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
    for (uint32_t i = 0; i < m_conf.n_flows; i++)
    {
        NetDeviceContainer devices = m_s_pointToPoint.Install(m_senders.Get(i), m_gateway.Get(0));
        m_ipv4Helper.NewNetwork();
        m_ipv4Helper.Assign(devices);
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
    // m_r_pointToPoint.SetQueue("ns3::DropTailQueue",
    //                           "MaxSize",
    //                           StringValue(std::to_string(m_conf.device_queue_size) + "p"));
    NetDeviceContainer devices = m_r_pointToPoint.Install(m_gateway.Get(0), m_receivers.Get(0));
    m_ipv4Helper.NewNetwork();
    m_ipv4Helper.Assign(devices);

    // Ptr<PointToPointNetDevice> ptpnd = DynamicCast<PointToPointNetDevice>(device.Get(1));
    // Ptr<Queue<Packet>> queue = ptpnd->GetQueue();
    // queue->TraceConnect("PacketsInQueue", "Receiver", MakeCallback(&DevicePacketsInQueueTrace));
    // Ptr<PointToPointNetDevice> ptpnd2 = DynamicCast<PointToPointNetDevice>(device.Get(0));
    // Ptr<Queue<Packet>> queue2 = ptpnd2->GetQueue();
    // queue2->TraceConnect("PacketsInQueue", "Gateway", MakeCallback(&DevicePacketsInQueueTrace));

    TrafficControlHelper tch;
    tch.SetRootQueueDisc("ns3::RedQueueDisc");
    tch.Uninstall(devices);
    QueueDiscContainer qDiscs = tch.Install(devices);
    qDiscs.Get(1)->TraceConnect("PacketsInQueue", "1", MakeCallback(&TcPacketsInQueueTrace));
    qDiscs.Get(0)->TraceConnect("PacketsInQueue", "0", MakeCallback(&TcPacketsInQueueTrace));
}

void
SimulatorHelper::SetupRouterQueue()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Create router queue");

    // TrafficControlHelper tch;
    // tch.SetRootQueueDisc("ns3::RedQueueDisc");
    // uint32_t lastDeviceId = m_receivers.Get(0)->GetNDevices() - 1;
    // tch.Uninstall(m_receivers.Get(0)->GetDevice(lastDeviceId));
    // QueueDiscContainer qDiscs = tch.Install(m_receivers.Get(0)->GetDevice(lastDeviceId));
    // qDiscs.Get(0)->TraceConnectWithoutContext("PacketsInQueue",
    //                                           MakeCallback(&TcPacketsInQueueTrace));
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
