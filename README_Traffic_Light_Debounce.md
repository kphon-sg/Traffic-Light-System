# 🛠️ Traffic Light System – Button Debounce (LPC1768, Keil C)

## 📌 Giới thiệu

Dự án này mô phỏng một hệ thống đèn giao thông đơn giản sử dụng vi điều khiển **LPC1768**, gồm:
- Đèn LED mô phỏng đèn giao thông (Đỏ, Vàng, Xanh)
- Nút nhấn để chuyển trạng thái thủ công
- Timer interrupt để tự động đổi đèn mỗi vài giây
- Xử lý **debounce nút nhấn** để tránh nhiễu tín hiệu

---

## ⚙️ Phần cứng sử dụng
- Vi điều khiển **LPC1768 (ARM Cortex-M3)**
- 3 LED nối ra các chân **P2.0, P2.1, P2.2**
- Nút nhấn nối vào **P2.10**, dùng active-low
- Clock hệ thống: **25 MHz**

---

## 🧠 Các thành phần chính trong phần mềm

| Thành phần      | Mô tả |
|-----------------|-------|
| `setLED()`      | Bật LED tương ứng với trạng thái đèn |
| `TIMER0_IRQHandler()` | Ngắt timer mỗi 1 giây để tự động đổi trạng thái sau 5s |
| `EINT3_IRQHandler()` | Xử lý khi nhấn nút, bao gồm debounce |
| `delay_ms()`    | Hàm tạo trễ đơn giản để debounce nút nhấn |

---

## 🧪 Cơ chế hoạt động

- Hệ thống bắt đầu với đèn **đỏ sáng**
- Mỗi **5 giây**, đèn tự động chuyển sang màu tiếp theo
- Khi **nhấn nút**, hệ thống lập tức chuyển sang trạng thái đèn tiếp theo
- Dùng **debounce software** để lọc rung nút nhấn:
  ```c
  delay_ms(DEBOUNCE_MS);
  ```

---

## 🔧 Thông số cấu hình

| Biến | Vai trò |
|------|--------|
| `SYSTEM_CLOCK` = 25000000 | Tần số hệ thống (25 MHz) |
| `AUTO_CHANGE_INTERVAL` = 5 | Đổi đèn sau 5 giây |
| `DEBOUNCE_MS` = 50 | Thời gian chống nhiễu nút nhấn |

---

## 📌 Kết luận

Dự án đơn giản này giúp bạn làm quen với:
- Sử dụng **interrupts** trong vi điều khiển
- Xử lý nút nhấn có **debounce**
- Làm việc với **LED logic**
- Sử dụng **timer và ngắt ngoài (EINT3)**

**Có thể mở rộng**:
- Giao diện LCD hiển thị trạng thái
- Âm thanh cảnh báo
- Giao tiếp UART để log trạng thái về máy tính

---

**Người thực hiện:** *[Điền tên bạn tại đây]*  
**Liên hệ:** *[Email hoặc LinkedIn nếu cần]*