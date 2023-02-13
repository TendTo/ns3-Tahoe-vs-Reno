/*
 * Copyright (c) 2019 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Apoorva Bhargava <apoorvabhargava13@gmail.com>
 *         Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 *
 */

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
    static ns3::TypeId tid = ns3::TypeId("ns3::TcpTahoe")
                                 .SetParent<ns3::TcpCongestionOps>()
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
TcpTahoe::IncreaseWindow(ns3::Ptr<ns3::TcpSocketState> tcb, uint32_t segmentsAcked)
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
TcpTahoe::GetSsThresh(ns3::Ptr<const ns3::TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this << tcb << bytesInFlight);

    // In Linux, it is written as:  return max(tp->snd_cwnd >> 1U, 2U);
    return std::max<uint32_t>(2 * tcb->m_segmentSize, tcb->m_cWnd / 2);
}

ns3::Ptr<ns3::TcpCongestionOps>
TcpTahoe::Fork()
{
    return ns3::CopyObject<TcpTahoe>(this);
}

void
TcpTahoe::SlowStart(ns3::Ptr<ns3::TcpSocketState> tcb, uint32_t segmentsAcked)
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
TcpTahoe::CongestionAvoidance(ns3::Ptr<ns3::TcpSocketState> tcb, uint32_t segmentsAcked)
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
