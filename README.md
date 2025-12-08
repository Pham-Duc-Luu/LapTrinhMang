Xây dựng ứng dụng mua bán vé xem phim online
"Hệ cho phép người dùng lựa chọn và mua vé xem phim online đang chiếu trên một hệ thống phòng vé, sử dụng socket với giao diện text. Hệ thống cũng cần cho phép quản lý việc đưa phim lên hệ thống bán vé. Lưu trữ thông tin film tai server, tạo giao diện client cho 2 đối tượng người mua và quản lý. Các chức năng cần có:
Chức năng cho người mua:

- Tạo tài khoản
- Đăng nhập
- Tìm kiếm phim theo tên
- Duyệt danh mục phim theo thể loại, trung tâm chiếu, khung giờ.
- Đặt vé cho một phim cụ thể, xem bản đồ ghế trống, chọn ghế. Có thể chọn nhiều ghế.

Chức năng cho người quản lý bán hàng

- Thêm phim mới: tiêu đề, mô tả, thời lượng chiếu,
- Công bố chiếu phim: xác định các rạp sẽ chiếu, khung giờ cho mỗi rạp
- Thay đổi thông tin công chiếu: sửa đổi các suất chiếu.
  Chức năng admin:
- Tạo tài khoản, phân quyền"

"Yêu cầu:

- Kết nối client server bằng socket (2 điểm)
- Ghi log các message nhận được và gửi đi bên server (1 điểm)
- Tạo tài khoản người dùng (1 điểm)
- Đăng nhập (1 điểm)
- Tìm kiếm phim theo tên (2 điểm)
- Duyệt danh mục phim theo thể loại, trung tâm chiếu, khung giờ. (2 điểm)
- Đặt vé cho một phim cụ thể, xem bản đồ ghế trống, chọn ghế. Có thể chọn nhiều ghế. (3 điểm)
- Thêm phim mới: tiêu đề, mô tả, thời lượng chiếu (2 điểm)
- Công bố chiếu phim: xác định các rạp sẽ chiếu, khung giờ cho mỗi rạp (2 điểm)
- Thay đổi thông tin công chiếu: sửa đổi các suất chiếu. (2 điểm)
- Chức năng Tạo tài khoản, phân quyền của admin (2 điểm)"

Cấu trúc user :

- username
- password
- role: user hoặc admin

Cấu trúc movie :

- name
- description
- duration
- categorys
