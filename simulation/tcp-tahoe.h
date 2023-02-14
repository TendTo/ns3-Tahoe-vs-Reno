#ifndef TCP_TAHOE_H
#define TCP_TAHOE_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-socket-state.h"

using namespace ns3;

/**
 * @brief TcpTahoe class.
 * Tahoe congestion control algorithm.
 * This class implement the Tahoe congestion control type and it mimics the first TCP variant with
 * in-built congestion control algorithms.
 */
class TcpTahoe : public TcpCongestionOps
{
  public:
    /**
     * @brief Get the type ID.
     * @return the object TypeId.
     */
    static TypeId GetTypeId(void);
    /**
     * @brief Create an unbound tcp socket.
     */
    TcpTahoe();
    /**
     * @brief Copy constructor.
     * @param sock the object to copy.
     */
    TcpTahoe(const TcpTahoe& sock);
    /**
     * @brief Get the name of the class.
     * @return name of the class.
     */
    std::string GetName() const override;

    /**
     * @brief Congestion avoidance algorithm implementation.
     * Called each time the congestion window is supposed to be increased.
     * @param tcb internal congestion state.
     * @param segmentsAcked count of segments acked.
     */
    void IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) override;
    /**
     * @brief Get the slow start threshold after a loss event.
     * @param tcb internal congestion state.
     * @param bytesInFlight total bytes in flight.
     * @return ssThresh value.
     */
    uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) override;
    /**
     * @brief Copy the congestion control algorithm across sockets.
     * @return a pointer of the copied object.
     */
    Ptr<TcpCongestionOps> Fork() override;

  protected:
    /**
     * @brief Slow start phase handler.
     * Each time the method is invoked, the congestion window is increased by (segmentAcked *
     * segmentSize).
     * @param tcb internal congestion state.
     * @param segmentsAcked count of segments acked.
     */
    void SlowStart(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
    /**
     * @brief Congestion avoidance phase handler.
     * Each time the method is invoked, the congestion window is increased by (segmentAcked *
     * segmentSize / congestion window).
     * It stores the number of segments acked if they are not enough to increase the congestion
     * window of a segment size.
     * @param tcb internal congestion state.
     * @param segmentsAcked count of segments acked.
     */
    void CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

  private:
    uint32_t m_cWndCnt = 0; //!< Counter for the number of segments acked
};

#endif /* TCP_TAHOE_H */
