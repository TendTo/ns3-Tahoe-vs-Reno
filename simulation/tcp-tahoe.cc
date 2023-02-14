#include "tcp-tahoe.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("TcpTahoe");
NS_OBJECT_ENSURE_REGISTERED(TcpTahoe);

TcpTahoe::TcpTahoe()
    : TcpCongestionOps()
{
}

TcpTahoe::TcpTahoe(const TcpTahoe& socket)
    : TcpCongestionOps(socket)
{
}

ns3::TypeId
TcpTahoe::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpTahoe")
                            .SetParent<TcpCongestionOps>()
                            .SetGroupName("Internet")
                            .AddConstructor<TcpTahoe>();
    return tid;
}

std::string
TcpTahoe::GetName() const
{
    return "TcpTahoe";
}

void
TcpTahoe::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (tcb->m_cWnd < tcb->m_ssThresh) // The cwnd is less than the ssThresh => slow start
    {
        NS_LOG_DEBUG("In slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh);
        SlowStart(tcb, segmentsAcked);
    }
    else // The cwnd is greater than or equal to the ssThresh => congestion avoidance
    {
        NS_LOG_DEBUG("In cong. avoidance, m_cWnd " << tcb->m_cWnd << " m_ssThresh "
                                                   << tcb->m_ssThresh);
        CongestionAvoidance(tcb, segmentsAcked);
    }
}

uint32_t
TcpTahoe::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this << tcb << bytesInFlight);

    return std::max<uint32_t>(2 * tcb->m_segmentSize, tcb->m_cWnd / 2);
}

ns3::Ptr<TcpCongestionOps>
TcpTahoe::Fork()
{
    return CopyObject<TcpTahoe>(this);
}

void
TcpTahoe::SlowStart(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (segmentsAcked >= 1)
    {
        uint32_t sndCwnd = tcb->m_cWnd;
        // Set the cwnd to the minimum of the following:
        // 1. The current cwnd + the number of segments acked * segment size
        // 2. The ssThresh
        tcb->m_cWnd =
            std::min((sndCwnd + (segmentsAcked * tcb->m_segmentSize)), (uint32_t)tcb->m_ssThresh);
        NS_LOG_INFO("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh "
                                                     << tcb->m_ssThresh);
    }
}

void
TcpTahoe::CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    // Number of packets in the cwnd
    uint32_t w = tcb->m_cWnd / tcb->m_segmentSize;

    // Floor w to 1 if w == 0
    if (w == 0)
        w = 1;

    NS_LOG_DEBUG("w in segments " << w << " m_cWndCnt " << m_cWndCnt << " segments acked "
                                  << segmentsAcked);
    // Increase m_cWndCnt by the number of segments acked
    m_cWndCnt += segmentsAcked;

    if (m_cWndCnt >= w)
    {
        // Floor value of (segments_acked / segments in cwnd)
        uint32_t delta = m_cWndCnt / w;
        // Increase the cwnd by the (segments acked * segment size / segments in cwnd)
        tcb->m_cWnd += delta * tcb->m_segmentSize;
        // Remove the number of segments used to increase cwnd from m_cWndCnt
        m_cWndCnt -= delta * w;
        NS_LOG_DEBUG("Subtracting delta * w from m_cWndCnt " << delta * w);
    }
    NS_LOG_DEBUG("At end of CongestionAvoidance(), m_cWnd: " << tcb->m_cWnd
                                                             << " m_cWndCnt: " << m_cWndCnt);
}
