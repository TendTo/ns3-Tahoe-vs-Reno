#include "simulation/configuration.h"
#include "simulation/simulator-helper.h"
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
    Tracer tracer(conf, GraphDataUpdateType::All);

    SimulatorHelper simHelper(conf, tracer);
    simHelper.Setup();

    NS_LOG_INFO("Run Simulation");
    simHelper.Run();
    NS_LOG_INFO("The simulation has ended");

    return 0;
}
