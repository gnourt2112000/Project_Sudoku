# FunGame

Project Môn học thực hành lập trình mạng, được viết bằng C.
FunGame gồm 2 game nhỏ là caro và tictactoe.Cả 2 trò chơi, người chơi sẽ chơi trực tiếp với máy.

**Chức năng của Chương trình**:
  - Đăng ký
  - Đăng nhập
  - Chơi game caro
  - Xem log (lịch sử chơi)
  - Xem bảng xếp hạng và thông tin người chơi game caro
  - Chơi game tictactoe
  - Xem bảng xếp hạng và thông tin người chơi game tictactoe
---
**Công nghệ sử dụng**:
  - Sử dụng thư viện termios.h để ẩn mật khẩu khi nhập
  - Sử dụng thuật toán Minimax trong game (dùng để máy đánh)
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
**Video demo và báo cáo**
* Video:  [Click](https://www.youtube.com/watch?v=_deGQKfb5F8)
* Slide + Report:  [Click](https://github.com/nguyenvd27/project-network-programming-fun-game/tree/master/Slide%2BReport)
