#include "node.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("P2P-Project-Node");

static void
ScheduleTracing(const ApplicationContainer& apps, Tracer& tracer)
{
    tracer.ScheduleTracing(apps);
}

void
InitializeNode(const Configuration& conf, Tracer& tracer)
{
    // Explicitly create the nodes required by the topology.
    NS_LOG_INFO("Create nodes");
    NodeContainer nodes;
    nodes.Create(2);

    // Explicitly create the point-to-point link required by the topology.
    NS_LOG_INFO("Create channel");
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(conf.access_bandwidth));
    pointToPoint.SetChannelAttribute("Delay", StringValue(conf.access_delay));

    // Install the network devices on the nodes so they can use the PointToPoint channel.
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // Install the internet stack on the nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // We've got the "hardware" in place.  Now we need to add IP addresses.
    NS_LOG_INFO("Assign IP Addresses");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign(devices);

    // Create a BulkSendApplication and install it on node 0
    NS_LOG_INFO("Create Applications");
    uint16_t port = 9; // well-known echo port number
    BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(i.GetAddress(1), port));
    source.SetAttribute("MaxBytes", UintegerValue(conf.mbytes_to_send * 1000000));
    // source.SetAttribute("SendSize", UintegerValue(conf.mbytes_to_send));
    ApplicationContainer sourceApps = source.Install(nodes.Get(0));
    sourceApps.Start(Seconds(0));
    sourceApps.Stop(Seconds(conf.duration));

    // Create a PacketSinkApplication and install it on node 1
    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(nodes.Get(1));
    sinkApps.Start(Seconds(0));
    sinkApps.Stop(Seconds(conf.duration));

    // Set up tracing if enabled
    if (conf.ascii_tracing)
    {
        AsciiTraceHelper ascii;
        pointToPoint.EnableAsciiAll(ascii.CreateFileStream(conf.prefix_file_name + ".tr"));
    }
    if (conf.pcap_tracing)
        pointToPoint.EnablePcapAll(conf.prefix_file_name, false);

    Simulator::Schedule(Seconds(0.01), &ScheduleTracing, sourceApps, tracer);
    Simulator::ScheduleDestroy(MakeCallback(&Tracer::PrintGraphData, &tracer));
}