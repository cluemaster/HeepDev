# Heep Modernization Project

This directory contains the modernized components of the Heep IoT platform, updating the original implementation with modern languages, frameworks, and best practices.

## Overview

Heep ("The Edge for Humans") is an open-source Internet of Things (IoT) platform designed to make building, installing, and using IoT devices easier. This modernization project updates the original implementation to:

- Use modern programming languages and frameworks
- Improve security
- Add containerization
- Enhance scalability
- Provide better documentation

## Components

### Server

The modernized server is written in Python 3.11+ and uses:

- Asynchronous processing with `asyncio`
- Type hints for better code quality
- WebSockets for real-time communication
- MQTT support for IoT protocols
- MongoDB for persistent storage
- Docker containerization

### Frontend

The frontend has been completely rewritten using:

- React 18 with TypeScript
- Material UI for modern UI components
- Zustand for state management
- React Router for navigation
- Recharts for data visualization
- WebSockets for real-time updates

### Firmware

Device firmware has been updated to support:

- ESP32 and ESP8266 platforms
- OTA updates
- MQTT protocol
- Secure communication with TLS
- Web configuration interface
- Enhanced reliability and error handling

## Getting Started

### Prerequisites

- Docker and Docker Compose
- Node.js 18+ (for frontend development)
- Python 3.11+ (for server development)
- Arduino IDE or PlatformIO (for firmware development)

### Running with Docker

The easiest way to run the modernized Heep stack is with Docker Compose:

```bash
cd modernization/docker
docker-compose up -d
```

This will start:
- The Heep server
- The web frontend
- MongoDB for persistence
- MQTT broker for device communication

### Development Setup

#### Server

```bash
cd modernization/python_server
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install -r requirements.txt
python server.py
```

#### Frontend

```bash
cd modernization/frontend
npm install
npm run dev
```

#### Firmware

1. Open the Arduino IDE
2. Install the required libraries:
   - WebSockets
   - ArduinoJson
   - PubSubClient
   - EEPROM
   - LittleFS
3. Open the `modernization/firmware/ESP32_HeepClient/HeepClient.ino` file
4. Modify the WiFi credentials and server details
5. Upload to your ESP32 or ESP8266 device

## API Documentation

### Server API

The server exposes both a WebSocket API and a REST API:

- WebSocket: `ws://server-ip:8088`
- REST API: `http://server-ip:8088/api`

See the `docs/api.md` file for detailed API documentation.

### Device API

Devices expose a REST API for direct control:

- Device Info: `http://device-ip/api/device`
- Control Update: `http://device-ip/api/control` (POST)
- OTA Update: `http://device-ip/update` (POST)

## Architecture

The modernized Heep architecture follows a microservices approach:

1. **Server**: Core service that manages devices and provides APIs
2. **Frontend**: Web interface for user interaction
3. **Devices**: IoT devices running Heep firmware
4. **MongoDB**: Persistent storage for device data
5. **MQTT Broker**: Message broker for device communication

## Contributing

Contributions to the modernization effort are welcome! Please see the [MODERNIZATION.md](../MODERNIZATION.md) file for the roadmap and current priorities.

## License

This project is licensed under the Apache License 2.0 - see the LICENSE file for details.
