# Heep: The Edge for Humans

> 🚀 **Note**: This repository is currently undergoing a major modernization effort. See [MODERNIZATION.md](MODERNIZATION.md) for details.

**Heep** is an open-source Internet of Things (IoT) platform designed to make building, installing, and using IoT devices easier. The project's tagline is "The Edge for Humans," as it aims to make edge computing and IoT technology more accessible to everyday users.

This repository contains Circuit Board designs, Server Code, OS Dependent Client Code, and Client Firmware for Heep devices.

## Project Structure

The Heep ecosystem consists of several main components:

1. **Hardware Development** (see [Heep_Hardware](https://github.com/cluemaster/Heep_Hardware))
   - Circuit board designs for various IoT applications
   - Multiple board variants including:
     - Heep 32u4 Board (ATMega32u4 microprocessor)
     - Heep Teensy Board (simple network-enabled Teensy breakout)
     - Heep Teensy Board With Relays (for controlling high voltage/current devices)

2. **Software Architecture**
   - Server code (Python-based)
   - Client firmware for microcontrollers
   - Desktop applications (built with Electron)
   - Web interfaces for device control

3. **Example Applications**
   - Home automation devices
   - Smart plugs and receptacles
   - Security applications (like the [Passive IR Sensor](https://github.com/cluemaster/PassiveIRSensor))
   - Environmental monitoring

## Current Dependencies

> ⚠️ These dependencies are being updated as part of the modernization effort

* Heep Server
	* Python 2.7 (upgrading to Python 3.11+)
* Python Clients
	* Python 2.7 (upgrading to Python 3.11+)
* Firmware Clients
	* Modified UIP_Ethernet Library found in Firmware/ArduinoLibrary
	* Teensyduino
* Front End
	* Node.js

## Getting Started (Legacy Instructions)

### Running the Heep Server
1. Clone the repo
2. Open a terminal and navigate to the FrontEnd/FlowChart directory
3. Type `npm install` to install all front end dependencies
4. Type `npm run bs` to build and start the FlowChart
5. Open a terminal and navigate to the Server directory
6. Type `python TestPLCServer.py` to start the server that will listen to devices on the network
7. Connect Devices and view the front end by opening a web browser and going to your-IP:3001

### Running a Heep Client
1. Clone the repo
2. Open a terminal and navigate to a client in the ExampleDevices directory
3. Find the name of the .py file in the example device folder you chose
4. Type `Python device-name.py` in the terminal to start the client
5. The client should show up on the heep server front end

## Modernization Efforts

This project is being modernized to use current languages, frameworks, and best practices:

- Python 3.11+ with asyncio and type hints
- Modern JavaScript/TypeScript frameworks
- Updated firmware libraries with enhanced security
- Docker containerization
- Improved documentation
- Enhanced security features

See [MODERNIZATION.md](MODERNIZATION.md) for the complete roadmap.

## Heep Architecture

### Software Design for Sending

![Sending Architecture](Architecture%20Drawings/HeepBackendSending.png "Sending")

### Software Design for Receiving

![Receiving Architecture](Architecture%20Drawings/HeepBackendReceiving.png "Receiving")

## Contributing

Contributions to the modernization effort are welcome! Please see the [MODERNIZATION.md](MODERNIZATION.md) file for current priorities and the roadmap.

## License

This project is licensed under the Apache License 2.0 - see the LICENSE file for details.
