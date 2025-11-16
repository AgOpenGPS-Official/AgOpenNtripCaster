import React, { useState, useEffect, useRef } from 'react';
import {
  getContainers,
  getContainerLogs,
  downloadContainerLogs,
  type ContainerInfo,
  type LogLine,
} from '../../services/dockerLogsApi';
import styles from './ContainerLogsViewer.module.css';

export const ContainerLogsViewer: React.FC = () => {
  const [containers, setContainers] = useState<ContainerInfo[]>([]);
  const [selectedContainer, setSelectedContainer] = useState<string>('backend');
  const [logs, setLogs] = useState<LogLine[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [autoScroll, setAutoScroll] = useState(true);
  const [isPaused, setIsPaused] = useState(false);
  const [tailLines, setTailLines] = useState(100);
  const [levelFilter, setLevelFilter] = useState<string>('all');

  const logsEndRef = useRef<HTMLDivElement>(null);
  const logsContainerRef = useRef<HTMLDivElement>(null);

  // Load available containers on mount
  useEffect(() => {
    const loadContainers = async () => {
      try {
        const containerList = await getContainers();
        setContainers(containerList);
      } catch (err) {
        console.error('Failed to load containers:', err);
        setError('Failed to load containers');
      }
    };

    loadContainers();
  }, []);

  // Load logs when container or tail lines change
  useEffect(() => {
    if (!isPaused) {
      loadLogs();
    }
  }, [selectedContainer, tailLines, isPaused]);

  // Auto-refresh logs every 5 seconds if not paused
  useEffect(() => {
    if (isPaused) return;

    const interval = setInterval(() => {
      loadLogs();
    }, 5000);

    return () => clearInterval(interval);
  }, [selectedContainer, tailLines, isPaused]);

  // Auto-scroll to bottom when new logs arrive
  useEffect(() => {
    if (autoScroll && logsEndRef.current) {
      logsEndRef.current.scrollIntoView({ behavior: 'smooth' });
    }
  }, [logs, autoScroll]);

  const loadLogs = async () => {
    setLoading(true);
    setError(null);

    try {
      const response = await getContainerLogs(selectedContainer, tailLines);
      setLogs(response.lines);
    } catch (err) {
      console.error('Failed to load logs:', err);
      setError('Failed to load container logs');
    } finally {
      setLoading(false);
    }
  };

  const handleDownload = async () => {
    try {
      await downloadContainerLogs(selectedContainer, 1000);
    } catch (err) {
      console.error('Failed to download logs:', err);
      setError('Failed to download logs');
    }
  };

  const handleClear = () => {
    setLogs([]);
  };

  const togglePause = () => {
    setIsPaused(!isPaused);
  };

  const toggleAutoScroll = () => {
    setAutoScroll(!autoScroll);
  };

  const getLogLevelClass = (level: string): string => {
    switch (level.toUpperCase()) {
      case 'ERROR':
        return styles.logError;
      case 'WARNING':
        return styles.logWarning;
      case 'DEBUG':
        return styles.logDebug;
      default:
        return styles.logInfo;
    }
  };

  const filteredLogs = logs.filter((log) => {
    if (levelFilter === 'all') return true;
    return log.level.toUpperCase() === levelFilter.toUpperCase();
  });

  return (
    <div className={styles.container}>
      {/* Controls */}
      <div className={styles.controls}>
        <div className={styles.controlGroup}>
          <label htmlFor="container-select">Container:</label>
          <select
            id="container-select"
            value={selectedContainer}
            onChange={(e) => setSelectedContainer(e.target.value)}
            className={styles.select}
          >
            {containers.map((container) => (
              <option key={container.id} value={container.id}>
                {container.displayName}
              </option>
            ))}
          </select>
        </div>

        <div className={styles.controlGroup}>
          <label htmlFor="level-filter">Level:</label>
          <select
            id="level-filter"
            value={levelFilter}
            onChange={(e) => setLevelFilter(e.target.value)}
            className={styles.select}
          >
            <option value="all">All</option>
            <option value="ERROR">Error</option>
            <option value="WARNING">Warning</option>
            <option value="INFO">Info</option>
            <option value="DEBUG">Debug</option>
          </select>
        </div>

        <div className={styles.controlGroup}>
          <label htmlFor="tail-lines">Lines:</label>
          <select
            id="tail-lines"
            value={tailLines}
            onChange={(e) => setTailLines(Number(e.target.value))}
            className={styles.select}
          >
            <option value={50}>50</option>
            <option value={100}>100</option>
            <option value={200}>200</option>
            <option value={500}>500</option>
            <option value={1000}>1000</option>
          </select>
        </div>

        <div className={styles.buttonGroup}>
          <button
            onClick={togglePause}
            className={`${styles.button} ${isPaused ? styles.buttonWarning : ''}`}
            title={isPaused ? 'Resume' : 'Pause'}
          >
            {isPaused ? '▶️ Resume' : '⏸️ Pause'}
          </button>

          <button onClick={handleClear} className={styles.button} title="Clear logs">
            🗑️ Clear
          </button>

          <button onClick={handleDownload} className={styles.button} title="Download logs">
            ⬇️ Download
          </button>

          <button
            onClick={toggleAutoScroll}
            className={`${styles.button} ${autoScroll ? styles.buttonActive : ''}`}
            title={autoScroll ? 'Disable auto-scroll' : 'Enable auto-scroll'}
          >
            {autoScroll ? '📜 Auto-scroll: ON' : '📜 Auto-scroll: OFF'}
          </button>
        </div>
      </div>

      {/* Error Message */}
      {error && (
        <div className={styles.error}>
          ⚠️ {error}
        </div>
      )}

      {/* Log Display */}
      <div
        ref={logsContainerRef}
        className={styles.logsContainer}
      >
        {loading && logs.length === 0 ? (
          <div className={styles.loading}>Loading logs...</div>
        ) : filteredLogs.length === 0 ? (
          <div className={styles.empty}>
            No logs available. {isPaused && '(Paused)'}
          </div>
        ) : (
          <div className={styles.logLines}>
            {filteredLogs.map((log, index) => (
              <div key={index} className={`${styles.logLine} ${getLogLevelClass(log.level)}`}>
                <span className={styles.logTimestamp}>
                  [{new Date(log.timestamp).toLocaleTimeString()}]
                </span>
                <span className={styles.logLevel}>[{log.level}]</span>
                <span className={styles.logMessage}>{log.message}</span>
              </div>
            ))}
            <div ref={logsEndRef} />
          </div>
        )}
      </div>

      {/* Status Bar */}
      <div className={styles.statusBar}>
        <span>
          Showing {filteredLogs.length} of {logs.length} lines
        </span>
        <span>
          {isPaused ? '⏸️ Paused' : '▶️ Live'} | Auto-refresh: {isPaused ? 'OFF' : '5s'}
        </span>
      </div>
    </div>
  );
};

export default ContainerLogsViewer;
