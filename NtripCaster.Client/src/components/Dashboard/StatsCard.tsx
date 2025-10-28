import React from 'react';
import styles from './StatsCard.module.css';

interface StatsCardProps {
  title: string;
  value: string | number;
  subtitle?: string;
  icon?: string;
  trend?: 'up' | 'down' | 'stable';
  trendValue?: string;
  color?: 'blue' | 'green' | 'orange' | 'red';
}

export const StatsCard: React.FC<StatsCardProps> = ({
  title,
  value,
  subtitle,
  icon,
  trend,
  trendValue,
  color = 'blue',
}) => {
  return (
    <div className={`${styles.card} ${styles[color]}`}>
      <div className={styles.header}>
        <h3 className={styles.title}>{title}</h3>
        {icon && <span className={styles.icon}>{icon}</span>}
      </div>

      <div className={styles.value}>{value}</div>

      {subtitle && <div className={styles.subtitle}>{subtitle}</div>}

      {trend && (
        <div className={`${styles.trend} ${styles[trend]}`}>
          <span className={styles.trendIcon}>
            {trend === 'up' && '↑'}
            {trend === 'down' && '↓'}
            {trend === 'stable' && '→'}
          </span>
          {trendValue && <span className={styles.trendText}>{trendValue}</span>}
        </div>
      )}
    </div>
  );
};

export default StatsCard;
