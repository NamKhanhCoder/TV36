# Phần Hồng Anh — Channel và Schedule hoàn thiện

## 1. Phiên bản và cách chạy

Bản làm việc cập nhật main TV36 đến commit nền `a189b52` ngày 19/10/2026. Các sửa đổi tiếp theo hoàn thiện phần kênh/lịch theo file debug 17/10. Main chung vẫn có 10 use case; chọn **4 — Channel**, **5 — Schedule**.

```powershell
cd C:\Users\ADMIN\Desktop\devop\TV36
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -OutputName TV36-complete.exe
.\build\TV36-complete.exe
python tests\test_crud.py build\TV36-complete.exe
```

GCC/GDB đã có trên máy. Ứng dụng chỉ dùng C++17/thư viện chuẩn. Python chỉ cần khi chạy kiểm thử. Các file .exe cũ không tự cập nhật theo source; hãy dùng đúng `build/TV36-complete.exe`. Thoát executable trước khi build lại cùng tên.

`build.ps1` biên dịch source trong main/Entities/UseCases/Utils; có `-Warnings` để xem cảnh báo. Hai script build.bat/build.sh cũng đã thêm `-std=c++17` để hỗ trợ filesystem dùng khi lưu file.

## 2. Hai đối tượng vẫn giữ cấu trúc đơn giản

| Đối tượng | Dữ liệu |
| --- | --- |
| Channel | id kế thừa Entity; name, categoryID, channelNumber, resolution, streamURL, status |
| Schedule | id kế thừa Entity; channelID, programName, startTime, endTime, description |

Mỗi lịch là một chương trình trên một kênh. Danh sách lịch tổng hợp hiển thị được nhiều kênh; không cần biến Schedule thành đối tượng chứa danh sách kênh. Phát cùng chương trình trên hai kênh thì tạo hai lịch riêng.

Giữ nguyên constructor/getter/setter và ba hàm parseFromString/exportToString/exportToVector để tương thích phần nhóm đã ghép. Không thêm Entity, service hay thư viện mới. Các quan hệ được kiểm tra bằng repository của các kiểu đã có.

## 3. Chức năng và quy tắc Channel

- Thêm/xem/tìm/sửa/xóa, sinh mã KENH tự động và lưu/đọc file.
- Tên và URL không rỗng, không chứa dấu `|` hoặc xuống dòng; nhập sai yêu cầu nhập lại tại trường đó. Tên ngắn không bị cấm nếu không có quy định khác của nhóm.
- In bảng thể loại trước khi chọn. categoryID phải đúng định dạng TL + ít nhất 3 chữ số và có trong Data/channel_categories.txt. Chữ thường được chuẩn hóa sang hoa.
- Số kênh là số nguyên dương, không trùng bất kỳ kênh nào, kể cả kênh ngừng phát. Khi sửa bỏ qua chính ID đang sửa.
- Độ phân giải chỉ SD/HD/FHD/UHD.
- Trạng thái nhập ở InputHelper: **0 = NGUNG_PHAT, 1 = DANG_PHAT**. Khi sửa, Enter giữ nguyên.
- Khi xóa, hỏi y/n. Sau xác nhận, nạp lại Package và Schedule; chặn nếu còn bất kỳ gói/lịch tham chiếu, báo rõ mã GOI/LPS. Áp dụng cả gói ngừng đăng ký, kênh ngừng phát và lịch cũ.
- Mã kênh trong danh sách Package được so sánh sau khi bỏ khoảng trắng ngoài và đổi sang hoa. Nếu dữ liệu gói sai định dạng thì báo lỗi, không coi là không có tham chiếu.

Menu xem/tìm: tất cả, ID, tên chứa chuỗi, thể loại, trạng thái. Menu 5 sắp xếp để xem theo số kênh hoặc tên. Sắp xếp bản sao, không thay đổi file.

Nhập 0 tại trường Name để hủy thêm/sửa; không lưu bản nháp và không tăng counter khi hủy.

## 4. Chức năng và quy tắc Schedule

- Thêm/xem/tìm/sửa/xóa, sinh mã LPS tự động.
- In bảng kênh và trạng thái trước khi chọn. Mã kênh phải có trong Data/channels.txt.
- Tạo lịch mới hoặc đổi sang kênh khác phải chọn kênh DANG_PHAT.
- Kênh đã NGUNG_PHAT: vẫn xem lịch cũ, sửa tên chương trình/mô tả và xóa lịch; đổi khoảng giờ bị chặn, cần bật lại kênh trước. Khi đổi sang kênh khác đang phát thì được kiểm tra như bình thường.
- Ngày giờ nhập chuẩn `yyyy-mm-dd HH:mm`; kiểm tra ngày thực tế, năm nhuận và giờ phút. Kết thúc sau bắt đầu. Bản hiện tại không áp đặt lịch phải ở tương lai.
- Nhập sai định dạng ngày/giờ sẽ nhập lại tại trường đó. Kết thúc không sau bắt đầu thì nhập lại kết thúc. Chồng lịch thì nhập lại khoảng giờ, giữ tên chương trình và mã kênh đã chọn.
- Cùng kênh bị chồng khi `newStart < oldEnd && oldStart < newEnd`. Khi sửa bỏ qua chính scheduleID. Hai lịch tiếp giáp hợp lệ; cùng giờ trên hai kênh khác nhau hợp lệ; xử lý đúng qua nửa đêm.
- Mô tả tùy chọn. Khi sửa, Enter giữ nguyên, `-` xóa nội dung. Chặn dấu phân cách/xuống dòng.
- Nhập 0 tại Program name để hủy thêm/sửa.

Menu xem/tìm: tất cả, ID, theo kênh, theo ngày. Lọc ngày gồm cả chương trình bắt đầu hôm trước nhưng còn phát sau 00:00; lịch kết thúc đúng 00:00 không tính vào ngày tiếp theo. Ngày lọc nhập theo helper Date của nhóm (dd/mm/yyyy). Menu 5 sắp theo giờ bắt đầu tăng dần, chỉ sắp để xem.

## 5. Các file cần đọc để học code

| File | Trách nhiệm |
| --- | --- |
| Entities/Channel.h/.cpp | Dữ liệu một kênh, kiểm tra thuộc tính cơ bản và chuyển đổi một dòng |
| Entities/Schedule.h/.cpp | Dữ liệu một lịch, ngày giờ chuẩn và khoảng thời gian |
| UseCases/ChannelMenu.h/.cpp | CRUD kênh; dùng thêm repoCategory, repoPackage, repoSchedule |
| UseCases/ScheduleMenu.h/.cpp | CRUD lịch; dùng thêm repoChannel và findConflict |
| Utils/InputHelper.h/.cpp | Nhập mã/số/độ phân giải/trạng thái; bổ sung getInputText, getInputDateTime, getInputConfirmation |
| tests/test_crud.py | Kiểm thử qua chương trình console thực, dữ liệu tạm |
| Data/channels.txt và schedules.txt | Mỗi dòng một bản ghi; counter riêng để sinh ID |

## 6. Lần theo một lần thêm và sửa

1. Main case 4 tạo ChannelMenu; BaseMenu::run in menu và gọi handleChoice.
2. loadChannels nạp lại danh sách, kiểm tra cấu trúc/ID/số kênh trùng. Với Schedule dùng loadSchedules, đồng thời kiểm tra lịch chồng có sẵn trong file.
3. createChannel tạo bản nháp; inputFields nhập dữ liệu, dùng helper cho định dạng và dùng repository để kiểm tra tồn tại/trùng.
4. Chỉ sau khi dữ liệu hợp lệ mới sinh ID và gọi repo.create. Nếu counter cũ, sinh tiếp đến mã chưa có.
5. saveChanges ghi toàn bộ danh sách sang file `.pending`, kiểm tra ghi/đóng thành công rồi dùng filesystem::rename thay file gốc; đọc lại xác nhận trước thông báo thành công.
6. updateChannel/updateSchedule tìm bản ghi, tạo candidate = *found rồi sửa candidate. Chỉ khi nhập đủ hợp lệ mới repo.update. Hủy hoặc lỗi trước bước đó không sửa dở bản gốc.

Entity giữ một bản ghi, repository giữ danh sách, menu điều phối yêu cầu người dùng. getID() kế thừa từ Entity; người dùng không sửa ID.

## 7. Kiểm tra file và lỗi ghi

loadChecked là hàm hỗ trợ trong từng file menu, không phải lớp mới. Nó kiểm tra mở file, số cột, ID, parser, chuyển ngược dữ liệu và ID trùng trước khi gọi loadFromFile của nhóm. File tham chiếu thiếu/hỏng thì thao tác báo lỗi và dừng, không xem như danh sách rỗng để cho xóa.

File chính thiếu/hỏng/trùng số kênh/chồng lịch: khóa thao tác để tránh ghi đè dữ liệu nạp dở; sửa file rồi mở lại menu. Không tự xóa dòng hỏng. Quan hệ cũ bị thiếu đối tượng tham chiếu có thể sửa bằng cách chọn lại đối tượng hợp lệ; nếu không có đối tượng nào chọn được thì thao tác hủy với thông báo.

Khi ghi lỗi hoặc không thay được file gốc, menu báo lỗi và khóa để tránh tiếp tục dùng trạng thái RAM chưa lưu. File gốc chỉ được thay sau khi file tạm ghi và đóng thành công. File .pending có thể còn lại sau lỗi; không được tự nạp vào chương trình. Cơ chế này giảm nguy cơ hỏng file khi ghi thất bại, không phải hệ quản trị CSDL có khóa đồng thời hay bảo đảm chống mất điện.

## 8. Dữ liệu mẫu và phạm vi nhóm

- Data hiện có **10 kênh và 10 lịch**, counter ít nhất 10. Giữ nguyên 4 kênh/3 lịch trước đó, chỉ thêm mẫu để đủ số lượng.
- Bản sao dữ liệu trước bổ sung: build/before-completion-20261019. Không thay Data của các thành viên khác.
- Các kênh mới phân loại theo danh mục thật: Discovery → TL007 Khám phá; HBO → TL003 Phim; Cartoon → TL006 Thiếu nhi; MTV → TL008 Âm nhạc; Bloomberg → TL009 Quốc tế; Hà Nội → TL010 Địa phương.
- URL example.com chỉ là thông tin minh họa, chưa xác nhận phát thật. Bản ghi KENH004 do nhóm thêm trước đó vẫn giữ nguyên.
- PackageMenu/SupportTicket và các lỗi ngoài hai phân hệ vẫn thuộc thành viên khác. Đặc biệt người làm Package cần kiểm tra tồn tại mã kênh khi thêm/sửa gói; chặn xóa ở Channel không thay thế kiểm tra chiều đó.
- Các file chung Entity/Repository/BaseMenu, menu chính 10 use case và cấu trúc dữ liệu phần khác giữ nguyên. Script build thêm chuẩn C++17; InputHelper bổ sung hàm mà không đổi API cũ.

## 9. Kiểm thử và bàn giao

Kết quả lần hoàn thiện: build thành công cả 26 file C++ với GCC/C++17 và -Wall -Wextra; **39 nhóm kiểm thử đều đạt**, hash Data thật không đổi sau kiểm thử. Vẫn có cảnh báo kiểu số có dấu/không dấu trong tiện ích/repository của nhóm. Đã kiểm tra cấu hình JSON/task/debugger; chưa thao tác thử nút F5 trong giao diện VS Code. Task/launch trên máy trỏ tới TV36-complete.exe.

Chạy `python tests/test_crud.py build/TV36-complete.exe`. Script tạo bản sao Data trong thư mục tạm; fixture kiểm thử riêng không phụ thuộc số mẫu thật. Các ca kiểm tra bao gồm CRUD, nhập lại, hủy, trạng thái, khóa ngoại, số kênh trùng, ràng buộc xóa, chồng lịch nhiều dạng, lịch qua ngày, bỏ qua chính bản ghi khi sửa, kênh ngừng phát, lọc/sắp xếp, file hỏng/thiếu, counter cũ và ghi lỗi.

Một ca giữ chương trình đang ở xác nhận xóa, thay dữ liệu gói trong bản sao rồi xác nhận để chứng minh tham chiếu được nạp lại. Một ca đặt file chỉ đọc trên Windows để xác nhận lỗi thay file không làm hỏng bản gốc. Cuối bộ kiểm thử so sánh hash toàn bộ Data thật.

Bản hoàn thiện này dành cho main TV36. Báo cáo BAO_CAO_CAP_NHAT_2026-10-19.md ở máy mô tả tình trạng ngay sau pull, trước lần hoàn thiện; dùng hướng dẫn hiện tại để xem hành vi mới.
