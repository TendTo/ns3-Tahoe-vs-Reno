#ifndef P2P_SIMULATION_TRACER_H
#define P2P_SIMULATION_TRACER_H

#include "configuration.h"

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/gnuplot.h"
#include "ns3/socket.h"

using namespace ns3;

enum GraphDataUpdateType
{
    None,
    Cwnd,
    SsThresh
};

struct SenderGraphData
{
    uint32_t time;
    uint32_t cwnd;
    uint32_t ssthresh;
};

struct ReceiverGraphData
{
    uint32_t time;
    uint32_t tcpQueueSize;
};

class Tracer
{
  public:
    Tracer(const Configuration& conf);
    Tracer(const Configuration& conf, const GraphDataUpdateType updateType);

    const std::map<uint32_t, std::vector<SenderGraphData>>& GetSenderGraphData() const;
    const std::vector<ReceiverGraphData>& GetReceiverGraphData() const;

    void ScheduleTracing();
    void CwndTracer(std::string ctx, uint32_t oldval, uint32_t newval);
    void SsThreshTracer(std::string ctx, uint32_t oldval, uint32_t newval);
    void TcpQueueTracer(uint32_t oldval, uint32_t newval);
    void PrintGraphData() const;
    void PrintGraphDataToFile() const;

  protected:
    static uint32_t GetNodeIdFromContext(std::string ctx);
    void UpdateGraphData(uint32_t nodeId);

  private:
    const Configuration& m_config;              //!< Configuration
    const GraphDataUpdateType m_updateType;     //!< Aggregation type
    std::map<uint32_t, uint32_t> m_cwndMap;     //!< Congestion window outut
    std::map<uint32_t, uint32_t> m_ssThreshMap; //!< Slow start threshold outut
    std::map<uint32_t, std::vector<SenderGraphData>>
        m_senderGraphData;                              //!< Aggregated sender data outut
    std::vector<ReceiverGraphData> m_receiverGraphData; //!< Aggregated receiver data outut
};

#endif /* P2P_SIMULATION_TRACER_H */