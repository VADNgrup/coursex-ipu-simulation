import os
import pandas as pd
import matplotlib
matplotlib.use('Agg')  # non-interactive backend
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

LOG_FILE = './.tmp/cafeteria_log.csv'
DASH_FILE = './.tmp/cafeteria_dashboard.csv'


def infer_ts_unit(series):
    val = series.dropna().iloc[0]
    if val < 1e10:   return 's'
    elif val < 1e13: return 'ms'
    elif val < 1e16: return 'us'
    else:            return 'ns'


def fmt_time_axis(ax):
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    ax.xaxis.set_major_locator(mdates.AutoDateLocator(minticks=2, maxticks=8))
    plt.setp(ax.xaxis.get_majorticklabels(), rotation=30, ha='right')


def plot_metrics():
    if not os.path.exists(LOG_FILE) or os.stat(LOG_FILE).st_size == 0:
        print(f'No data found in {LOG_FILE}.')
        return

    df = pd.read_csv(LOG_FILE)
    if df.empty:
        print('Log file is empty.')
        return

    log_unit = infer_ts_unit(df['timestamp'])
    df['datetime'] = pd.to_datetime(df['timestamp'], unit=log_unit)

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Cafeteria Simulation Performance Metrics', fontsize=16, fontweight='bold')

    colors = ['#4C72B0', '#55A868', '#C44E52', '#8172B2', '#CCB974']
    queues = sorted(df['queue'].unique())

    # --- Figure 1: Waiting Time Distribution (box plot) ---
    wait_data = [df[df['queue'] == q]['wait_time'].dropna().values for q in queues]
    bp1 = axes[0, 0].boxplot(wait_data, tick_labels=queues, patch_artist=True,
                              medianprops=dict(color='black', linewidth=2))
    for patch, color in zip(bp1['boxes'], colors):
        patch.set_facecolor(color); patch.set_alpha(0.7)
    axes[0, 0].set_title('Waiting Time Distribution (seconds)')
    axes[0, 0].set_ylabel('Seconds')
    axes[0, 0].set_xlabel('Queue')
    axes[0, 0].grid(axis='y', linestyle='--', alpha=0.7)

    # --- Figure 2: Processing Time Distribution (box plot) ---
    proc_data = [df[df['queue'] == q]['proc_time'].dropna().values for q in queues]
    bp2 = axes[0, 1].boxplot(proc_data, tick_labels=queues, patch_artist=True,
                              medianprops=dict(color='black', linewidth=2))
    for patch, color in zip(bp2['boxes'], colors):
        patch.set_facecolor(color); patch.set_alpha(0.7)
    axes[0, 1].set_title('Service / Prep / Eating Time Distribution (seconds)')
    axes[0, 1].set_ylabel('Seconds')
    axes[0, 1].set_xlabel('Queue')
    axes[0, 1].grid(axis='y', linestyle='--', alpha=0.7)

    # --- Figure 3: Throughput per Minute ---
    # Use ax.plot() directly (not df.plot()) to avoid pandas/mpl date unit mismatch
    df.set_index('datetime', inplace=True)
    throughput = df.groupby('queue').resample('1min')['student_id'].count().unstack(level=0).fillna(0)
    if not throughput.empty:
        x_vals = throughput.index.to_pydatetime()
        for i, col in enumerate(throughput.columns):
            axes[1, 0].plot(x_vals, throughput[col].values,
                            marker='o', label=col, color=colors[i % len(colors)])
        axes[1, 0].set_title('Throughput: Students Processed per Minute')
        axes[1, 0].set_ylabel('Count')
        axes[1, 0].legend()
        axes[1, 0].grid(True, linestyle='--', alpha=0.7)
        fmt_time_axis(axes[1, 0])

    # --- Figure 4: Queue length over time ---
    if os.path.exists(DASH_FILE) and os.stat(DASH_FILE).st_size > 0:
        dash_df = pd.read_csv(DASH_FILE)
        if not dash_df.empty:
            dash_unit = infer_ts_unit(dash_df['timestamp'])
            dash_df['datetime'] = pd.to_datetime(dash_df['timestamp'], unit=dash_unit)
            for i, q_name in enumerate(dash_df['queue'].unique()):
                sub = dash_df[dash_df['queue'] == q_name]
                x_vals = sub['datetime'].dt.to_pydatetime()
                axes[1, 1].plot(x_vals, sub['current_length'].values,
                                label=q_name, color=colors[i % len(colors)])
            axes[1, 1].set_title('Queue Length Over Time')
            axes[1, 1].set_ylabel('Students in Queue')
            axes[1, 1].legend()
            axes[1, 1].grid(True, linestyle='--', alpha=0.7)
            fmt_time_axis(axes[1, 1])

    plt.tight_layout()
    output_path = 'cafeteria_metrics.png'
    plt.savefig(output_path, dpi=300)
    print(f'Metrics plot saved to {output_path}')


if __name__ == '__main__':
    plot_metrics()
