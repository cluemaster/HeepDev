"""
Heep Server - Modernized Version
--------------------------------
This is a Python 3.11+ implementation of the original Heep server.
The code has been updated with:
- Type hints
- Async processing with asyncio
- Modern logging
- Better error handling
- Improved security
"""

import asyncio
import json
import logging
import os
import sys
import time
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Any, Union, Tuple
import ssl

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(sys.stdout),
        logging.FileHandler('heep_server.log')
    ]
)
logger = logging.getLogger('heep_server')

@dataclass
class HeepDevice:
    """Represents a Heep device connected to the server."""
    device_id: str
    name: str
    ip_address: str
    port: int
    last_seen: float = field(default_factory=time.time)
    controls: List[Dict[str, Any]] = field(default_factory=list)
    vertices: List[Dict[str, Any]] = field(default_factory=list)
    online: bool = True
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert device to dictionary for JSON serialization."""
        return {
            'device_id': self.device_id,
            'name': self.name,
            'ip_address': self.ip_address,
            'port': self.port,
            'last_seen': self.last_seen,
            'controls': self.controls,
            'vertices': self.vertices,
            'online': self.online
        }
    
    def update_last_seen(self) -> None:
        """Update the last_seen timestamp to current time."""
        self.last_seen = time.time()
        self.online = True

class HeepServer:
    """
    Modernized Heep Server using asyncio for concurrent connections.
    This replaces the original Python 2.7 implementation with a more
    scalable and maintainable design.
    """
    
    def __init__(self, host: str = '0.0.0.0', port: int = 8088):
        self.host = host
        self.port = port
        self.devices: Dict[str, HeepDevice] = {}
        self.connections: List[asyncio.StreamWriter] = []
        self.ssl_context: Optional[ssl.SSLContext] = None
        self.setup_ssl()
        logger.info(f"Heep server initializing on {host}:{port}")
        
    def setup_ssl(self) -> None:
        """Configure SSL context for secure connections if certificates available."""
        cert_path = os.path.join('certs', 'server.crt')
        key_path = os.path.join('certs', 'server.key')
        
        if os.path.exists(cert_path) and os.path.exists(key_path):
            self.ssl_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
            self.ssl_context.load_cert_chain(cert_path, key_path)
            logger.info("SSL certificate loaded successfully")
        else:
            logger.warning("SSL certificates not found, running in insecure mode")
    
    async def device_heartbeat_monitor(self) -> None:
        """Monitor device connections and mark offline if no heartbeat received."""
        while True:
            current_time = time.time()
            for device_id, device in list(self.devices.items()):
                # Mark device offline if no heartbeat for 30 seconds
                if current_time - device.last_seen > 30:
                    if device.online:
                        device.online = False
                        logger.info(f"Device {device.name} ({device_id}) marked offline")
                        await self.broadcast_device_status(device)
                
                # Remove device if offline for more than 5 minutes
                if current_time - device.last_seen > 300:
                    logger.info(f"Removing inactive device {device.name} ({device_id})")
                    del self.devices[device_id]
                    await self.broadcast_device_list()
            
            await asyncio.sleep(10)  # Check every 10 seconds
    
    async def handle_client(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        """Handle a client connection."""
        addr = writer.get_extra_info('peername')
        logger.info(f"New connection from {addr}")
        self.connections.append(writer)
        
        try:
            while True:
                data = await reader.read(4096)
                if not data:
                    break
                
                try:
                    message = json.loads(data.decode('utf-8'))
                    await self.process_message(message, writer)
                except json.JSONDecodeError:
                    logger.error(f"Invalid JSON received from {addr}")
                except Exception as e:
                    logger.error(f"Error processing message: {e}")
        
        except (ConnectionError, asyncio.IncompleteReadError) as e:
            logger.info(f"Connection closed with {addr}: {e}")
        finally:
            writer.close()
            await writer.wait_closed()
            self.connections.remove(writer)
            logger.info(f"Connection with {addr} closed")
    
    async def process_message(self, message: Dict[str, Any], writer: asyncio.StreamWriter) -> None:
        """Process incoming message from client."""
        if 'message_type' not in message:
            logger.warning(f"Received message with no message_type: {message}")
            return
        
        message_type = message['message_type']
        
        if message_type == 'register_device':
            await self.handle_device_registration(message)
        elif message_type == 'heartbeat':
            await self.handle_heartbeat(message)
        elif message_type == 'control_update':
            await self.handle_control_update(message)
        elif message_type == 'get_devices':
            await self.send_device_list(writer)
        elif message_type == 'send_command':
            await self.forward_command(message)
        else:
            logger.warning(f"Unknown message type: {message_type}")
    
    async def handle_device_registration(self, message: Dict[str, Any]) -> None:
        """Handle device registration message."""
        device_id = message.get('device_id')
        name = message.get('name', f"Device-{device_id}")
        ip_address = message.get('ip_address')
        port = message.get('port', 80)
        controls = message.get('controls', [])
        
        if not device_id or not ip_address:
            logger.error("Missing required fields in registration message")
            return
        
        device = HeepDevice(
            device_id=device_id,
            name=name,
            ip_address=ip_address,
            port=port,
            controls=controls
        )
        
        self.devices[device_id] = device
        logger.info(f"Registered device: {name} ({device_id}) at {ip_address}:{port}")
        
        await self.broadcast_device_list()
    
    async def handle_heartbeat(self, message: Dict[str, Any]) -> None:
        """Handle device heartbeat message."""
        device_id = message.get('device_id')
        
        if not device_id or device_id not in self.devices:
            logger.warning(f"Heartbeat from unknown device: {device_id}")
            return
        
        device = self.devices[device_id]
        was_offline = not device.online
        
        device.update_last_seen()
        
        if was_offline:
            logger.info(f"Device {device.name} ({device_id}) is back online")
            await self.broadcast_device_status(device)
    
    async def handle_control_update(self, message: Dict[str, Any]) -> None:
        """Handle control update message from device."""
        device_id = message.get('device_id')
        control_id = message.get('control_id')
        value = message.get('value')
        
        if not device_id or not control_id or device_id not in self.devices:
            logger.warning(f"Control update for unknown device or control")
            return
        
        device = self.devices[device_id]
        
        for control in device.controls:
            if control.get('id') == control_id:
                control['value'] = value
                await self.broadcast_control_update(device_id, control_id, value)
                break
    
    async def forward_command(self, message: Dict[str, Any]) -> None:
        """Forward command to device."""
        device_id = message.get('device_id')
        command = message.get('command')
        
        if not device_id or not command or device_id not in self.devices:
            logger.warning(f"Cannot forward command to unknown device")
            return
        
        device = self.devices[device_id]
        
        if not device.online:
            logger.warning(f"Cannot forward command to offline device: {device.name}")
            return
        
        # In a real implementation, we would send the command to the device
        # For now, just log it
        logger.info(f"Would send command {command} to device {device.name} at {device.ip_address}:{device.port}")
    
    async def broadcast_device_list(self) -> None:
        """Broadcast the current device list to all connected clients."""
        message = {
            'message_type': 'device_list',
            'devices': [device.to_dict() for device in self.devices.values()]
        }
        
        await self.broadcast_message(message)
    
    async def broadcast_device_status(self, device: HeepDevice) -> None:
        """Broadcast device status update to all connected clients."""
        message = {
            'message_type': 'device_status',
            'device_id': device.device_id,
            'online': device.online
        }
        
        await self.broadcast_message(message)
    
    async def broadcast_control_update(self, device_id: str, control_id: str, value: Any) -> None:
        """Broadcast control update to all connected clients."""
        message = {
            'message_type': 'control_update',
            'device_id': device_id,
            'control_id': control_id,
            'value': value
        }
        
        await self.broadcast_message(message)
    
    async def broadcast_message(self, message: Dict[str, Any]) -> None:
        """Broadcast a message to all connected clients."""
        data = json.dumps(message).encode('utf-8')
        
        for writer in self.connections:
            try:
                writer.write(data)
                await writer.drain()
            except Exception as e:
                logger.error(f"Error broadcasting message: {e}")
    
    async def send_device_list(self, writer: asyncio.StreamWriter) -> None:
        """Send the current device list to a specific client."""
        message = {
            'message_type': 'device_list',
            'devices': [device.to_dict() for device in self.devices.values()]
        }
        
        data = json.dumps(message).encode('utf-8')
        
        try:
            writer.write(data)
            await writer.drain()
        except Exception as e:
            logger.error(f"Error sending device list: {e}")
    
    async def start(self) -> None:
        """Start the server."""
        server = await asyncio.start_server(
            self.handle_client, 
            self.host, 
            self.port,
            ssl=self.ssl_context
        )
        
        heartbeat_task = asyncio.create_task(self.device_heartbeat_monitor())
        
        addr = server.sockets[0].getsockname()
        logger.info(f'Serving on {addr}')
        
        async with server:
            await server.serve_forever()

async def main() -> None:
    """Main entry point for the server."""
    server = HeepServer()
    await server.start()

if __name__ == "__main__":
    asyncio.run(main())
