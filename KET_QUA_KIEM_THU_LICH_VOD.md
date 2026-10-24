# Kết quả nghiệm thu lịch phát sóng và VOD

Ngày: 24/10/2026. Người thực hiện phần lịch: Nguyễn Hồng Anh.
Checkout: `TV36-hong-anh-vod-schedule`, nhánh `hong-anh/vod-schedule`, nền `ecbd02f`.

## Kết quả

- Build nền và build sau tích hợp: **PASS**, GCC C++17, 26 file nguồn của dự án; không có chẩn đoán lỗi trong build log.
- Bộ nghiệm thu: **55/55 mục PASS**. Trong đó mục entity chạy **26 kiểm tra C++** về ngày giờ, overflow, parse, round-trip và bảo toàn đối tượng khi dòng lỗi.
- Đồ thị dữ liệu mẫu: **PASS**. 31 lịch gồm 28 lịch TV giữ nguyên nội dung và ba lịch VOD mới; 20 VOD; mã duy nhất, tham chiếu tồn tại, không trùng cùng kênh, thời lượng lịch VOD khớp.
- Counter lịch: 31; counter VOD: giữ 26 (không hạ về 20). Dữ liệu/counter VOD và lịch trong Data_backup khớp Data.
- Hash Data nguồn trước/sau kiểm thử không đổi. Các ca CRUD chạy trên bản sao tạm.

## Các nhóm đã kiểm tra

| Nhóm | Bằng chứng chính |
|---|---|
| CRUD | Thêm TV/VOD, mở lại theo ID, sửa giữ nguyên, đổi VOD/bắt đầu, chuyển TV↔VOD, hủy, xóa n/y |
| VOD | Chọn đúng ACTIVE/HOURLY; từ chối không tồn tại, NON_HOURLY, INACTIVE |
| Tra cứu | Hai lịch VOD014 đúng thứ tự; phân biệt tự do/chưa xếp lịch/không tồn tại; lọc ngày qua đêm, kênh, sắp xếp |
| Trùng lịch | Trùng một phần, toàn bộ, nằm trong/bao ngoài, qua đêm, TV↔VOD; giáp ranh và khác kênh hợp lệ; sửa không tự trùng |
| Thời gian | Qua ngày/tháng/năm, năm nhuận 1900/2000/2024, nhiều ngày, cận 2100, thời lượng 0/âm/rất lớn và ngày giờ sai |
| Ràng buộc ngược | Chặn xóa/đổi loại/đổi thời lượng khi có lịch; chấp nhận thời lượng giữ nguyên; cho sửa khi gỡ hết lịch; giữ kiểm tra gói/kênh |
| Lịch sử | VOD/kênh ngừng hoạt động vẫn giữ/xem/xóa lịch cũ; chặn xếp lại |
| Lưu bền vững | Đọc lịch 6/7 trường và trường cuối rỗng; backup đọc được; counter thấp không tạo ID lịch trùng |
| Ghi thất bại | Windows file chỉ đọc: thêm/sửa/xóa lịch và sửa VOD báo lỗi, file gốc nguyên vẹn, dọn file tạm |

## Cách chạy và xem bằng chứng

- Mở `CHAY_LICH_VOD.bat` để chạy bản đã biên dịch. Launcher đặt thư mục làm việc về gốc dự án.
- Hoặc build bằng `build.bat` của nhóm, rồi chạy `main.exe` từ gốc dự án. File `main.exe` có sẵn từ nhóm chưa được thay thế; bản mới ở `build/TV36.exe`.
- Hướng dẫn đặc tả/cơ chế/demo: `HUONG_DAN_LICH_VOD_HONG_ANH.md`.
- Kết quả máy đọc: `build/verification/results.json`; mỗi ca có `.input.txt`, `.log` và Data thử riêng. Build log trong cùng thư mục.
- Chạy lại: `python tests/test_schedule_vod.py --exe build/TV36.exe --output build/verification-new` (chọn thư mục kết quả chưa tồn tại).

## Giới hạn

Chưa thêm phát video thật hoặc tính cước. Không đổi cơ chế loader dùng chung của nhóm: dòng dữ liệu hỏng vẫn bị cảnh báo/bỏ qua; bộ kiểm thử không chứng minh mọi lỗi của các module khác đã được sửa. Kiểm thử I/O được tái hiện bằng file chỉ đọc trên Windows; chưa giả lập mất điện hoặc mọi dạng lỗi phần cứng.

Mã nguồn bàn giao trên nhánh `hong-anh/vod-schedule`.
