/**
 * Type definitions for Heep IoT platform
 */

/**
 * Represents a control on a Heep device
 */
export interface HeepControl {
  id: string;
  name: string;
  type: ControlType;
  value: number | string | boolean;
  min?: number;
  max?: number;
  options?: string[];
  unit?: string;
  icon?: string;
}

/**
 * Represents a connection between two devices
 */
export interface HeepVertex {
  id: string;
  sourceDevice: string;
  sourceControl: string;
  targetDevice: string;
  targetControl: string;
  enabled: boolean;
}

/**
 * Represents a Heep device
 */
export interface HeepDevice {
  device_id: string;
  name: string;
  ip_address: string;
  port: number;
  last_seen: number;
  controls: HeepControl[];
  vertices: HeepVertex[];
  online: boolean;
  type?: string;
  location?: string;
  firmware_version?: string;
  last_updated?: number;
}

/**
 * Types of controls supported by Heep
 */
export enum ControlType {
  TOGGLE = 'toggle',
  SLIDER = 'slider',
  BUTTON = 'button',
  MOMENTARY = 'momentary',
  DROPDOWN = 'dropdown',
  DISPLAY = 'display',
  RGB = 'rgb',
  TEMPERATURE = 'temperature',
  HUMIDITY = 'humidity',
  PRESSURE = 'pressure',
  LIGHT = 'light',
  SENSOR = 'sensor',
}

/**
 * Message types for WebSocket communication
 */
export enum MessageType {
  REGISTER_DEVICE = 'register_device',
  HEARTBEAT = 'heartbeat',
  CONTROL_UPDATE = 'control_update',
  GET_DEVICES = 'get_devices',
  DEVICE_LIST = 'device_list',
  DEVICE_STATUS = 'device_status',
  SEND_COMMAND = 'send_command',
  ERROR = 'error',
}

/**
 * Base interface for all messages
 */
export interface HeepMessage {
  message_type: MessageType;
}

/**
 * Device registration message
 */
export interface RegisterDeviceMessage extends HeepMessage {
  message_type: MessageType.REGISTER_DEVICE;
  device_id: string;
  name: string;
  ip_address: string;
  port: number;
  controls: HeepControl[];
}

/**
 * Heartbeat message
 */
export interface HeartbeatMessage extends HeepMessage {
  message_type: MessageType.HEARTBEAT;
  device_id: string;
}

/**
 * Control update message
 */
export interface ControlUpdateMessage extends HeepMessage {
  message_type: MessageType.CONTROL_UPDATE;
  device_id: string;
  control_id: string;
  value: any;
}

/**
 * Device list message
 */
export interface DeviceListMessage extends HeepMessage {
  message_type: MessageType.DEVICE_LIST;
  devices: HeepDevice[];
}

/**
 * Device status message
 */
export interface DeviceStatusMessage extends HeepMessage {
  message_type: MessageType.DEVICE_STATUS;
  device_id: string;
  online: boolean;
}

/**
 * Send command message
 */
export interface SendCommandMessage extends HeepMessage {
  message_type: MessageType.SEND_COMMAND;
  device_id: string;
  command: any;
}

/**
 * Error message
 */
export interface ErrorMessage extends HeepMessage {
  message_type: MessageType.ERROR;
  error: string;
  details?: any;
}

/**
 * Union type of all message types
 */
export type HeepMessageUnion = 
  | RegisterDeviceMessage
  | HeartbeatMessage
  | ControlUpdateMessage
  | DeviceListMessage
  | DeviceStatusMessage
  | SendCommandMessage
  | ErrorMessage;
