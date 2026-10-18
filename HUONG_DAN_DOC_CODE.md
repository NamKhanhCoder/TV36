# Phần Hồng Anh: Channel và Schedule

Bản CRUD bổ sung trên TV36, đã cập nhật main của nhóm đến commit `d978f87`. Mục tiêu của bản này là thêm/xem/tìm/sửa/xóa và lưu file cho hai phân hệ theo cấu trúc nhóm. Các ràng buộc liên đối tượng để TODO theo thống nhất.

## 1. Chạy và trình diễn

Mở PowerShell tại thư mục chứa `main.cpp`:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1
.\build\TV36.exe
```

Chọn `4` cho Channel, `5` cho Schedule. Trong mỗi menu: `1` thêm, `2` xem/tìm, `3` sửa, `4` xóa, `0` quay lại. Nhập `0` tại menu chính để thoát. Các trường sửa bỏ trống sẽ giữ nguyên; mô tả lịch nhập `-` để xóa nội dung. Sau mỗi thao tác nhấn Enter quay về menu. Nếu nhập sai, thao tác báo lỗi và quay về menu để chọn lại; dữ liệu sửa dở trên bản sao chưa được lưu.

Máy đã có GCC/GDB. Dự án dùng thư viện chuẩn C++; Python chỉ dùng nếu muốn chạy kiểm thử. VS Code: mở đúng thư mục dự án, Ctrl+Shift+B để build. F5 dùng extension C/C++ và GDB; nếu đổi máy, kiểm tra đường dẫn debugger trong `.vscode/launch.json`.

Dữ liệu mới của nhóm có KENH001 đến KENH004; LPS001, LPS002, LPS003 để xem thử. Các URL example.com không phải luồng phát thật. Lịch LPS003 qua nửa đêm và có mô tả rỗng.

Ví dụ thêm kênh: tên `Kenh demo`, mã thể loại `TL004`, số `10`, độ phân giải `HD`, URL `https://example.com/demo.m3u8`, trạng thái nhập `1` (đang phát) hoặc `0` (ngừng phát). Với dữ liệu hiện tại, mã mới là KENH005. Thử tìm theo mã đó, sửa tên, thoát/mở lại, rồi thử hủy xóa bằng `n` trước khi xóa bằng `y`.

Ví dụ thêm lịch: mã kênh `KENH001`, chương trình `Chuong trinh demo`, bắt đầu `2026-10-16 23:30`, kết thúc `2026-10-17 00:30`, mô tả tùy chọn. Định dạng ngày giờ phải đủ số 0: `yyyy-mm-dd HH:mm`. Cả ngày và giờ được lưu trong chuỗi; kết thúc phải lớn hơn bắt đầu.

## 2. Những file thuộc phần bạn

| File | Trách nhiệm |
| --- | --- |
| `Entities/Channel.h` | Khai báo thuộc tính và hàm của một kênh |
| `Entities/Channel.cpp` | Constructor, getter/setter, kiểm tra cơ bản, đọc/ghi một dòng kênh |
| `Entities/Schedule.h` | Khai báo thuộc tính và hàm của một lịch |
| `Entities/Schedule.cpp` | Xử lý thông tin lịch, ngày giờ và đọc/ghi một dòng lịch |
| `UseCases/ChannelMenu.h/.cpp` | Nhập liệu và điều phối CRUD kênh |
| `UseCases/ScheduleMenu.h/.cpp` | Nhập liệu và điều phối CRUD lịch |
| `Data/channels.txt`, `Data/channels_counter.txt` | Danh sách kênh và số dùng để sinh mã |
| `Data/schedules.txt`, `Data/schedules_counter.txt` | Danh sách lịch và số dùng để sinh mã |
| `main.cpp` | Hai lựa chọn 4, 5 mở menu kênh và lịch trong menu chung mới của nhóm |
| `tests/test_crud.py` | Kiểm thử qua chương trình console thực tế trên dữ liệu tạm |

Phần này dùng lại các tiện ích của nhóm; `InputHelper.h/.cpp` bổ sung hàm nhập trạng thái 0/1. Các phân hệ của thành viên khác được lấy theo main đã cập nhật. `build.ps1`, `.vscode` và `.gitignore` hỗ trợ build/debug trên máy. Khi ghép với bản nhóm mới hơn, đối chiếu 8 file C++ của Channel/Schedule, các mục menu và hàm `getInputChannelStatus` trong InputHelper; ghép từng thay đổi thay vì ghi đè toàn bộ file dùng chung. Trao đổi trước khi thay dữ liệu thật của nhóm bằng dữ liệu mẫu.

## 3. Học theo một lần thêm kênh

1. Trong `main.cpp`, nhánh `case 4` tạo `ChannelMenu`, rồi gọi `run()` kế thừa từ `BaseMenu`.
2. Constructor `ChannelMenu` tạo `Repository<Channel>`, nạp `Data/channels.txt`, kiểm tra ID trùng và ghi nhận dữ liệu đã nạp được.
3. `BaseMenu::run()` lặp: in menu → đọc lựa chọn → gọi `handleChoice`.
4. Chọn 1 dẫn đến `createChannel()`.
5. `Channel draft` là bản nháp. `inputFields(draft, false)` nhập các trường và gọi setter.
6. Setter kiểm tra trường tương ứng. Nếu sai, `throw invalid_argument` chuyển về `catch` trong menu, hiển thị thông báo; chưa gọi repository để thêm.
7. Khi hợp lệ, `generateNextID()` sinh mã; vòng lặp bỏ qua mã đã có nếu counter cũ. Constructor đủ tham số tạo `Channel item` có ID.
8. `repo.create(item)` thêm vào vector trong RAM. `saveChanges()` gọi `repo.writeToFile()` rồi xác minh nội dung đọc lại trước khi báo thành công.

Entity giữ một bản ghi; Repository giữ cả danh sách; Menu nhận yêu cầu và gọi đúng thao tác. Bạn có thể lần theo luồng này để hiểu tất cả chức năng còn lại.

## 4. Ba hàm chuyển đổi dữ liệu

`exportToString()` tạo dòng lưu file:

```text
KENH001|VTV1|TL004|1|HD|https://example.com/vtv1.m3u8|DANG_PHAT
```

`parseFromString()` tách bằng `splitString`, kiểm tra đủ số cột, chuyển số kênh bằng `stoi`, tạo đối tượng hợp lệ rồi gán lại. Channel có 7 cột, Schedule có 6 cột. Parser Schedule chấp nhận mô tả rỗng sau dấu `|` cuối.

`exportToVector()` tạo `vector<string>` để TablePrinter in từng ô. Số phần tử phải khớp số cột `headers` và `widths`. Bảng rút gọn chuỗi dài; tìm theo ID còn in chi tiết từng trường để xem đầy đủ URL/mô tả.

## 5. Vì sao sửa trên bản sao?

`readByID` trả con trỏ đến bản ghi trong repository. Menu kiểm tra `nullptr` trước khi dùng, rồi tạo `Channel candidate = *found` hoặc `Schedule candidate = *found`.

Các setter sửa candidate trước. Chỉ khi tất cả trường hợp lệ mới gọi `repo.update(candidate)` và lưu. Ví dụ nhập tên mới rồi số kênh âm: tên gốc trong repository vẫn giữ nguyên vì candidate chưa được cập nhật vào danh sách.

ID được kế thừa từ `Entity`, không có mục nhập sửa ID. Getter có `const` nghĩa là hàm đọc không thay đổi đối tượng. Setter nhận `const string&` giúp truyền chuỗi mà không cần sao chép và không sửa chuỗi đầu vào.

## 6. Ngày giờ của Schedule

Schedule giữ `startTime` và `endTime` dưới dạng string theo cách SupportTicket đang làm. `normalizeDateTime()` kiểm tra độ dài, vị trí dấu và chữ số, rồi gọi constructor `Date` và `Time` để kiểm tra ngày thực tế, năm nhuận và giờ phút.

`Date::toFileString()` hiện trả `dd-mm-yyyy`; Schedule giữ chuỗi đã kiểm tra ở dạng `yyyy-mm-dd HH:mm`. Vì năm, tháng, ngày, giờ, phút đều có vị trí cố định, so sánh chuỗi theo thứ tự từ trái sang phải cũng là so sánh thời gian.

`setTimeRange(start, end)` kiểm tra cả hai mốc rồi gán cùng lúc. Điều này cho phép chuyển cả chương trình sang ngày khác mà không so sánh mốc mới đầu tiên với mốc cũ còn lại. Chưa kiểm tra chồng với lịch khác vì đó là TODO tích hợp.

## 7. Đọc lỗi và các phần để họp nhóm

File thiếu/hỏng hoặc trùng ID: menu báo và khóa thao tác để tránh ghi đè phần dữ liệu đã nạp dở. Sửa file, quay về menu chính rồi mở lại phân hệ. Không tự bỏ qua dòng hỏng. Counter cũ được xử lý bằng cách sinh tiếp đến mã chưa có.

Lưu không xác minh được: menu báo lỗi và khóa thao tác; cần kiểm tra file. Repository của nhóm hiện ghi trực tiếp, chưa có cơ chế ghi nguyên tử/khôi phục tự động nếu lỗi ổ đĩa xảy ra giữa lúc ghi.

TODO đã đặt trong code:

- Channel: kiểm tra categoryID tồn tại, số kênh không trùng, chặn xóa khi Package/Schedule tham chiếu.
- Schedule: kiểm tra kênh tồn tại/trạng thái, kiểm tra chồng lịch (loại chính bản ghi khi sửa).
- Phần phối hợp: kiểm tra mã kênh khi thêm/sửa gói; chặn xóa thể loại đang có kênh.

Chưa cần tạo thêm lớp nghiệp vụ hay thay cấu trúc Package/ChannelCategory cho bản CRUD này. Đây là bản bàn giao CRUD, chưa phải toàn bộ kiểm tra nghiệp vụ cuối cùng của nhóm.

## 8. Kiểm thử lại

Kết quả kiểm tra bản cập nhật trạng thái: build thành công cả 26 file C++ bằng GCC 14.2.0 với C++17; 19 nhóm kiểm thử đều đạt. Còn cảnh báo so sánh kiểu số trong tiện ích/repository sẵn có của nhóm. Kiểm thử không thay đổi file dữ liệu của dự án. Chưa chạy thử nút F5 trong giao diện VS Code.

```powershell
python tests\test_crud.py
```

Script cần Python 3, chỉ dùng thư viện chuẩn. Nó chạy executable đã build, tạo bản sao Data trong thư mục tạm, thao tác qua menu rồi kiểm tra file sau khi mở lại chương trình. Các nhóm kiểm tra gồm mở và quay về từ đủ 10 menu, xóa bản ghi cuối cùng rồi nạp lại danh sách rỗng, CRUD hai phân hệ, hủy xóa, giữ/xóa mô tả, sửa sai không lưu dở, lịch qua nửa đêm/năm nhuận, dữ liệu hỏng/trùng mã, ID không tồn tại, counter cũ và danh sách rỗng. Cuối cùng so sánh hash để xác nhận Data của dự án không đổi.

Để học ngược, hãy tự đổi một thông báo trong menu, build và chạy; sau đó tự theo dõi một lần thêm, một lần sửa và một lần đọc lại file. Chỉ thay đổi từng phần nhỏ để biết mình vừa làm gì.

## 9. Nhập trạng thái kênh bằng 0/1

`InputHelper::getInputChannelStatus(message, optional)` chứa toàn bộ vòng lặp nhập trạng thái: `0` trả `NGUNG_PHAT`, `1` trả `DANG_PHAT`. Giá trị khác (kể cả nhập chữ, 2 hoặc 1.0) báo lỗi và nhập lại ngay tại trường trạng thái. Khi thêm kênh, không được bỏ trống; khi sửa, `optional = true` cho phép Enter trả chuỗi rỗng để giữ nguyên trạng thái hiện tại.

`ChannelMenu::inputFields` gọi helper rồi truyền kết quả vào `Channel::setStatus`. Kiểu thuộc tính và định dạng file vẫn là chuỗi trạng thái cũ, nên dữ liệu nhóm đã có tiếp tục đọc được.

Trong lần cập nhật này, bản `build/TV36.exe` cũ đang chạy nên Windows khóa file. Bản mới được build thành `build/TV36-fixed-input.exe`; mở file này để thử nhập 0/1. Sau khi thoát bản cũ, có thể chạy lại `build.ps1` để cập nhật tên executable mặc định. Có thể chỉ định tên đầu ra khi build để tránh file đang bị khóa:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -OutputName TV36-fixed-input.exe
.\build\TV36-fixed-input.exe
python tests\test_crud.py build\TV36-fixed-input.exe
```

## 10. Sửa lỗi nhập sai mã vẫn được nhận

Trước đây mã thể loại dùng getInputString nên chỉ chặn chuỗi rỗng. Hiện ChannelMenu dùng InputHelper::getInputID với tiền tố TL; ScheduleMenu dùng cùng hàm với tiền tố KENH. Mã phải có tiền tố và ít nhất 3 chữ số; chữ thường được chuyển sang chữ hoa. Ví dụ khenh, KENH001 tại ô thể loại, TL04, TL00A đều bị từ chối và yêu cầu nhập lại tại ô mã; tl004 được chuẩn hóa thành TL004.

Số kênh dùng getInputPositiveInt; số âm, 0, chữ, số thập phân và tràn số bị từ chối ngay. Độ phân giải dùng getInputChannelResolution và chỉ nhận SD/HD/FHD/UHD. Khi sửa, Enter tại các trường này giữ nguyên. Channel::setCategoryID cũng kiểm tra định dạng để dữ liệu đọc từ file không bỏ qua ràng buộc.

Tên kênh vẫn là chuỗi tự do không rỗng; một tên ngắn như j chưa bị cấm bởi yêu cầu hiện tại. Kiểm tra định dạng mã chưa kiểm tra mã đó tồn tại trong danh sách; phần liên kết vẫn chờ nhóm tích hợp. File hỏng chứa mã thể loại sai định dạng sẽ bị báo lỗi và khóa menu, không tự sửa hoặc xóa dữ liệu.

Bản kiểm thử mới đã tái hiện chuỗi nhập khenh rồi sửa thành tl004, xác nhận chưa chuyển sang ô số kênh khi mã còn sai. Tổng cộng 19 nhóm kiểm thử đạt; dữ liệu dự án không đổi.
