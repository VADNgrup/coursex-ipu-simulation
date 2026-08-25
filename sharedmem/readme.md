Ba file C trên thể hiện cơ chế giao tiếp giữa các tiến trình (**Inter-Process Communication - IPC**) trong hệ điều hành Linux/Unix thông qua **Bộ nhớ dùng chung (System V Shared Memory)** với cùng một khóa định danh `key = 1234`.

**Chức năng của từng file**

* **File 1 (Khởi tạo vùng nhớ):** Đóng vai trò khởi tạo. Chương trình sử dụng `shmget` với cờ `IPC_CREAT | 0666` để cấp phát một vùng nhớ dùng chung kích thước 1024 bytes mang key `1234`.


* **File 2 (Tiến trình Đọc / Consumer):** Kết nối vào vùng nhớ key `1234` thông qua `shmget` và `shmat`. Sau đó, tiến trình đi vào vòng lặp vô tận `while(1)` để liên tục đọc giá trị ký tự `*shm` tại ô nhớ đầu tiên và in ra màn hình.


* **File 3 (Tiến trình Ghi / Producer):** Kết nối vào vùng nhớ key `1234`. Tiến trình đi vào vòng lặp vô tận, yêu cầu người dùng nhập ký tự từ bàn phím (`scanf`), sau đó ghi trực tiếp ký tự đó vào ô nhớ đầu tiên `*shm`.



---

**Hướng dẫn biên dịch và chạy chương trình**

Cần thực hiện trên môi trường Linux/Unix hoặc WSL (Windows Subsystem for Linux).

**Bước 1: Lưu mã nguồn**
Lưu 3 đoạn mã vào 3 file riêng biệt: `create.c` (File 1), `read.c` (File 2), và `write.c` (File 3).

**Bước 2: Biên dịch các file**
Mở Terminal và chạy lệnh biên dịch bằng `gcc`:

```bash
gcc create.c -o create
gcc read.c -o read
gcc write.c -o write

```

*(Nếu compiler báo cảnh báo về hàm `main()` thiếu kiểu trả về, bạn có thể bổ sung `int main()` vào file C)*.

**Bước 3: Thực thi theo đúng thứ tự**

1. **Tạo vùng nhớ dùng chung (Chạy file 1):**
```bash
./create

```


*(Lệnh này tạo xong vùng nhớ `1234` rồi kết thúc ngay)*.


2. **Lắng nghe dữ liệu (Chạy file 2 trên Terminal thứ 1):**
```bash
./read

```


Terminal sẽ bắt đầu in liên tục ký tự rác hiện có trong vùng nhớ.


3. **Ghi dữ liệu (Chạy file 3 trên Terminal thứ 2):**
```bash
./write

```


Nhập một ký tự bất kỳ và nhấn Enter. Bạn sẽ thấy Terminal 1 lập tức cập nhật và in ra ký tự mới vừa nhập.



---

**Lưu ý kỹ thuật quan trọng**

* **Chạy sai thứ tự:** Nếu chạy `./read` hoặc `./write` trước khi chạy `./create`, chương trình sẽ báo lỗi `shmget error` vì vùng nhớ key `1234` chưa tồn tại.


* **Tải CPU cao:** Vòng lặp `while(1)` trong `read.c` chạy liên tục không có thời gian nghỉ (`sleep`), làm CPU chạy ở mức 100% trên core đó.


* **Quản lý Shared Memory:**
* Xem các vùng nhớ đang hoạt động: `ipcs -m`
* Xóa vùng nhớ thủ công sau khi thử nghiệm xong: `ipcrm -m <shmid>` hoặc `ipcrm -M 1234`