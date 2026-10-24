# Lịch phát sóng và VOD — phần Nguyễn Hồng Anh

## Phạm vi và tác giả

Phần Hồng Anh: `Schedule`, `ScheduleMenu`, tính giờ kết thúc VOD, tra lịch, dữ liệu mẫu và kiểm thử.
Phần tích hợp vào module Hiếu: kiểm tra lịch trước khi đổi thời lượng, chuyển `NON_HOURLY` hoặc xóa VOD; cập nhật VOD qua bản nháp và lưu file an toàn. Model VOD, API `getHinhThucXem()` và hai giá trị `HOURLY`/`NON_HOURLY` giữ theo code của Nguyễn Khắc Hiếu.

Nhánh: `hong-anh/vod-schedule`, dựa trên `main` tại `ecbd02f`.

## Đặc tả UC05 — Quản lý lịch phát sóng

**Tác nhân:** người quản lý hệ thống IPTV. **Điều kiện:** đã có kênh; lịch VOD cần nội dung `ACTIVE`, `HOURLY` với thời lượng dương.

Một kênh có nhiều lịch. Một lịch liên kết tối đa một VOD. Một VOD có thể được phát nhiều lần, trên nhiều kênh. `NON_HOURLY` là xem tự do, không được xếp lịch; `HOURLY` là nội dung có thể xếp lịch, không phải tính phí theo giờ. VOD mới được tạo trước khi lập lịch.

| Thuộc tính | Ý nghĩa và ràng buộc |
|---|---|
| `id` | Mã tự sinh `LPSxxx`, không sửa |
| `channelID` | Kênh phải tồn tại; tạo/đổi vị trí phát yêu cầu `DANG_PHAT` |
| `programName` | Tên chương trình; lịch VOD chụp tên khi chọn VOD, không tự đổi theo tên VOD sau này |
| `startTime` | Ngày giờ hợp lệ `yyyy-mm-dd HH:mm`, năm 1900–2100 |
| `endTime` | Sau bắt đầu; lịch VOD tự cộng thời lượng phút |
| `description` | Cho phép rỗng, không chứa dấu phân cách hoặc xuống dòng |
| `vodID` | Rỗng: lịch TV thường; có mã: lịch VOD, tham chiếu `NoiDungVOD` |

### Luồng thêm

1. Menu chính chọn `5`, rồi `1. Add Schedule`.
2. Chọn `1. TV` hoặc `2. VOD`, chọn kênh.
3. TV: nhập tên chương trình, bắt đầu và kết thúc. VOD: chọn mã từ danh sách ACTIVE/HOURLY, nhập bắt đầu, xem giờ kết thúc được tính.
4. Hệ thống kiểm tra khung giờ và trùng lịch; nhập mô tả; chỉ sinh mã và báo thành công sau khi dữ liệu hợp lệ và lưu được.
5. Có thể nhập `0` tại các bước nhập lịch để hủy. Hủy không tạo bản ghi.

### Luồng xem và tìm kiếm

Menu lịch chọn `2`: xem tất cả, tìm ID lịch, lọc kênh, lọc ngày hoặc `5. Search schedules by VOD ID`.
Tra VOD cho kết quả theo giờ bắt đầu; phân biệt mã không tồn tại, `NON_HOURLY`, và `HOURLY` chưa có lịch.
Bảng có loại TV/VOD, mã VOD, kênh, tên và hai mốc giờ. Tìm ID lịch hiển thị thêm từng trường đầy đủ, kể cả nội dung dài bị rút gọn trên bảng.
Lọc ngày bao gồm chương trình qua nửa đêm. Khoảng `[start,end)` kết thúc đúng 00:00 không tính vào ngày sau.

### Luồng sửa/xóa

- Sửa dùng bản nháp. Enter giữ giá trị cũ; mô tả `-` để xóa, `0` để hủy.
- Đổi TV sang VOD: chọn VOD hợp lệ, tên lấy từ VOD, tự tính kết thúc. Đổi VOD sang TV: bỏ mã VOD, cho nhập tên và kết thúc.
- Đổi VOD hoặc giờ bắt đầu sẽ tính lại kết thúc. Giữ nguyên VOD/bắt đầu thì giữ khung giờ đã lưu và tên đã chụp.
- Lịch cũ vẫn xem, giữ nguyên và xóa được nếu kênh hoặc VOD đã ngừng hoạt động. Đổi kênh/VOD/giờ cần các đối tượng đang hoạt động.
- Xóa hỏi y/n; `n` giữ bản ghi. Sau khi xóa hết lịch tham chiếu mới có thể đổi thời lượng, chuyển NON_HOURLY hoặc xóa VOD (vẫn phải thỏa kiểm tra gói cước).

## Giải thích cơ chế

### Kế thừa và phân công trách nhiệm

`Schedule` kế thừa `Entity`, đóng gói các thuộc tính, kiểm tra ID/ngày giờ và chuyển đổi dữ liệu. `ScheduleMenu` kế thừa `BaseMenu`, dùng `Repository<Schedule>`, `Repository<Channel>`, `Repository<NoiDungVOD>` để kiểm tra quan hệ. Getter/setter không tự truy cập file. `vodID` rỗng/có giá trị xác định loại lịch, không cần lưu thêm một trường loại.

### Cộng phút và tránh trùng

`Schedule::endAfterMinutes(start, duration)` cộng giờ/phút bằng số nguyên 64 bit, chuyển phần dư sang ngày, rồi đi qua tháng bằng `Date::daysInMonth`. Không dùng múi giờ hoặc DST của hệ điều hành. Tháng 2 và các năm 1900/2000/2100 tuân theo quy tắc năm nhuận; kết quả quá 2100 bị từ chối.

Hai lịch trên cùng kênh trùng nếu `A.start < B.end && B.start < A.end`. Áp dụng chung cho TV/VOD, bỏ qua chính ID khi sửa. Hai lịch giáp ranh hợp lệ; cùng giờ khác kênh hợp lệ.

### Lưu file và tương thích

File lịch mới: `id|channelID|programName|startTime|endTime|description|vodID`.
Parser đọc cả 6 trường cũ và 7 trường mới, giữ trường cuối rỗng. Không suy đoán liên kết VOD từ tên. Parser dựng đối tượng tạm; dòng sai không thay đổi đối tượng trước đó.

`writeLinkedData` ghi sang file tạm cùng thư mục, kiểm tra ghi/flush/close rồi thay file đích. Windows dùng `MoveFileExW`; hệ khác dùng rename. Nếu thất bại, file gốc được giữ và file tạm được dọn; menu lịch khôi phục repository trước thao tác. Menu cập nhật VOD chỉ cam kết bản nháp khi đủ điều kiện. Counter có thể bỏ qua một số khi tạo thất bại; không tái dùng ID đã cấp.

Giới hạn hiện có của framework nhóm: `Repository::loadFromFile()` cảnh báo và bỏ dòng không parse được; chưa phải cơ chế sửa mọi dữ liệu hỏng. Nên dùng bộ Data/Data_backup đã đồng bộ và giữ bản sao trước khi nhập file ngoài.

## Kịch bản demo

1. Mở ứng dụng ở đúng thư mục dự án. Chọn `5 → 2 → 5`, nhập `VOD014`: thấy `LPS029` (24/10 20:00–22:30) và `LPS030` (25/10 10:00–12:30).
2. Tìm `VOD019`: `LPS031`, từ `2026-10-25 23:00` đến `2026-10-26 01:40`. Lọc ngày 26/10 cũng thấy lịch này.
3. Tra `VOD001`: thông báo NON_HOURLY; tra `VOD003`: chưa có lịch; tra `VOD999`: không tồn tại.
4. Thêm TV trên KENH001, ngày 24/10 21:00–22:00: bị chặn trùng LPS029. Đổi sang 22:30–23:00: hợp lệ. Nhập `0` nếu muốn hủy demo.
5. Sửa LPS029, Enter giữ nguyên mọi trường: không tự báo trùng chính nó. Đổi sang VOD019: kết thúc đổi thành 22:40 và tên chụp theo VOD019.
6. Vào menu chính `9`, sửa VOD014: thử thời lượng 151 hoặc chuyển NON_HOURLY; bị chặn và hiển thị LPS029/LPS030. Enter giữ giá trị cũ vẫn hợp lệ. Thử xóa cũng bị chặn bởi lịch.
7. Dùng bản sao Data để xóa cả hai lịch VOD014; khi đó đổi thời lượng/hình thức xem được, xóa VOD vẫn bị chặn nếu còn thuộc gói.
8. Thoát, mở lại và tìm theo ID để chứng minh dữ liệu lưu bền vững.

## Build và kiểm thử

Mở `CHAY_LICH_VOD.bat` để chạy bản mới tại `build/TV36.exe`. Để tự biên dịch, chạy `build.bat` trên Windows hoặc `build.sh` theo môi trường của nhóm, rồi chạy executable vừa tạo. Chương trình chạy từ thư mục gốc để đọc `Data/` đúng đường dẫn.

Bộ kiểm thử: `python tests/test_schedule_vod.py --exe <duong-dan-exe> --output <thu-muc-ket-qua-moi>`.
Script tự tạo bản sao dữ liệu và log từng kịch bản, không chạy CRUD trên Data gốc. C++ unit test được biên dịch từ stdin để không tạo main thứ hai trong cây nguồn mà build.bat quét.

Kết quả kiểm thử và vị trí executable được ghi trong `KET_QUA_KIEM_THU_LICH_VOD.md` sau khi nghiệm thu.
