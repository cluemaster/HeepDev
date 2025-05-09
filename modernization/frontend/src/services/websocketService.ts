import { HeepMessageUnion, MessageType } from '../types/heep';

/**
 * Type for WebSocket event listeners
 */
type WebSocketListener = (message: HeepMessageUnion) => void;

/**
 * Service for WebSocket communication with Heep server
 */
class WebSocketService {
  private socket: WebSocket | null = null;
  private listeners: Map<MessageType, WebSocketListener[]> = new Map();
  private reconnectInterval: number = 5000; // 5 seconds
  private reconnectAttempts: number = 0;
  private maxReconnectAttempts: number = 10;
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null;
  private url: string;
  
  constructor(url: string = `ws://${window.location.hostname}:8088`) {
    this.url = url;
    // Replace http(s) with ws(s) if the URL includes it
    if (this.url.startsWith('http')) {
      this.url = this.url.replace(/^http/, 'ws');
    }
  }
  
  /**
   * Connect to the WebSocket server
   */
  public connect(): void {
    if (this.socket && (this.socket.readyState === WebSocket.OPEN || this.socket.readyState === WebSocket.CONNECTING)) {
      console.log('WebSocket already connected or connecting');
      return;
    }
    
    this.socket = new WebSocket(this.url);
    
    this.socket.onopen = this.handleOpen.bind(this);
    this.socket.onmessage = this.handleMessage.bind(this);
    this.socket.onclose = this.handleClose.bind(this);
    this.socket.onerror = this.handleError.bind(this);
  }
  
  /**
   * Disconnect from the WebSocket server
   */
  public disconnect(): void {
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer);
      this.reconnectTimer = null;
    }
    
    if (this.socket) {
      this.socket.close();
      this.socket = null;
    }
    
    this.listeners.clear();
    this.reconnectAttempts = 0;
  }
  
  /**
   * Send a message to the WebSocket server
   */
  public send(message: HeepMessageUnion): void {
    if (this.socket && this.socket.readyState === WebSocket.OPEN) {
      this.socket.send(JSON.stringify(message));
    } else {
      console.error('Cannot send message, WebSocket is not connected');
    }
  }
  
  /**
   * Add a listener for a specific message type
   */
  public addListener(type: MessageType, listener: WebSocketListener): void {
    if (!this.listeners.has(type)) {
      this.listeners.set(type, []);
    }
    
    this.listeners.get(type)?.push(listener);
  }
  
  /**
   * Remove a listener for a specific message type
   */
  public removeListener(type: MessageType, listener: WebSocketListener): void {
    const typeListeners = this.listeners.get(type);
    
    if (typeListeners) {
      const index = typeListeners.indexOf(listener);
      
      if (index !== -1) {
        typeListeners.splice(index, 1);
      }
    }
  }
  
  /**
   * Handle WebSocket open event
   */
  private handleOpen(): void {
    console.log('WebSocket connected');
    this.reconnectAttempts = 0;
    
    // Request the device list when the connection is established
    this.send({
      message_type: MessageType.GET_DEVICES
    });
  }
  
  /**
   * Handle WebSocket message event
   */
  private handleMessage(event: MessageEvent): void {
    try {
      const message = JSON.parse(event.data) as HeepMessageUnion;
      
      // Notify all listeners for this message type
      const typeListeners = this.listeners.get(message.message_type);
      
      if (typeListeners) {
        typeListeners.forEach(listener => listener(message));
      }
    } catch (error) {
      console.error('Error parsing WebSocket message:', error);
    }
  }
  
  /**
   * Handle WebSocket close event
   */
  private handleClose(): void {
    console.log('WebSocket disconnected');
    
    // Attempt to reconnect if reconnect attempts are below the maximum
    if (this.reconnectAttempts < this.maxReconnectAttempts) {
      this.reconnectAttempts++;
      
      console.log(`Reconnecting (attempt ${this.reconnectAttempts} of ${this.maxReconnectAttempts})...`);
      
      this.reconnectTimer = setTimeout(() => {
        this.connect();
      }, this.reconnectInterval);
    } else {
      console.error('Maximum reconnect attempts reached');
    }
  }
  
  /**
   * Handle WebSocket error event
   */
  private handleError(error: Event): void {
    console.error('WebSocket error:', error);
  }
}

// Create a singleton instance
const websocketService = new WebSocketService();

export default websocketService;
