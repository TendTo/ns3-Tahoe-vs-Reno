#include "tracer.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Tracer");

Tracer::Tracer(const Configuration& config)
    : m_config(config),
      m_updateType(GraphDataUpdateType::Cwnd)
{
}

Tracer::Tracer(const Configuration& conf, const GraphDataUpdateType updateType)
    : m_config(conf),
      m_updateType(updateType)
{
}

void
Tracer::ScheduleTracing()
{
    NS_LOG_FUNCTION(this);

    Config::Connect("/NodeList/*/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
                    MakeCallback(&Tracer::CwndTracer, this));
    Config::Connect("/NodeList/*/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold",
                    MakeCallback(&Tracer::SsThreshTracer, this));

    NS_LOG_INFO("Tracing scheduled");
}

const std::map<uint32_t, std::vector<SenderGraphData>>&
Tracer::GetSenderGraphData() const
{
    return m_senderGraphData;
}

const std::vector<ReceiverGraphData>&
Tracer::GetReceiverGraphData() const
{
    return m_receiverGraphData;
}

void
Tracer::CwndTracer(std::string ctx, uint32_t oldval, uint32_t newval)
{
    NS_LOG_FUNCTION(this << ctx << oldval << newval);

    uint32_t nodeId = GetNodeIdFromContext(ctx);
    m_cwndMap[nodeId] = newval;
    NS_LOG_DEBUG("Node: " << nodeId << " Cwnd: " << newval);

    if (m_updateType == GraphDataUpdateType::Cwnd)
        UpdateGraphData(nodeId);
}

void
Tracer::SsThreshTracer(std::string ctx, uint32_t oldval, uint32_t newval)
{
    NS_LOG_FUNCTION(this << ctx << oldval << newval);

    uint32_t nodeId = GetNodeIdFromContext(ctx);
    m_ssThreshMap[nodeId] = newval;
    NS_LOG_DEBUG("Node: " << nodeId << " SsThresh: " << newval);

    if (m_updateType == GraphDataUpdateType::SsThresh)
        UpdateGraphData(nodeId);
}

void
Tracer::TcpQueueTracer(uint32_t oldval, uint32_t newval)
{
    NS_LOG_FUNCTION(this << oldval << newval);

    ReceiverGraphData graphData = {static_cast<uint32_t>(Simulator::Now().GetMilliSeconds()),
                                   newval};
    m_receiverGraphData.push_back(graphData);

    NS_LOG_DEBUG("Time: " << graphData.time << " TcpQueueSize: " << graphData.tcpQueueSize);
}

uint32_t
Tracer::GetNodeIdFromContext(std::string context)
{
    NS_LOG_FUNCTION(context);

    std::size_t const n1 = context.find_first_of('/', 1);
    std::size_t const n2 = context.find_first_of('/', n1 + 1);
    return std::stoul(context.substr(n1 + 1, n2 - n1 - 1));
}

void
Tracer::UpdateGraphData(uint32_t nodeId)
{
    NS_LOG_FUNCTION(this << nodeId);
    SenderGraphData graphData = {
        static_cast<uint32_t>(Simulator::Now().GetMilliSeconds()),
        m_cwndMap.count(nodeId) == 0 ? m_config.initial_cwnd : m_cwndMap.at(nodeId),
        m_ssThreshMap.count(nodeId) == 0 ? m_config.initial_ssthresh : m_ssThreshMap.at(nodeId)};
    m_senderGraphData[nodeId].push_back(graphData);

    NS_LOG_DEBUG("Node: " << nodeId << " Time: " << graphData.time << " Cwnd: " << graphData.cwnd
                          << " SsThresh: " << graphData.ssthresh);
}

void
Tracer::PrintGraphData() const
{
    std::cout << "============= Results =============" << std::endl;
    for (const auto& [nodeId, graphDataVector] : m_senderGraphData)
    {
        std::cout << "Node: " << nodeId << std::endl;
        for (const auto& [time, cwnd, ssthresh] : graphDataVector)
        {
            std::cout << "\tTime: " << time << " Cwnd: " << cwnd / m_config.adu_bytes
                      << " SsThresh: " << ssthresh / m_config.adu_bytes << std::endl;
        }
    }
    std::cout << "===================================" << std::endl;
}

void
Tracer::PrintGraphDataToFile() const
{
    Gnuplot plot(m_config.prefix_file_name + ".png");
    plot.SetTitle("TCP Congestion Window");
    plot.SetTerminal("png");
    plot.SetLegend("Time (ms)", "Congestion Window (segments)");

    for (const auto& [nodeId, graphDataVector] : m_senderGraphData)
    {
        // Skip nodes that have no data or the sink node, that has only one data point
        if (graphDataVector.empty() || graphDataVector.size() == 1)
            continue;

        Gnuplot2dDataset cwndDataset;
        cwndDataset.SetTitle("Node " + std::to_string(nodeId) + " Cwnd");
        Gnuplot2dDataset ssthreshDataset;
        ssthreshDataset.SetTitle("Node " + std::to_string(nodeId) + " SsThresh");
        for (const auto& [time, cwnd, ssthresh] : graphDataVector)
        {
            cwndDataset.Add(time, cwnd / m_config.adu_bytes);
            ssthreshDataset.Add(time, ssthresh / m_config.adu_bytes);
        }
        plot.AddDataset(cwndDataset);
        plot.AddDataset(ssthreshDataset);
    }

    if (!m_receiverGraphData.empty())
    {
        Gnuplot2dDataset receiverDataset;
        receiverDataset.SetTitle("Queue Size");
        for (const auto& [time, queueSize] : m_receiverGraphData)
        {
            receiverDataset.Add(time, queueSize);
        }
        plot.AddDataset(receiverDataset);
    }

    std::ofstream plotFile(m_config.prefix_file_name + ".plt");
    plot.GenerateOutput(plotFile);
    plotFile.close();
}
