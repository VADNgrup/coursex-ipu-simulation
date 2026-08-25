#!/bin/bash

# Clean các file cũ[cite: 8]
rm -f makeshm microwave_engine controller test_timer test_door test_power test_sbutton

# Biên dịch các tiến trình mới
gcc makeshm.c -o makeshm
gcc microwave_engine.c -o microwave_engine
gcc controller.c -o controller
gcc test_timer.c -o test_timer
gcc test_door.c -o test_door
gcc test_power.c -o test_power
gcc test_sbutton.c -o test_sbutton
echo "Biên dịch thành công!"



### Hướng dẫn chạy thử nghiệm

# 1. **Biên dịch:**
# ```bash
# bash biendich.sh

# ```


# 2. **Khởi tạo vùng nhớ:**
# ```bash
# ./makeshm

# ```


# 3. **Mở Terminal 1 (Xem mô phỏng lò chạy):**
# ```bash
# ./microwave_engine

# ```


# 4. **Mở Terminal 2 (Điều khiển lò):**
# ```bash
# ./controller

# ```


# *Thử nhập `3` (+30s) -> `5` (START) ở Terminal 2 để xem lò bắt đầu đếm lùi và bật Emitter/Light ở Terminal 1.*