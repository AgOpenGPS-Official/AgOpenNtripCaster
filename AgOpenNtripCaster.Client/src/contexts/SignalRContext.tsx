import React, { createContext, useContext, useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import { useAuth } from '../hooks/useAuth';

interface SignalRContextType {
  isConnected: boolean;
  isConnecting: boolean;
}

const SignalRContext = createContext<SignalRContextType>({
  isConnected: false,
  isConnecting: false,
});

export const useSignalR = () => useContext(SignalRContext);

export const SignalRProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { isAuthenticated } = useAuth();
  const [isConnected, setIsConnected] = useState(false);
  const [isConnecting, setIsConnecting] = useState(false);

  useEffect(() => {
    // Only connect if user is authenticated
    if (!isAuthenticated) {
      // Disconnect if not authenticated
      signalRService.disconnect();
      setIsConnected(false);
      setIsConnecting(false);
      return;
    }

    // Connect to SignalR
    const connect = async () => {
      setIsConnecting(true);
      try {
        const connected = await signalRService.connect();
        setIsConnected(connected);
      } catch (error) {
        console.error('Failed to connect to SignalR:', error);
        setIsConnected(false);
      } finally {
        setIsConnecting(false);
      }
    };

    // Subscribe to connection status changes
    const unsubscribe = signalRService.onConnectionStatusChange((connected) => {
      setIsConnected(connected);
      if (!connected) {
        setIsConnecting(false);
      }
    });

    // Initial connection
    connect();

    // Cleanup on unmount
    return () => {
      unsubscribe();
      // Don't disconnect here - keep connection alive for app lifetime
      // Only disconnect when user logs out (handled by isAuthenticated change)
    };
  }, [isAuthenticated]);

  return (
    <SignalRContext.Provider value={{ isConnected, isConnecting }}>
      {children}
    </SignalRContext.Provider>
  );
};
