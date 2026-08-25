import numpy as np
import matplotlib.pyplot as plt

def generate_train_arrivals(total_time=300, mean_interarrival=20):
    """
    Sinh mốc thời gian tàu đến ga Takizawa trong khoảng [0, 300] phút.
    Sử dụng Phân phối Mũ (Exponential) liên tục trên toàn dải 300 phút.
    """
    arrivals = []
    current_time = 0
    
    # Sinh tàu liên tục cho đến khi vượt quá 300 phút (+ thêm buffer để có tàu đón người đến muộn)
    while current_time < total_time + 100:
        dt = np.random.exponential(scale=mean_interarrival) # dt ~ Exp(20 mins)
        current_time += dt
        arrivals.append(current_time)
        
    return np.array(arrivals)

def run_simulation(num_trials=10000, total_time=300, mean_interarrival=20):
    wait_times = []
    
    for _ in range(num_trials):
        trains = generate_train_arrivals(total_time, mean_interarrival)
        # Hành khách đến ngẫu nhiên trong khoảng [0, 300] phút
        passenger_time = np.random.uniform(0, total_time)
        
        # Tìm tàu đầu tiên đến SAU thời điểm hành khách xuất hiện
        future_trains = trains[trains >= passenger_time]
        if len(future_trains) > 0:
            next_train = future_trains[0]
            wait_times.append(next_train - passenger_time)
            
    return np.array(wait_times)

# ------------------ TRỰC QUAN HÓA & VẼ ĐỒ THỊ ------------------
np.random.seed(42) # Cố định seed để dễ so sánh kết quả
num_trials = 10000
wait_times = run_simulation(num_trials=num_trials)

fig, axes = plt.subplots(2, 1, figsize=(12, 10))

# 1. BIỂU ĐỒ 1: KỊCH BẢN MINH HỌA TIMELINE (BIG PICTURE)
example_trains = generate_train_arrivals(total_time=300)
example_trains_in_window = example_trains[example_trains <= 300]
example_passenger = np.random.uniform(50, 250)
next_train = example_trains[example_trains >= example_passenger][0]
example_wait = next_train - example_passenger

ax1 = axes[0]
ax1.hlines(0, 0, 300, colors='gray', linestyles='-', linewidth=2)
ax1.plot(example_trains_in_window, np.zeros_like(example_trains_in_window), 'ro', markersize=8, label='Tàu đến (Train arrival "t")')
ax1.plot(example_passenger, 0, 'b^', markersize=12, label=f'Khách đến (#) t = {example_passenger:.1f}m')
ax1.annotate('', xy=(next_train, 0.05), xytext=(example_passenger, 0.05),
             arrowprops=dict(arrowstyle='<->', color='purple', lw=2))
ax1.text((example_passenger + next_train)/2, 0.08, f'Wait Time = {example_wait:.1f} mins', 
         ha='center', fontsize=11, fontweight='bold', color='purple')

ax1.set_xlim(-10, 310)
ax1.set_ylim(-0.2, 0.2)
ax1.set_yticks([])
ax1.set_xlabel('Thời gian (Phút từ 0 đến 300)', fontsize=12)
ax1.set_title('Minh họa 1 Kịch bản Lịch trình Tàu & Thời gian chờ (Timeline Example)', fontsize=14, fontweight='bold')
ax1.legend(loc='upper right', fontsize=10)
ax1.grid(True, axis='x', linestyle='--', alpha=0.5)

# 2. BIỂU ĐỒ 2: PHÂN PHỐI THỜI GIAN CHỜ (WAIT TIME DISTRIBUTION)
ax2 = axes[1]
count, bins, patches = ax2.hist(wait_times, bins=50, density=True, alpha=0.7, color='teal', edgecolor='black')

mean_wait = np.mean(wait_times)
median_wait = np.median(wait_times)
max_wait = np.max(wait_times)

ax2.axvline(mean_wait, color='red', linestyle='dashed', linewidth=2, label=f'Chờ trung bình (Mean): {mean_wait:.1f} phút')
ax2.axvline(median_wait, color='orange', linestyle='dashed', linewidth=2, label=f'Trung vị (Median): {median_wait:.1f} phút')

ax2.set_title(f'Phân phối Thời gian Chờ Tàu tại Ga Takizawa ({num_trials:,} Lần mô phỏng Monte Carlo)', fontsize=14, fontweight='bold')
ax2.set_xlabel('Thời gian chờ (Phút)', fontsize=12)
ax2.set_ylabel('Mật độ xác suất (Density)', fontsize=12)
ax2.legend(fontsize=11)
ax2.grid(True, linestyle='--', alpha=0.5)

plt.tight_layout()
plt.show()

# In thống kê chi tiết
print("=== KẾT QUẢ MÔ PHỎNG MONTE CARLO ===")
print(f"Tổng số lần mô phỏng      : {num_trials:,}")
print(f"Thời gian chờ TRUNG BÌNH  : {mean_wait:.2f} phút")
print(f"Thời gian chờ TRUNG VỊ    : {median_wait:.2f} phút")
print(f"Thời gian chờ LỚN NHẤT    : {max_wait:.2f} phút")
print(f"Xác suất chờ > 30 phút    : {(np.sum(wait_times > 30) / num_trials)*100:.2f}%")