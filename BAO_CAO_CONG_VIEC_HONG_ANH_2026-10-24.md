# Báo cáo công việc đã thực hiện — Nguyễn Hồng Anh

Ngày tổng hợp: 24/10/2026. Dự án: TV36 — quản lý dịch vụ truyền hình số/IPTV.
Nhánh bàn giao: `hong-anh/vod-schedule`, phát triển từ `main` tại commit `ecbd02f`.

## 1. Yêu cầu đã thực hiện

Theo góp ý sau bảo vệ: giữ VOD xem tự do và bổ sung VOD có lịch phát sóng; lịch phải hiển thị và tra được VOD. Tái sử dụng hai giá trị `HOURLY`/`NON_HOURLY` do Nguyễn Khắc Hiếu bổ sung.

Đã giữ lịch chương trình truyền hình thông thường và thêm lịch VOD trên cùng hệ thống kênh. Không thay dữ liệu hoặc tài liệu của checkout cũ `TV36`; toàn bộ công việc nằm trong checkout riêng `TV36-hong-anh-vod-schedule`.

## 2. Những phần đã làm trong module lịch của Hồng Anh

### Mô hình Schedule

- Thêm thuộc tính `vodID`, getter/setter; mã rỗng là lịch TV, có mã là lịch VOD.
- Constructor vẫn nhận được cách gọi cũ vì tham số `vodID` cuối có mặc định rỗng.
- Parser đọc được bản ghi cũ 6 trường và mới 7 trường, kể cả mô tả hoặc mã VOD rỗng cuối dòng.
- Khi ghi, sử dụng `id|channelID|programName|startTime|endTime|description|vodID`.
- Dựng đối tượng tạm khi parse, không làm đổi bản ghi hiện tại nếu dòng sai.
- Tên chương trình VOD được chụp lúc chọn VOD; đổi tên phim sau này không tự sửa tên lịch đã lưu.

File: `Entities/Schedule.h`, `Entities/Schedule.cpp`.

### Thêm, sửa, xem và xóa lịch

- Thêm lựa chọn TV hoặc VOD khi tạo/sửa; hỗ trợ chuyển TV sang VOD và ngược lại.
- Chọn VOD từ danh sách đang `ACTIVE` và `HOURLY`.
- Chỉ nhập giờ bắt đầu cho lịch VOD; giờ kết thúc tự tính bằng thời lượng video theo phút.
- Cộng giờ đúng qua nửa đêm, cuối tháng, cuối năm và năm nhuận; từ chối kết quả ngoài năm 1900–2100.
- Đổi VOD hoặc giờ bắt đầu sẽ tính lại giờ kết thúc. Giữ nguyên thông tin thì giữ khung giờ đã lưu.
- Bảng hiển thị loại TV/VOD, mã VOD, kênh, tên chương trình và hai mốc giờ.
- Bổ sung tra lịch theo mã VOD, kết quả tăng dần theo giờ bắt đầu. Phân biệt VOD không tồn tại, VOD xem tự do và VOD chưa có lịch.
- Giữ tìm theo ID lịch, lọc kênh/ngày và sắp xếp; chi tiết có đầy đủ nội dung bị rút gọn trên bảng.
- Enter giữ giá trị cũ; mô tả `-` để xóa; nhập `0` tại luồng điền lịch để hủy.
- Sửa trên bản nháp, xóa có xác nhận y/n, chỉ báo thành công sau khi lưu được.

File: `UseCases/ScheduleMenu.h`, `UseCases/ScheduleMenu.cpp`.

### Kiểm tra nghiệp vụ

- Chặn trùng trên cùng kênh giữa mọi loại lịch: trùng một phần, toàn bộ, nằm trong nhau và qua ngày.
- Cho phép giáp ranh, khác kênh; sửa không tự trùng chính ID.
- Tạo hoặc đổi kênh/khung giờ yêu cầu kênh đang phát. Xếp hoặc đổi lịch VOD yêu cầu VOD ACTIVE/HOURLY.
- Một VOD có nhiều lần phát; HOURLY được phép tồn tại trước khi có lịch.
- Lịch cũ vẫn giữ, xem và xóa được khi kênh/VOD ngừng hoạt động.
- Giữ kiểm tra sẵn có: không xóa kênh đang có lịch.

## 3. Phần tích hợp bổ sung vào module VOD của Hiếu

Đây là các sửa đổi tối thiểu để bảo vệ quan hệ VOD–Schedule; model hình thức xem vẫn dùng nguyên của Hiếu.

- Thêm repository lịch và hàm đọc lại các mã lịch đang tham chiếu VOD.
- Chặn đổi thời lượng khi còn bất kỳ lịch tham chiếu nào, nhưng cho phép nhập lại đúng thời lượng cũ.
- Chặn chuyển sang NON_HOURLY hoặc xóa khi còn lịch; thông báo các mã lịch liên quan.
- Sau khi gỡ hết lịch tham chiếu, cho đổi thời lượng/hình thức xem; xóa vẫn cần thỏa ràng buộc gói cước vốn có.
- Cho phép sửa tên, mô tả, URL, trạng thái và các thông tin không thay khung giờ.
- Cập nhật dùng bản nháp, kiểm tra lại trước cam kết, khôi phục dữ liệu trong bộ nhớ khi lưu lỗi.
- Đổi thông báo chấp nhận từng trường để không báo đã lưu thành công trước khi ghi file.

File: `UseCases/NoiDungVODMenu.h`, `UseCases/NoiDungVODMenu.cpp`.

## 4. Lưu file an toàn

Thêm `Utils/LinkedDataWriter.h`, dùng cho lịch và các thao tác lưu VOD:

1. Ghi dữ liệu sang file tạm trong cùng thư mục.
2. Kiểm tra ghi, flush và close.
3. Chỉ thay file chính khi file tạm hoàn chỉnh; Windows dùng `MoveFileExW`.
4. Nếu thất bại, giữ file chính, dọn file tạm và báo lỗi; menu khôi phục trạng thái bộ nhớ.

Không sửa framework Repository dùng chung của các module còn lại. Loader hiện có của nhóm vẫn cảnh báo/bỏ dòng dữ liệu hỏng.

## 5. Dữ liệu mẫu và counter

- Giữ nội dung 28 lịch TV hiện có, bổ sung cột `vodID` rỗng.
- Thêm `LPS029`: VOD014, 24/10/2026 20:00–22:30.
- Thêm `LPS030`: phát lại VOD014, 25/10/2026 10:00–12:30.
- Thêm `LPS031`: VOD019, 25/10/2026 23:00–26/10/2026 01:40.
- Tổng 31 lịch; counter lịch 31. Counter VOD giữ 26, không hạ xuống 20.
- Đồng bộ VOD, lịch và hai counter tương ứng giữa `Data` và `Data_backup`.
- Kiểm tra mã duy nhất, tham chiếu đúng, không trùng giờ và thời lượng lịch VOD khớp nội dung.

## 6. Kiểm thử đã thực hiện

- Build toàn bộ dự án C++17 trước và sau sửa: thành công.
- **55/55 mục nghiệm thu đạt**, trong đó nhóm entity chạy **26 kiểm tra C++** về ngày giờ, parser và round-trip.
- Bao phủ CRUD hai loại lịch, chuyển loại, giữ nguyên/hủy, mở lại sau lưu, tra cứu, chống trùng, năm nhuận, vượt năm 2100, ràng buộc VOD, kênh/gói, đọc dữ liệu cũ và backup.
- File chỉ đọc trên Windows: thêm/sửa/xóa lịch và sửa VOD báo lỗi, file chính giữ nguyên, không sót file tạm.
- Các thao tác kiểm thử chạy trên bản sao; hash Data nguồn trước/sau kiểm thử không đổi.

Bộ kiểm thử: `tests/test_schedule_vod.py`. Báo cáo chi tiết: `KET_QUA_KIEM_THU_LICH_VOD.md`. Log/bản chạy cục bộ nằm ở `build/verification/` và `build/TV36.exe`; thư mục build được Git bỏ qua. Không coi các kiểm thử này là bằng chứng mọi module khác của nhóm đã hoàn thiện.

## 7. Cách chạy và tài liệu đi kèm

- Build từ source bằng `build.bat`, rồi chạy `main.exe` ở thư mục gốc dự án.
- Trên máy đã có bản build của lần bàn giao, mở `CHAY_LICH_VOD.bat` để chạy `build/TV36.exe`.
- Hướng dẫn đặc tả, giải thích cơ chế và demo từng bước: `HUONG_DAN_LICH_VOD_HONG_ANH.md`.
- Kiểm thử lại: `python tests/test_schedule_vod.py --exe build/TV36.exe --output build/verification-new` (cần có executable đã build và thư mục kết quả mới).

Không bổ sung phát video thật hoặc tính cước. Những giới hạn này thuộc phạm vi của bài quản lý thông tin.
