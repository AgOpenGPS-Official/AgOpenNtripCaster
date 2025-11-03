import * as signalR from '@microsoft/signalr';

export interface ClientPositionUpdate {
  clientId: string;
  username: string;
  mountPoint: string;
  latitude: number;
  longitude: number;
  accuracy?: number;
  timestamp: string;
}

export interface ClientStreamStatusUpdate {
  clientId: string;
  username: string;
  status: string; // 'streaming' | 'paused' | 'connected'
  reason: string;
  updatedAt: string;
}

export interface ClientDisconnectedUpdate {
  clientId: string;
  username: string;
}

export interface DashboardStats {
  activeClients: number;
  activeSources: number;
  totalBytesReceived: number;
  totalBytesSent: number;
  totalBytesTransferred: number;
  serverStartTime: string;
  currentTime: string;
  uptimeFormatted: string;
}

export interface ActivityEvent {
  id: number;
  type: string;
  mountPointId: number;
  mountPointName: string;
  userId?: string;
  userName?: string;
  description: string;
  createdAt: string;
}

export interface MountPointStatusUpdate {
  mountPointId: number;
  mountPointName: string;
  activeSourceCount: number;
  activeClientCount: number;
  updatedAt: string;
}

export const AlertSeverity = {
  Info: 'Info',
  Warning: 'Warning',
  Error: 'Error',
  Critical: 'Critical',
} as const;

export type AlertSeverity = typeof AlertSeverity[keyof typeof AlertSeverity];

export interface SystemAlert {
  id: string;
  severity: AlertSeverity;
  title: string;
  message: string;
  createdAt: string;
  code?: string;
  metadata?: Record<string, unknown>;
}

type PositionUpdateCallback = (update: ClientPositionUpdate) => void;
type StatusUpdateCallback = (update: ClientStreamStatusUpdate) => void;
type ClientDisconnectedCallback = (update: ClientDisconnectedUpdate) => void;
type ConnectionStatusCallback = (isConnected: boolean) => void;
type DashboardStatsCallback = (stats: DashboardStats) => void;
type ActivityCallback = (activity: ActivityEvent) => void;
type MountPointStatusCallback = (update: MountPointStatusUpdate) => void;
type SystemAlertCallback = (alert: SystemAlert) => void;
type ConnectionStatsCallback = (stats: ConnectionStats) => void;

export interface ConnectionStats {
  totalConnections: number;
  activeClientCount: number;
  activeSourceCount: number;
  throughputMbps: number;
  uploadMbps: number;
  downloadMbps: number;
  averageLatencyMs: number;
  cpuUsagePercent: number;
  memoryUsagePercent: number;
  collectedAt: string;
}

class SignalRService {
  private connection: signalR.HubConnection | null = null;
  private positionUpdateCallbacks: Set<PositionUpdateCallback> = new Set();
  private statusUpdateCallbacks: Set<StatusUpdateCallback> = new Set();
  private clientDisconnectedCallbacks: Set<ClientDisconnectedCallback> = new Set();
  private connectionStatusCallbacks: Set<ConnectionStatusCallback> = new Set();
  private dashboardStatsCallbacks: Set<DashboardStatsCallback> = new Set();
  private activityCallbacks: Set<ActivityCallback> = new Set();
  private mountPointStatusCallbacks: Set<MountPointStatusCallback> = new Set();
  private systemAlertCallbacks: Set<SystemAlertCallback> = new Set();
  private connectionStatsCallbacks: Set<ConnectionStatsCallback> = new Set();
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectDelay = 3000; // 3 seconds

  async connect(): Promise<boolean> {
    try {
      if (this.connection?.state === signalR.HubConnectionState.Connected) {
        return true;
      }

      // Build hub URL - use base server URL (without /api since the hub path includes it)
      const serverUrl = import.meta.env.VITE_API_URL ?
        import.meta.env.VITE_API_URL.replace('/api', '') :
        'http://localhost:5000';
      const hubUrl = `${serverUrl}/api/ntrip-hub`;

      this.connection = new signalR.HubConnectionBuilder()
        .withUrl(hubUrl, {
          withCredentials: true,
        })
        .withAutomaticReconnect({
          nextRetryDelayInMilliseconds: (retryContext) => {
            if (retryContext.previousRetryCount >= this.maxReconnectAttempts) {
              return null; // Stop reconnecting
            }
            return Math.pow(2, retryContext.previousRetryCount) * 1000;
          },
        })
        .configureLogging(signalR.LogLevel.Information)
        .build();

      // Set up event handlers
      this.connection.on('ClientPositionUpdated', (update: ClientPositionUpdate) => {
        console.log('📍 SignalR ClientPositionUpdated:', {
          clientId: update.clientId,
          username: update.username,
          latitude: update.latitude,
          longitude: update.longitude,
          accuracy: update.accuracy,
          timestamp: update.timestamp
        });
        this.notifyPositionUpdateListeners(update);
      });

      this.connection.on('ClientStreamStatusChanged', (update: ClientStreamStatusUpdate) => {
        this.notifyStatusUpdateListeners(update);
      });

      this.connection.on('ClientConnected', () => {
        // Client connected event received
      });

      this.connection.on('ClientDisconnected', (update: ClientDisconnectedUpdate) => {
        this.notifyClientDisconnectedListeners(update);
      });

      // Real-time dashboard stats updates
      this.connection.on('DashboardStatsUpdated', (stats: DashboardStats) => {
        this.notifyDashboardStatsListeners(stats);
      });

      // Real-time activity events
      this.connection.on('ActivityCreated', (activity: ActivityEvent) => {
        this.notifyActivityListeners(activity);
      });

      // Real-time mount point status updates
      this.connection.on('MountPointStatusChanged', (update: MountPointStatusUpdate) => {
        this.notifyMountPointStatusListeners(update);
      });

      // System alerts
      this.connection.on('SystemAlert', (alert: SystemAlert) => {
        this.notifySystemAlertListeners(alert);
      });

      // Real-time connection statistics
      this.connection.on('ConnectionStatsUpdated', (stats: ConnectionStats) => {
        this.notifyConnectionStatsListeners(stats);
      });

      // Handle connection state changes
      this.connection.onreconnecting(() => {
        this.notifyConnectionStatusListeners(false);
      });

      this.connection.onreconnected(() => {
        this.reconnectAttempts = 0;
        this.notifyConnectionStatusListeners(true);
      });

      this.connection.onclose(() => {
        this.notifyConnectionStatusListeners(false);
      });

      await this.connection.start();
      this.reconnectAttempts = 0;
      this.notifyConnectionStatusListeners(true);
      return true;
    } catch (error) {
      console.error('SignalR connection failed:', error);
      this.reconnectAttempts++;

      if (this.reconnectAttempts < this.maxReconnectAttempts) {
        console.log(`Retrying connection in ${this.reconnectDelay}ms...`);
        setTimeout(() => this.connect(), this.reconnectDelay);
      }

      this.notifyConnectionStatusListeners(false);
      return false;
    }
  }

  async disconnect(): Promise<void> {
    if (this.connection) {
      try {
        await this.connection.stop();
        console.log('SignalR disconnected');
        this.notifyConnectionStatusListeners(false);
      } catch (error) {
        console.error('Error disconnecting SignalR:', error);
      }
    }
  }

  isConnected(): boolean {
    return this.connection?.state === signalR.HubConnectionState.Connected;
  }

  // Subscribe to position updates
  onPositionUpdate(callback: PositionUpdateCallback): () => void {
    this.positionUpdateCallbacks.add(callback);
    // Return unsubscribe function
    return () => {
      this.positionUpdateCallbacks.delete(callback);
    };
  }

  // Subscribe to status updates
  onStatusUpdate(callback: StatusUpdateCallback): () => void {
    this.statusUpdateCallbacks.add(callback);
    return () => {
      this.statusUpdateCallbacks.delete(callback);
    };
  }

  // Subscribe to client disconnected events
  onClientDisconnected(callback: ClientDisconnectedCallback): () => void {
    this.clientDisconnectedCallbacks.add(callback);
    return () => {
      this.clientDisconnectedCallbacks.delete(callback);
    };
  }

  // Subscribe to connection status changes
  onConnectionStatusChange(callback: ConnectionStatusCallback): () => void {
    this.connectionStatusCallbacks.add(callback);
    return () => {
      this.connectionStatusCallbacks.delete(callback);
    };
  }

  // Subscribe to dashboard stats updates
  onDashboardStats(callback: DashboardStatsCallback): () => void {
    this.dashboardStatsCallbacks.add(callback);
    return () => {
      this.dashboardStatsCallbacks.delete(callback);
    };
  }

  // Subscribe to activity events
  onActivityCreated(callback: ActivityCallback): () => void {
    this.activityCallbacks.add(callback);
    return () => {
      this.activityCallbacks.delete(callback);
    };
  }

  // Subscribe to mount point status changes
  onMountPointStatusChanged(callback: MountPointStatusCallback): () => void {
    this.mountPointStatusCallbacks.add(callback);
    return () => {
      this.mountPointStatusCallbacks.delete(callback);
    };
  }

  // Subscribe to system alerts
  onSystemAlert(callback: SystemAlertCallback): () => void {
    this.systemAlertCallbacks.add(callback);
    return () => {
      this.systemAlertCallbacks.delete(callback);
    };
  }

  // Subscribe to connection statistics
  onConnectionStatsUpdated(callback: ConnectionStatsCallback): () => void {
    this.connectionStatsCallbacks.add(callback);
    return () => {
      this.connectionStatsCallbacks.delete(callback);
    };
  }

  private notifyPositionUpdateListeners(update: ClientPositionUpdate): void {
    this.positionUpdateCallbacks.forEach((callback) => {
      try {
        callback(update);
      } catch (error) {
        console.error('Error in position update callback:', error);
      }
    });
  }

  private notifyStatusUpdateListeners(update: ClientStreamStatusUpdate): void {
    this.statusUpdateCallbacks.forEach((callback) => {
      try {
        callback(update);
      } catch (error) {
        console.error('Error in status update callback:', error);
      }
    });
  }

  private notifyClientDisconnectedListeners(update: ClientDisconnectedUpdate): void {
    this.clientDisconnectedCallbacks.forEach((callback) => {
      try {
        callback(update);
      } catch (error) {
        console.error('Error in client disconnected callback:', error);
      }
    });
  }

  private notifyConnectionStatusListeners(isConnected: boolean): void {
    this.connectionStatusCallbacks.forEach((callback) => {
      try {
        callback(isConnected);
      } catch (error) {
        console.error('Error in connection status callback:', error);
      }
    });
  }

  private notifyDashboardStatsListeners(stats: DashboardStats): void {
    this.dashboardStatsCallbacks.forEach((callback) => {
      try {
        callback(stats);
      } catch (error) {
        console.error('Error in dashboard stats callback:', error);
      }
    });
  }

  private notifyActivityListeners(activity: ActivityEvent): void {
    this.activityCallbacks.forEach((callback) => {
      try {
        callback(activity);
      } catch (error) {
        console.error('Error in activity callback:', error);
      }
    });
  }

  private notifyMountPointStatusListeners(update: MountPointStatusUpdate): void {
    this.mountPointStatusCallbacks.forEach((callback) => {
      try {
        callback(update);
      } catch (error) {
        console.error('Error in mount point status callback:', error);
      }
    });
  }

  private notifySystemAlertListeners(alert: SystemAlert): void {
    this.systemAlertCallbacks.forEach((callback) => {
      try {
        callback(alert);
      } catch (error) {
        console.error('Error in system alert callback:', error);
      }
    });
  }

  private notifyConnectionStatsListeners(stats: ConnectionStats): void {
    this.connectionStatsCallbacks.forEach((callback) => {
      try {
        callback(stats);
      } catch (error) {
        console.error('Error in connection stats callback:', error);
      }
    });
  }
}

// Export singleton instance
export const signalRService = new SignalRService();
