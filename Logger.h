#pragma once
#include <iostream>

// --- CÔNG TẮC DEBUG ---
// Bật: Bỏ comment dòng dưới. Tắt (khi nộp bài): Thêm // vào trước.
#define DEBUG_MODE 

#ifdef DEBUG_MODE
    // Nếu DEBUG_MODE đang bật, LOG(x) sẽ biến thành lệnh cout (giữ nguyên format bạn đã viết)
    #define LOG(x) std::cout << x <<endl;
#else
    // Nếu DEBUG_MODE bị tắt, LOG(x) sẽ biến thành khoảng trắng (không làm gì cả)
    #define LOG(x) 
#endif