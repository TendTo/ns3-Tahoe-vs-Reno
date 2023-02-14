#ifndef P2P_SIMULATION_NAIVE_LOSS_RECOVERY_H
#define P2P_SIMULATION_NAIVE_LOSS_RECOVERY_H

#include "tcp-tahoe.h"

#include "ns3/core-module.h"
#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/tcp-socket-base.h"

using namespace ns3;

class TcpTahoeLossRecovery : public TcpRecoveryOps
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Constructor
     */
    TcpTahoeLossRecovery();

    /**
     * \brief Copy constructor.
     * \param recovery object to copy.
     */
    TcpTahoeLossRecovery(const TcpTahoeLossRecovery& recovery);

    /**
     * \brief Constructor
     */
    ~TcpTahoeLossRecovery() override;

    std::string GetName() const override;

    void EnterRecovery(Ptr<TcpSocketState> tcb,
                       uint32_t dupAckCount,
                       uint32_t unAckDataCount,
                       uint32_t deliveredBytes) override;

    void DoRecovery(Ptr<TcpSocketState> tcb, uint32_t deliveredBytes) override;

    void ExitRecovery(Ptr<TcpSocketState> tcb) override;

    Ptr<TcpRecoveryOps> Fork() override;

  private:
    TcpTahoe m_tahoeCongestionControl;
};

#endif /* P2P_SIMULATION_NAIVE_LOSS_RECOVERY_H */