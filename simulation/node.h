#ifndef P2P_SIMULATION_NODE_H
#define P2P_SIMULATION_NODE_H

#include "configuration.h"
#include "tracer.h"

#include "ns3/application.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-helper.h"

void InitializeNode(const Configuration& conf);

#endif // P2P_SIMULATION_NODE_H