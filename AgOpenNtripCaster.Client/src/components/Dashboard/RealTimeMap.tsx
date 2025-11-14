import React, { useEffect, useRef, useState } from 'react';
import L from 'leaflet';
import type { LatLng } from 'leaflet';
import 'leaflet/dist/leaflet.css';
import styles from './RealTimeMap.module.css';

interface ClientPosition {
  id: string;
  name: string;
  latitude: number;
  longitude: number;
  accuracy?: number;
  lastUpdate: number;
  isStale: boolean;
}

interface RealTimeMapProps {
  clients?: ClientPosition[];
  sources?: { id: string; name: string; latitude: number; longitude: number }[];
}

// Fix for Leaflet default markers
// @ts-expect-error - Leaflet internal property access for icon URL fix
delete L.Icon.Default.prototype._getIconUrl;
L.Icon.Default.mergeOptions({
  iconRetinaUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.9.4/images/marker-icon-2x.png',
  iconUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.9.4/images/marker-icon.png',
  shadowUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.9.4/images/marker-shadow.png',
});

export const RealTimeMap: React.FC<RealTimeMapProps> = ({ clients = [], sources = [] }) => {
  const mapContainer = useRef<HTMLDivElement>(null);
  const map = useRef<L.Map | null>(null);
  const markers = useRef<Map<string, L.Marker>>(new Map());
  const [mapReady, setMapReady] = useState(false);

  // Initialize map
  useEffect(() => {
    if (!mapContainer.current) return;

    if (!map.current) {
      // Create map centered on world view with slightly zoomed in default
      map.current = L.map(mapContainer.current).setView([20, 0], 4);

      // Add OpenStreetMap tiles with proper crossOrigin
      L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '© OpenStreetMap contributors',
        maxZoom: 19,
        crossOrigin: true,
      }).addTo(map.current);

      // Invalidate size with longer delay to ensure DOM is ready
      setTimeout(() => {
        map.current?.invalidateSize();
      }, 300);

      setMapReady(true);
    }

    return () => {
      // Don't destroy map on unmount - just clean it up
    };
  }, []);

  // Create simple POI marker for rovers (green circle with icon)
  const createRoverMarker = () => {
    return L.divIcon({
      html: `<div style="
        background: linear-gradient(135deg, #4CAF50 0%, #45a049 100%);
        border: 3px solid white;
        border-radius: 50%;
        width: 32px;
        height: 32px;
        display: flex;
        align-items: center;
        justify-content: center;
        font-size: 18px;
        box-shadow: 0 2px 8px rgba(0,0,0,0.3);
        cursor: pointer;
      ">🚜</div>`,
      iconSize: [32, 32],
      iconAnchor: [16, 16],
      popupAnchor: [0, -16],
      className: 'rover-marker'
    });
  };

  // Update client markers
  useEffect(() => {
    if (!map.current || !mapReady) return;

    // Update or create markers for clients
    clients.forEach((client) => {
      const markerId = `client-${client.id}`;
      const latlng: LatLng = L.latLng(client.latitude, client.longitude);

      if (markers.current.has(markerId)) {
        // Update existing marker
        const marker = markers.current.get(markerId)!;
        marker.setLatLng(latlng);

        // Update popup with current data
        const popupContent = `
          <div style="font-size: 12px;">
            <strong>🚜 ${client.name}</strong><br />
            Lat: ${client.latitude.toFixed(6)}<br />
            Lon: ${client.longitude.toFixed(6)}<br />
            Accuracy: ${client.accuracy?.toFixed(2) || 'N/A'}m<br />
            <span style="color: ${client.isStale ? 'red' : 'green'}; font-weight: bold;">
              ${client.isStale ? '⚠️ Stale' : '✓ Fresh'}
            </span>
          </div>
        `;
        marker.setPopupContent(popupContent);
      } else if (map.current) {
        // Create new marker with rover marker
        const icon = createRoverMarker();
        const marker = L.marker(latlng, { icon, title: client.name });

        const popupContent = `
          <div style="font-size: 12px;">
            <strong>🚜 ${client.name}</strong><br />
            Lat: ${client.latitude.toFixed(6)}<br />
            Lon: ${client.longitude.toFixed(6)}<br />
            Accuracy: ${client.accuracy?.toFixed(2) || 'N/A'}m<br />
            <span style="color: ${client.isStale ? 'red' : 'green'}; font-weight: bold;">
              ${client.isStale ? '⚠️ Stale' : '✓ Fresh'}
            </span>
          </div>
        `;

        marker.bindPopup(popupContent);
        marker.addTo(map.current);
        markers.current.set(markerId, marker);
      }
    });

    // Remove markers for clients that are no longer present
    const clientIds = new Set(clients.map((c) => `client-${c.id}`));
    markers.current.forEach((marker, markerId) => {
      if (markerId.startsWith('client-') && !clientIds.has(markerId)) {
        map.current?.removeLayer(marker);
        markers.current.delete(markerId);
      }
    });
  }, [clients, mapReady]);

  // Create simple POI marker for base stations (orange circle with icon)
  const createBaseStationMarker = () => {
    return L.divIcon({
      html: `<div style="
        background: linear-gradient(135deg, #FF9800 0%, #F57C00 100%);
        border: 3px solid white;
        border-radius: 50%;
        width: 32px;
        height: 32px;
        display: flex;
        align-items: center;
        justify-content: center;
        font-size: 18px;
        box-shadow: 0 2px 8px rgba(0,0,0,0.3);
        cursor: pointer;
      ">📡</div>`,
      iconSize: [32, 32],
      iconAnchor: [16, 16],
      popupAnchor: [0, -16],
      className: 'base-station-marker'
    });
  };

  // Add source markers
  useEffect(() => {
    if (!map.current || !mapReady) return;

    sources.forEach((source) => {
      const markerId = `source-${source.id}`;
      const latlng: LatLng = L.latLng(source.latitude, source.longitude);

      if (!markers.current.has(markerId) && map.current) {
        // Create base station marker for sources
        const icon = createBaseStationMarker();
        const marker = L.marker(latlng, { icon, title: source.name });

        const popupContent = `
          <div style="font-size: 12px;">
            <strong>📡 ${source.name}</strong><br />
            Base Station<br />
            Lat: ${source.latitude.toFixed(6)}<br />
            Lon: ${source.longitude.toFixed(6)}
          </div>
        `;

        marker.bindPopup(popupContent);
        marker.addTo(map.current);
        markers.current.set(markerId, marker);
      }
    });

    // Remove source markers that are no longer present
    const sourceIds = new Set(sources.map((s) => `source-${s.id}`));
    markers.current.forEach((marker, markerId) => {
      if (markerId.startsWith('source-') && !sourceIds.has(markerId)) {
        map.current?.removeLayer(marker);
        markers.current.delete(markerId);
      }
    });
  }, [sources, mapReady]);

  // Auto-fit bounds when clients or sources change
  useEffect(() => {
    if (!map.current || !mapReady) return;

    const allPositions = [
      ...clients.map((c) => [c.latitude, c.longitude] as [number, number]),
      ...sources.map((s) => [s.latitude, s.longitude] as [number, number]),
    ];

    if (allPositions.length > 0) {
      const bounds = L.latLngBounds(allPositions);
      map.current.fitBounds(bounds, { padding: [50, 50], maxZoom: 16 });
    }
  }, [clients, sources, mapReady]);

  return (
    <div className={styles.container}>
      <div ref={mapContainer} className={styles.map} />
      {clients.length === 0 && sources.length === 0 && (
        <div className={styles.emptyState}>
          <p>No clients or sources connected yet</p>
        </div>
      )}
    </div>
  );
};

export default RealTimeMap;
