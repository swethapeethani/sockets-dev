// enum STATE {
//     ERROR = -1,
//     INIT = 0,
//     PARSE_ELE = 1,
// };

struct resprequest{
    int state;
    int buffsize;
    int usedsize;
    int index;
    char buff[50];
};

// typedef struct {
//     char *buff;
//     int buffsize;
//     int usedsize;
// }respresponse;

