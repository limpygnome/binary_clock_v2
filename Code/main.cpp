/*                       ____               ____________
 *                      |    |             |            |
 *                      |    |             |    ________|
 *                      |    |             |   |
 *                      |    |             |   |    
 *                      |    |             |   |    ____
 *                      |    |             |   |   |    |
 *                      |    |_______      |   |___|    |
 *                      |            |  _  |            |
 *                      |____________| |_| |____________|
 *                        
 *      Author(s):      limpygnome (Marcus Craske)              limpygnome@gmail.com
 * 
 *      License:        Creative Commons Attribution-ShareAlike 3.0 Unported
 *                      http://creativecommons.org/licenses/by-sa/3.0/
 * 
 *      File:           main.cpp
 *      Path:           main.cpp
 * 
 *      Change-Log:
 *                      2013-06-25      Added header.
 * 
 * *****************************************************************************
 * The execution entry-point for the program.
 * *****************************************************************************
 */
#include <iostream>
#include <signal.h>

#include "ServiceController.h"

#include "HttpHandler.h"
#include "Display.h"
#include "Sensors.h"
#include "Alarm.h"
#include "WebService.h"
#include "Configurator.h"
#include "CountryLookup.h"
#include "RelayBoard.h"
using namespace BC::Services;

#include "ClientSocket.h"
#include <fstream>
using std::ofstream;
using BC::Web::Core::ClientSocket;

#include "Utils.h"

// The flag below is used to set the clock into virtual mode, allowing
// debugging without any GPIO/mapped-memory interaction. Useful for testing on a non-pi
// machine. Set to true to use virtual mode, false for real hardware. If you do not
// set virtual mode on a non-pi system, you may cause system instability and even
// damage.
#define HARDWARE_VIRTUAL_MODE   false

using BC::ServiceController;
using BC::Utils;
using std::cout;
using std::endl;

ServiceController *controller = 0;

// Interrupt to catch exit signal, allows graceful shutdown.
void signalHandler(int signal)
{
    cout << "Process exit signal caught, terminating controller..." << endl;
    if(controller != 0 && controller->isRunning())
        controller->stop();
    delete controller;
    cout << "Good-bye!" << endl;
    exit(signal);
}

int main(int argc, char** argv)
{
    // Add signal handlers
    signal(SIGINT, signalHandler);      // Interrupt from keyboard signal
    signal(SIGTERM, signalHandler);     // Termination signal

    // Create service controller
    controller = new ServiceController(!HARDWARE_VIRTUAL_MODE);
    // Add services
    // -- Alarm
    controller->serviceAdd(new Alarm(controller));
    // -- Configurator
    controller->serviceAdd(new Configurator(controller));
    // -- Display
    controller->serviceAdd(new Display(controller));
    // -- Sensors
    controller->serviceAdd(new Sensors(controller));
    // -- Web HTTP
    HttpHandler *handler = new HttpHandler();
    controller->serviceAdd(new WebService(controller, handler));
    // -- Country Lookup Service
    controller->serviceAdd(new CountryLookup(controller));
    // -- Relay Board
    controller->serviceAdd(new RelayBoard(controller));
    // Start the service controller
    controller->start();
    // Wait for the controller to terminate
    controller->waitForShutdown();
    // Stop all services
    controller->stop();
    // Dispose resources
    delete controller;
    // Good-bye - return signal to say we've shutdown successfully!
    std::cout << "Shutdown successful." << std::endl;
    return 0;
}
