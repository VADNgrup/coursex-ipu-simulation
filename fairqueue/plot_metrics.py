import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime

LOG_FILE = "./.tmp/cafeteria_log.csv"
DASH_FILE = "./.tmp/cafeteria_dashboard.csv"

def plot_metrics():
    if not os.path.exists(LOG_FILE) or os.stat(LOG_FILE).st_size == 0:
        print(f"No data found in {LOG_FILE}.")
        return

    # 1. Read event log
    df = pd.read_csv(LOG_FILE)
    if df.empty:
        print("Log file is empty.")
        return

    df['datetime'] = pd.to_datetime(df['timestamp'], unit='s')
    
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle("Cafeteria Simulation Performance Metrics", fontsize=16, fontweight='bold')

    # Figure 1: Average Waiting Time by Queue
    avg_wait = df.groupby('queue')['wait_time'].mean()
    avg_wait.plot(kind='bar', ax=axes[0, 0], color=['#4C72B0', '#55A868', '#C44E52'])
    axes[0, 0].set_title("Average Waiting Time (seconds)")
    axes[0, 0].set_ylabel("Seconds")
    axes[0, 0].grid(axis='y', linestyle='--', alpha=0.7)

    # Figure 2: Average Processing Time by Queue & Food
    avg_proc = df.groupby(['queue', 'food'])['proc_time'].mean().unstack()
    avg_proc.plot(kind='bar', ax=axes[0, 1])
    axes[0, 1].set_title("Average Service / Prep / Eating Time (seconds)")
    axes[0, 1].set_ylabel("Seconds")
    axes[0, 1].grid(axis='y', linestyle='--', alpha=0.7)

    # Figure 3: Throughput per Minute (Number of students processed per minute)
    df.set_index('datetime', inplace=True)
    throughput = df.groupby('queue').resample('1T')['student_id'].count().unstack(level=0).fillna(0)
    if not throughput.empty:
        throughput.plot(ax=axes[1, 0], marker='o')
        axes[1, 0].set_title("Throughput: Students Processed per Minute")
        axes[1, 0].set_ylabel("Count")
        axes[1, 0].grid(True, linestyle='--', alpha=0.7)

    # Figure 4: Queue length over time (if dashboard csv exists)
    if os.path.exists(DASH_FILE) and os.stat(DASH_FILE).st_size > 0:
        dash_df = pd.read_csv(DASH_FILE)
        if not dash_df.empty:
            dash_df['datetime'] = pd.to_datetime(dash_df['timestamp'], unit='s')
            for q_name in dash_df['queue'].unique():
                sub = dash_df[dash_df['queue'] == q_name]
                axes[1, 1].plot(sub['datetime'], sub['current_length'], label=q_name)
            axes[1, 1].set_title("Queue Length Over Time")
            axes[1, 1].set_ylabel("Students in Queue")
            axes[1, 1].legend()
            axes[1, 1].grid(True, linestyle='--', alpha=0.7)

    plt.tight_layout()
    output_path = "cafeteria_metrics.png"
    plt.savefig(output_path, dpi=300)
    print(f"Metrics plot saved to {output_path}")

if __name__ == "__main__":
    plot_metrics()
