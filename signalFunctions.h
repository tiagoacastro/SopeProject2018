#define OK 0
#define ERROR 1

void sigint_handler(int signo);

void toggle_termination_flag();

int flag_is_set();

int confirm_termination();

int install_sigint_handler();

void exitHandler();
