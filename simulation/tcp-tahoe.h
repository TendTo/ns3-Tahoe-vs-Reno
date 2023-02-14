/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Adrian Sai-wah Tam
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
 * Author: Adrian Sai-wah Tam <adrian.sw.tam@gmail.com>
 */

#ifndef TCP_TAHOE_H
#define TCP_TAHOE_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-socket-state.h"

/**
 * \ingroup congestionOps
 *
 * \brief Tahoe congestion control algorithm
 *
 * This class implement the Tahoe congestion control type
 * and it mimics the first TCP variant with in-built congestion control algorithms.
 */
class TcpTahoe : public ns3::TcpCongestionOps
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static ns3::TypeId GetTypeId(void);
    /**
     * Create an unbound tcp socket.
     */
    TcpTahoe();
    /**
     * \brief Copy constructor
     * \param sock the object to copy
     */
    TcpTahoe(const TcpTahoe& sock);

    std::string GetName() const override;

    void IncreaseWindow(ns3::Ptr<ns3::TcpSocketState> tcb, uint32_t segmentsAcked) override;
    uint32_t GetSsThresh(ns3::Ptr<const ns3::TcpSocketState> tcb, uint32_t bytesInFlight) override;
    ns3::Ptr<ns3::TcpCongestionOps> Fork() override;

  protected:
    /**
     * Slow start phase handler
     * \param tcb Transmission Control Block of the connection
     * \param segmentsAcked count of segments acked
     * \return Number of segments acked minus the difference between the receiver and sender
     * Cwnd
     */
    virtual void SlowStart(ns3::Ptr<ns3::TcpSocketState> tcb, uint32_t segmentsAcked);
    virtual void CongestionAvoidance(ns3::Ptr<ns3::TcpSocketState> tcb, uint32_t segmentsAcked);

  private:
    uint32_t m_cWndCnt = 0; //!< Counter for the number of segments acked
};

#endif /* TCP_TAHOE_H */
