import numpy as np
import matplotlib.pyplot as plt

# Định nghĩa 4 hướng di chuyển cố định: East, West, South, North
DIRECTIONS = {
    'E': (1, 0),
    'W': (-1, 0),
    'S': (0, -1),
    'N': (0, 1)
}

class Agent:
    def __init__(self, x, y, view_range=1):
        self.x = x
        self.y = y
        self.direction = np.random.choice(['E', 'W', 'S', 'N'])
        self.view_range = view_range

    def move(self, grid_size):
        self.direction = np.random.choice(['E', 'W', 'S', 'N'])
        dx, dy = DIRECTIONS[self.direction]
        self.x = int(np.clip(self.x + dx, 0, grid_size - 1))
        self.y = int(np.clip(self.y + dy, 0, grid_size - 1))

    def can_see(self, other):
        # Tầm nhìn Chebyshev distance <= view_range (+1 ô xung quanh)
        return max(abs(self.x - other.x), abs(self.y - other.y)) <= self.view_range


def run_single_trial(strategy, grid_size=50, max_steps=10000, view_range=1):
    # CỐ ĐỊNH VỊ TRÍ KHỞI TẠO: Mẹ góc dưới-trái (0,0), Con góc trên-phải (49,49)
    mother = Agent(0, 0, view_range=view_range)
    child = Agent(grid_size - 1, grid_size - 1, view_range=view_range)

    m_path = [(mother.x, mother.y)]
    c_path = [(child.x, child.y)]

    for step in range(1, max_steps + 1):
        if mother.can_see(child) or child.can_see(mother):
            return step, m_path, c_path

        # Mẹ di chuyển
        mother.move(grid_size)
        m_path.append((mother.x, mother.y))

        # Con di chuyển (nếu ở chiến thuật cả 2 cùng di chuyển)
        if strategy == "both_move":
            child.move(grid_size)
        c_path.append((child.x, child.y))

    return max_steps, m_path, c_path


def run_and_plot_tracking(num_trials=500, grid_size=50, max_steps=10000, view_range=1):
    results = {}

    print(f"Đang chạy mô phỏng {num_trials} lần để tìm lượt Nhanh nhất & Lâu nhất...")
    for strat in ["child_stays", "both_move"]:
        best_run = (max_steps + 1, [], [])   # (steps, mother_path, child_path)
        worst_run = (-1, [], [])

        for _ in range(num_trials):
            steps, m_path, c_path = run_single_trial(strat, grid_size, max_steps, view_range)
            
            if steps < best_run[0]:
                best_run = (steps, m_path, c_path)
            if steps > worst_run[0]:
                worst_run = (steps, m_path, c_path)

        results[strat] = {"fastest": best_run, "slowest": worst_run}

    # ------------------ VẼ BẢNG 4 BIỂU ĐỒ TRACKING (2x2) ------------------
    fig, axes = plt.subplots(2, 2, figsize=(14, 13))

    plots_config = [
        ("child_stays", "fastest", axes[0, 0], "1. Con đứng yên - Nhanh nhất"),
        ("child_stays", "slowest", axes[0, 1], "2. Con đứng yên - Lâu nhất"),
        ("both_move", "fastest", axes[1, 0], "3. Cả 2 di chuyển - Nhanh nhất"),
        ("both_move", "slowest", axes[1, 1], "4. Cả 2 di chuyển - Lâu nhất")
    ]

    for strat, speed, ax, title in plots_config:
        steps, m_path, c_path = results[strat][speed]

        m_x, m_y = zip(*m_path)
        c_x, c_y = zip(*c_path)

        # Đường đi của Mẹ
        ax.plot(m_x, m_y, color='red', alpha=0.4, linewidth=1.2, label='Đường đi Mẹ')
        
        # Đường đi/Vị trí của Con
        if strat == "both_move":
            ax.plot(c_x, c_y, color='blue', alpha=0.4, linewidth=1.2, label='Đường đi Con')
        
        # Điểm bắt đầu
        ax.scatter(m_x[0], m_y[0], color='darkred', marker='o', s=100, zorder=5, label='Mẹ bắt đầu (0,0)')
        ax.scatter(c_x[0], c_y[0], color='darkblue', marker='s', s=100, zorder=5, label=f'Con bắt đầu ({grid_size-1},{grid_size-1})')

        # Điểm gặp nhau
        ax.scatter(m_x[-1], m_y[-1], color='green', marker='*', s=250, zorder=6, label=f'Gặp nhau (Bước {steps})')

        ax.set_xlim(-2, grid_size + 1)
        ax.set_ylim(-2, grid_size + 1)
        ax.set_title(f"{title}\nTổng số bước: {steps}", fontsize=12, fontweight='bold')
        ax.set_xlabel('Tọa độ X')
        ax.set_ylabel('Tọa độ Y')
        ax.grid(True, linestyle='--', alpha=0.4)
        ax.legend(loc='upper right', fontsize=8)

    plt.suptitle("MÔ PHỎNG QUỸ ĐẠO VỊ TRÍ MẸ VÀ CON (DỪNG TẠI KHU VỰC TÌM THẤY)", fontsize=15, fontweight='bold', y=0.98)
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()

# Thực thi
if __name__ == "__main__":
    run_and_plot_tracking(num_trials=500, grid_size=50, max_steps=10000, view_range=1)