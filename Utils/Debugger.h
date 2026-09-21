#ifndef DEBUGGER_H
#define DEBUGGER_H

//Gemini sposored
//Creds to NamKhanhCoder
#include <iostream>

/* 
 * BẬT/TẮT DEBUG MODE Ở ĐÂY
 * Comment dòng #define DEBUG_MODE lại để tắt toàn bộ log khi nộp bài.
 * Bỏ comment để bật log khi đang code và fix bug.
 */
#define DEBUG_MODE 

#ifdef DEBUG_MODE

    // In ra một câu thông báo
    #define LOG(msg) std::cout << "[DEBUG] " << msg << std::endl;
    
    // In ra TÊN biến và GIÁ TRỊ của biến đó. Rất tiện để check giá trị truyền vào!
    // Ví dụ: int x = 5; LOG_VAL(x); -> In ra: [DEBUG] x = 5
    #define LOG_VAL(var) std::cout << "[DEBUG] " << #var << " = " << (var) << std::endl;

#else

    // Khi tắt DEBUG_MODE, các macro này sẽ trở thành khoảng trắng, 
    // compiler sẽ bỏ qua hoàn toàn, không ảnh hưởng tới hiệu năng và không in ra gì cả.
    #define LOG(msg)
    #define LOG_VAL(var)

#endif

#endif // DEBUGGER_H
