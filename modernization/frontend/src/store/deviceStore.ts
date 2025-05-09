import { create } from 'zustand';
import { HeepDevice, HeepControl, HeepVertex, MessageType } from '../types/heep';
import websocketService from '../services/websocketService';

interface DeviceState {
  devices: Record<string, HeepDevice>;
  isLoading: boolean;
  error: string | null;
  
  // Actions
  loadDevices: () => void;
  updateDevice: (device: HeepDevice) => void;
  updateControl: (deviceId: string, controlId: string, value: any) => void;
  addVertex: (vertex: HeepVertex) => void;
  removeVertex: (vertexId: string) => void;
  updateVertexEnabled: (vertexId: string, enabled: boolean) => void;
  setDeviceOnlineStatus: (deviceId: string, online: boolean) => void;
  renameDevice: (deviceId: string, newName: string) => void;
  
  // Computed values
  getAllDevices: () => HeepDevice[];
  getOnlineDevices: () => HeepDevice[];
  getOfflineDevices: () => HeepDevice[];
  getVertices: () => HeepVertex[];
}

const useDeviceStore = create<DeviceState>((set, get) => ({
  devices: {},
  isLoading: false,
  error: null,
  
  loadDevices: () => {
    set({ isLoading: true, error: null });
    
    // Set up WebSocket listeners
    websocketService.addListener(MessageType.DEVICE_LIST, (message) => {
      const deviceListMessage = message as { devices: HeepDevice[] };
      const devicesMap: Record<string, HeepDevice> = {};
      
      deviceListMessage.devices.forEach(device => {
        devicesMap[device.device_id] = device;
      });
      
      set({ devices: devicesMap, isLoading: false });
    });
    
    websocketService.addListener(MessageType.DEVICE_STATUS, (message) => {
      const statusMessage = message as { device_id: string, online: boolean };
      get().setDeviceOnlineStatus(statusMessage.device_id, statusMessage.online);
    });
    
    websocketService.addListener(MessageType.CONTROL_UPDATE, (message) => {
      const updateMessage = message as { device_id: string, control_id: string, value: any };
      get().updateControl(
        updateMessage.device_id, 
        updateMessage.control_id, 
        updateMessage.value
      );
    });
    
    websocketService.addListener(MessageType.ERROR, (message) => {
      const errorMessage = message as { error: string };
      set({ error: errorMessage.error, isLoading: false });
    });
    
    // Connect to the WebSocket server
    websocketService.connect();
    
    // Request the device list
    websocketService.send({
      message_type: MessageType.GET_DEVICES
    });
  },
  
  updateDevice: (device) => {
    set(state => ({
      devices: {
        ...state.devices,
        [device.device_id]: device
      }
    }));
  },
  
  updateControl: (deviceId, controlId, value) => {
    set(state => {
      // If device doesn't exist, do nothing
      if (!state.devices[deviceId]) {
        return state;
      }
      
      // Clone the device
      const device = { ...state.devices[deviceId] };
      
      // Find and update the control
      const controlIndex = device.controls.findIndex(c => c.id === controlId);
      
      if (controlIndex !== -1) {
        // Clone the controls array
        const controls = [...device.controls];
        
        // Update the control
        controls[controlIndex] = {
          ...controls[controlIndex],
          value
        };
        
        // Update the device with the new controls
        device.controls = controls;
        
        // Return the updated state
        return {
          devices: {
            ...state.devices,
            [deviceId]: device
          }
        };
      }
      
      // If control not found, do nothing
      return state;
    });
    
    // Send control update to server
    websocketService.send({
      message_type: MessageType.CONTROL_UPDATE,
      device_id: deviceId,
      control_id: controlId,
      value
    });
  },
  
  addVertex: (vertex) => {
    set(state => {
      // Get source and target devices
      const sourceDevice = state.devices[vertex.sourceDevice];
      const targetDevice = state.devices[vertex.targetDevice];
      
      if (!sourceDevice || !targetDevice) {
        return state;
      }
      
      // Clone the devices
      const updatedSourceDevice = { ...sourceDevice };
      const updatedTargetDevice = { ...targetDevice };
      
      // Add the vertex to both devices
      updatedSourceDevice.vertices = [...(updatedSourceDevice.vertices || []), vertex];
      updatedTargetDevice.vertices = [...(updatedTargetDevice.vertices || []), vertex];
      
      // Return the updated state
      return {
        devices: {
          ...state.devices,
          [vertex.sourceDevice]: updatedSourceDevice,
          [vertex.targetDevice]: updatedTargetDevice
        }
      };
    });
  },
  
  removeVertex: (vertexId) => {
    set(state => {
      // Create a copy of the devices
      const devices = { ...state.devices };
      
      // For each device, filter out the vertex
      Object.keys(devices).forEach(deviceId => {
        const device = devices[deviceId];
        
        if (device.vertices && device.vertices.length > 0) {
          device.vertices = device.vertices.filter(v => v.id !== vertexId);
        }
      });
      
      return { devices };
    });
  },
  
  updateVertexEnabled: (vertexId, enabled) => {
    set(state => {
      // Create a copy of the devices
      const devices = { ...state.devices };
      
      // For each device, update the vertex
      Object.keys(devices).forEach(deviceId => {
        const device = devices[deviceId];
        
        if (device.vertices && device.vertices.length > 0) {
          device.vertices = device.vertices.map(v => 
            v.id === vertexId ? { ...v, enabled } : v
          );
        }
      });
      
      return { devices };
    });
  },
  
  setDeviceOnlineStatus: (deviceId, online) => {
    set(state => {
      // If device doesn't exist, do nothing
      if (!state.devices[deviceId]) {
        return state;
      }
      
      // Clone the device and update the online status
      const device = { 
        ...state.devices[deviceId],
        online
      };
      
      // Return the updated state
      return {
        devices: {
          ...state.devices,
          [deviceId]: device
        }
      };
    });
  },
  
  renameDevice: (deviceId, newName) => {
    set(state => {
      // If device doesn't exist, do nothing
      if (!state.devices[deviceId]) {
        return state;
      }
      
      // Clone the device and update the name
      const device = { 
        ...state.devices[deviceId],
        name: newName
      };
      
      // Return the updated state
      return {
        devices: {
          ...state.devices,
          [deviceId]: device
        }
      };
    });
  },
  
  // Computed values
  getAllDevices: () => {
    return Object.values(get().devices);
  },
  
  getOnlineDevices: () => {
    return Object.values(get().devices).filter(device => device.online);
  },
  
  getOfflineDevices: () => {
    return Object.values(get().devices).filter(device => !device.online);
  },
  
  getVertices: () => {
    const devices = get().devices;
    const allVertices: HeepVertex[] = [];
    
    Object.values(devices).forEach(device => {
      if (device.vertices && device.vertices.length > 0) {
        device.vertices.forEach(vertex => {
          // Only add each vertex once (they appear in both source and target devices)
          if (!allVertices.some(v => v.id === vertex.id)) {
            allVertices.push(vertex);
          }
        });
      }
    });
    
    return allVertices;
  }
}));

export default useDeviceStore;
