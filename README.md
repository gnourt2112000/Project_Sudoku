# Game

Project Môn học thực hành lập trình mạng, được viết bằng C.

**Chức năng của Chương trình**:
  - Đăng ký
  - Đăng nhập
  - Chơi game sudoku
  - Xem log (lịch sử chơi)
  - Xem bảng xếp hạng và thông tin người chơi 
---
**Công nghệ sử dụng**:
  - Sử dụng thư viện termios.h để ẩn mật khẩu khi nhập
  - Sử dụng thuật toán backtracking để giải sudoku
  - Kết nối bằng giao thức TCP
  - Nhiều client cùng kết nối với server sử dụng select()
---
**Hướng dẫn cài đặt**:
1. Clone repo từ github.
2. Mở Terminal và gõ `make` để biên dịch chương trình.
3. Mở 2 cửa sổ Terminal, 1 cửa sổ là server, 1 cửa sổ là client.
4. Với Server gõ lệnh: `./server PORT_NUMBER`, ví dụ: `./server 5500`
4. Với Client gõ lệnh: `./client SERVER_ADDRESS PORT_NUBER`, ví dụ: `./client 127.0.0.1 5500`
---
