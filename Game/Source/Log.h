#ifndef __LOG_H__
#define __LOG_H__

//With this you can choose between full-path printing or relative-path printing
#define FULL_PATH 1

#define LOG(format, ...) Log(__FILE__, __LINE__, format, ##__VA_ARGS__)

void Log(const char file[], int line, const char* format, ...);

#endif  // __LOG_H__