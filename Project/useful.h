
#define MAX_ROOM_SEATS  9999
#define MAX_CLI_SEATS   99
#define DELAY()         sleep(0)
#define WIDTH_PID 5
#define WIDTH_XXNN 5
#define WIDTH_SEAT 4
#define xstr(x) str(x)
#define str(x) #x
#define format(x) "%0" xstr(x) "d"
#define log_to_open format(2) "-OPEN\n"
#define log_to_close format(2) "-CLOSE\n"
#define log_booking format(2) "-" format(WIDTH_PID) "-" format(2) ":"
#define log_client format(WIDTH_PID) " " format(2) "." format(2) " " format(WIDTH_SEAT) "\n"
#define log_error format(WIDTH_PID) " " format(3)