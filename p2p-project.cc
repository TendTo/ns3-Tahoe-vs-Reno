/*
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
 */

#include "simulation/configuration.h"
#include "simulation/simulator-helper.h"
#include "simulation/tcp-tahoe.h"
#include "simulation/tracer.h"

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("P2P-Project");

int
main(int argc, char* argv[])
{
    // Read the configuration from the command line and use it to initialize the default values
    Configuration conf;
    ParseConsoleArgs(conf, argc, argv);
    InitializeDefaultConfiguration(conf);

    // Set up tracing
    Tracer tracer(conf, GraphDataUpdateType::Cwnd);


    SimulatorHelper simHelper(conf, tracer);
    simHelper.Setup();

    NS_LOG_INFO("Run Simulation");
    simHelper.Run();
    NS_LOG_INFO("The simulation has ended");

    return 0;
}
