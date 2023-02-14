#ifndef P2P_SIMULATION_TRACER_H
#define P2P_SIMULATION_TRACER_H

#include "configuration.h"

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/gnuplot.h"
#include "ns3/socket.h"

using namespace ns3;

/**
 * @brief It specifies when the tracer should update the graph data.
 * A new point in the graph is added when the specified update type occurs.
 * The flags can be combined to specify multiple update types.
 */
enum GraphDataUpdateType
{
    None = 1 << 0,
    Cwnd = 1 << 1,
    SsThresh = 1 << 2,
    QueueSize = 1 << 3,
    All = (1 << 4) - 1
};

/**
 * @brief Allows to combine GraphDataUpdateType flags using the bitwise OR operator.
 * @param lhs left hand side GraphDataUpdateType
 * @param rhs right hand side GraphDataUpdateType
 * @return combined GraphDataUpdateType
 */
inline GraphDataUpdateType operator|(const GraphDataUpdateType& lhs,
                                     const GraphDataUpdateType& rhs);

/**
 * @brief Point in the graph for a sender node.
 * It contains the congestion window and the slow start threshold of the node at the given time.
 */
struct SenderGraphData
{
    uint32_t time;
    uint32_t cwnd;
    uint32_t ssthresh;
};

/**
 * @brief Point in the graph for the receiver node.
 * It contains the queue size of the node at the given time.
 */
struct ReceiverGraphData
{
    uint32_t time;
    uint32_t tcpQueueSize;
};

/**
 * @brief Tracer class.
 * It is used to trace the simulation and aggregate the data.
 * The data will later be used to create the graphs.
 * It uses the ns3 callback system to spy on the trace sources.
 */
class Tracer
{
  public:
    /**
     * @brief Tracer constructor.
     * @param conf simulation configuration.
     */
    Tracer(const Configuration& conf);
    /**
     * @brief Trace constructor.
     * @param conf simulation configuration.
     * @param updateType type of update the tracer will use to aggregate the data.
     */
    Tracer(const Configuration& conf, const GraphDataUpdateType updateType);

    /**
     * @brief Sender graph data getter.
     * @return sender graph data.
     */
    const std::map<uint32_t, std::vector<SenderGraphData>>& GetSenderGraphData() const;
    /**
     * @brief Receiver graph data getter.
     * @return receiver graph data.
     */
    const std::vector<ReceiverGraphData>& GetReceiverGraphData() const;

    /**
     * @brief Schedule the tracing.
     * Since some resources are not created at the beginning of the simulation,
     * the tracing must be scheduled to be attached after the simulation has started.
     */
    void ScheduleTracing();
    /**
     * @brief Trace the congestion window.
     * @param ctx id of the node.
     * @param oldval old congestion window value.
     * @param newval new congestion window value.
     */
    void CwndTracer(std::string ctx, uint32_t oldval, uint32_t newval);
    /**
     * @brief Trace the slow start threshold.
     * @param ctx id of the node.
     * @param oldval old slow start threshold value.
     * @param newval new slow start threshold value.
     */
    void SsThreshTracer(std::string ctx, uint32_t oldval, uint32_t newval);
    /**
     * @brief Trace the queue size.
     * @param oldval old queue size.
     * @param newval new queue size.
     */
    void TcpQueueTracer(uint32_t oldval, uint32_t newval);
    /**
     * @brief Print the aggregated data to the console.
     */
    void PrintGraphData() const;
    /**
     * @brief Print the aggregated data to a file.
     * The file can be later be processed by gnuplot to create a .png file.
     * `gnuplot <prefix_file_name>.plot`
     */
    void PrintGraphDataToFile() const;

  protected:
    /**
     * @brief Extract the node id from the context string.
     * @param ctx path of the trace source.
     * @return id of the node.
     */
    static uint32_t GetNodeIdFromContext(std::string ctx);
    /**
     * @brief Update the graph data, by adding a new point to the graph.
     * @param nodeId id of the node the data belongs to.
     */
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