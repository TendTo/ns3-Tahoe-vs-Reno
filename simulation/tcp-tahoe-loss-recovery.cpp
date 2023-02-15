#include "tcp-tahoe-loss-recovery.h"

NS_LOG_COMPONENT_DEFINE("TcpTahoeLossRecovery");
NS_OBJECT_ENSURE_REGISTERED(TcpTahoeLossRecovery);

TypeId
TcpTahoeLossRecovery::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpTahoeLossRecovery")
                            .SetParent<TcpRecoveryOps>()
                            .SetGroupName("Internet")
                            .AddConstructor<TcpTahoeLossRecovery>();
    return tid;
}

TcpTahoeLossRecovery::TcpTahoeLossRecovery()
    : TcpRecoveryOps()
{
    NS_LOG_FUNCTION(this);
}

TcpTahoeLossRecovery::TcpTahoeLossRecovery(const TcpTahoeLossRecovery& recovery)
    : TcpRecoveryOps(recovery)
{
    NS_LOG_FUNCTION(this);
}

TcpTahoeLossRecovery::~TcpTahoeLossRecovery()
{
    NS_LOG_FUNCTION(this);
}

std::string
TcpTahoeLossRecovery::GetName() const
{
    return "TcpTahoeLossRecovery";
}

void
TcpTahoeLossRecovery::EnterRecovery(Ptr<TcpSocketState> tcb,
                                    uint32_t dupAckCount,
                                    uint32_t unAckDataCount,
                                    uint32_t deliveredBytes)
{
    NS_LOG_FUNCTION(this << tcb << dupAckCount << unAckDataCount << deliveredBytes);

    NS_LOG_INFO("Entering recovery. LastAcked sq: " << tcb->m_lastAckedSeq);
    NS_LOG_LOGIC("Store the ssThresh set to half of cwnd. cwnd set to one. ssThresh set to zero "
                 "temporarily");
    // Store the halved ssThresh value
    m_half_ssThresh = tcb->m_cWnd / 2;
    // Set the ssThresh to zero, to avoid the sudden increase of cwnd at the end of the recovery
    tcb->m_ssThresh = 0;
    // Set the cwnd to one
    tcb->m_cWnd = 1;
    tcb->m_cWndInfl = 0;
}

void
TcpTahoeLossRecovery::DoRecovery(Ptr<TcpSocketState> tcb, uint32_t deliveredBytes)
{
    NS_LOG_FUNCTION(this << tcb << deliveredBytes);

    NS_LOG_INFO("Entering recovery. LastAcked sq: " << tcb->m_lastAckedSeq);
    NS_LOG_LOGIC("Apply tcp tahoe congestion control algorithm");
}

void
TcpTahoeLossRecovery::ExitRecovery(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    NS_LOG_INFO("Exiting recovery");
    // Restore the ssThresh value to the halved value
    tcb->m_ssThresh = m_half_ssThresh;
}

Ptr<TcpRecoveryOps>
TcpTahoeLossRecovery::Fork()
{
    return CopyObject<TcpTahoeLossRecovery>(this);
}
