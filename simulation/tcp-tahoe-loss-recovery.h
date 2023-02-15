#ifndef P2P_SIMULATION_TCP_TAHOE_LOSS_RECOVERY_H
#define P2P_SIMULATION_TCP_TAHOE_LOSS_RECOVERY_H

#include "tcp-tahoe.h"

#include "ns3/core-module.h"
#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/tcp-socket-base.h"

using namespace ns3;

/**
 * @brief TcpTahoeLossRecovery class.
 * It is used to implement the loss recovery part of the TCP Tahoe congestion control algorithm.
 * It doesn't use the fast recovery mechanism, present in newer TCP versions.
 * Instead, it brutally sets the ssthresh to half of the current cwnd and the cwnd to 1.
 */
class TcpTahoeLossRecovery : public TcpRecoveryOps
{
  public:
    /**
     * @brief Get the type ID.
     * @return the object TypeId.
     */
    static TypeId GetTypeId();

    /**
     * @brief Constructor.
     */
    TcpTahoeLossRecovery();

    /**
     * @brief Copy constructor.
     * @param recovery object to copy.
     */
    TcpTahoeLossRecovery(const TcpTahoeLossRecovery& recovery);

    /**
     * @brief Constructor.
     */
    ~TcpTahoeLossRecovery() override;

    /**
     * @brief Get the name of the class.
     * @return name of the class.
     */
    std::string GetName() const override;

    /**
     * @brief A packet has been lost, so the recovery mechanism is triggered.
     * @param tcb internal congestion state.
     * @param dupAckCount duplicate acknowledgement count.
     * @param unAckDataCount total bytes of data unacknowledged.
     * @param deliveredBytes bytes (S)ACKed in the last (S)ACK.
     */
    void EnterRecovery(Ptr<TcpSocketState> tcb,
                       uint32_t dupAckCount,
                       uint32_t unAckDataCount,
                       uint32_t deliveredBytes) override;
    /**
     * @brief Performs a normal Tcp Tahoe increment window procedure.
     * @param tcb internal congestion state.
     * @param deliveredBytes bytes (S)ACKed in the last (S)ACK.
     */
    void DoRecovery(Ptr<TcpSocketState> tcb, uint32_t deliveredBytes) override;
    /**
     * @brief Performs cwnd adjustments at the end of recovery.
     * The function is called when the TcpSocketState is changed from CA_RECOVERY.
     * @param tcb internal congestion state.
     */
    void ExitRecovery(Ptr<TcpSocketState> tcb) override;
    /**
     * @brief Copy the recovery algorithm across socket.
     * @return a pointer of the copied object.
     */
    Ptr<TcpRecoveryOps> Fork() override;

  private:
    uint32_t m_half_ssThresh;          //!< ssthresh value calculated by having the current cwnd.
    TcpTahoe m_tahoeCongestionControl; //!< The TCP Tahoe congestion control algorithm.
};

#endif /* P2P_SIMULATION_TCP_TAHOE_LOSS_RECOVERY_H */