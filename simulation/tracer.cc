#include "tracer.h"

using namespace ns3;

Tracer::Tracer()
{
}

Tracer::Tracer(GraphDataUpdateType updateType)
{
    m_updateType = updateType;
}

void
Tracer::ScheduleTracing(const ApplicationContainer& apps)
{
    for (uint i = 0; i < apps.GetN(); i++)
    {
        Ptr<BulkSendApplication> senderApp = DynamicCast<BulkSendApplication>(apps.Get(i));
        std::string nodeId = std::to_string(senderApp->GetNode()->GetId());
        senderApp->GetSocket()->TraceConnect("CongestionWindow",
                                             nodeId,
                                             MakeCallback(&Tracer::CwndTracer, this));
        senderApp->GetSocket()->TraceConnect("SlowStartThreshold",
                                             nodeId,
                                             MakeCallback(&Tracer::SsThreshTracer, this));
    }
    std::cout << "Tracing scheduled!" << std::endl;
}

const std::map<uint32_t, std::vector<GraphData>>&
Tracer::GetGraphData() const
{
    return m_graphDataMap;
}

void
Tracer::CwndTracer(std::string ctx, uint32_t oldval, uint32_t newval)
{
    std::cout << "cwnd"
              << "\tctx: " << ctx << " oldval: " << oldval << " newval: " << newval << std::endl;
    uint32_t nodeId = GetNodeIdFromContext(ctx);
    m_cwndMap[nodeId] = newval;
    std::cout << "cwnd updated successfully!\t" << m_cwndMap[nodeId] << std::endl;
    if (m_updateType == GraphDataUpdateType::Cwnd)
        UpdateGraphData(nodeId);
}

void
Tracer::SsThreshTracer(std::string ctx, uint32_t oldval, uint32_t newval)
{
    std::cout << "ssthresh"
              << "\tctx: " << ctx << " oldval: " << oldval << " newval: " << newval << std::endl;
    uint32_t nodeId = GetNodeIdFromContext(ctx);
    m_ssThreshMap[nodeId] = newval;
    if (m_updateType == GraphDataUpdateType::SsThresh)
        UpdateGraphData(nodeId);
}

uint32_t
Tracer::GetNodeIdFromContext(std::string context)
{
    return std::stoul(context);
}

void
Tracer::UpdateGraphData(uint32_t nodeId)
{
    std::cout << "Updating graph data for node: " << nodeId << std::endl;
    GraphData graphData = {static_cast<uint32_t>(Simulator::Now().GetMilliSeconds()),
                           m_cwndMap.count(nodeId) == 0 ? 0 : m_cwndMap.at(nodeId),
                           m_ssThreshMap.count(nodeId) == 0 ? 0 : m_ssThreshMap.at(nodeId)};
    m_graphDataMap[nodeId].push_back(graphData);
}

void
Tracer::PrintGraphData() const
{
    std::cout << "FEST" << std::endl;
    for (const auto& [nodeId, graphDataVector] : m_graphDataMap)
    {
        for (const auto& [time, cwnd, ssthresh] : graphDataVector)
        {
            std::cout << "Node: " << nodeId << " Time: " << time << " Cwnd: " << cwnd
                      << " SsThresh: " << ssthresh << std::endl;
        }
    }
}
