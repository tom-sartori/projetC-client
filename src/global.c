#define MAX_SIZE_SENT 512
#define ENDING_MESSAGE "/disconnect\n\0"
#define IP "127.0.0.1"
#define PORT_SOCKET_FILE 3333
#define NB_COMMAND 3

FORM *form;
FIELD *fields[2];
WINDOW *win_screen;
WINDOW *win_form;
WINDOW *win_display;
WINDOW *sub_win_form;
WINDOW *win_title;

int acceptedSocketDescriptor;
char *username;
int isDebugMode = 0;
