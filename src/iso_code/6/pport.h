#define OK              0        
#define NO_PORT         1
#define START_TIMEOUT   2
#define SYNC_TIMEOUT    3

#define CHAR1_TIMEOUT   1        
#define GEN_TIMEOUT     2
#define COUNT_FAIL      3
#define CSUM_FAIL       4


int pp_initialize(int port);
int pp_send_byte(unsigned char data);
int pp_read_byte(unsigned char *output);
int pp_send_n_bytes(int count, unsigned char *input);
int pp_read_n_bytes(int count, unsigned char *output);
void pp_wait_quiet(void);

int pp_send_data_block(int count, unsigned char *input);
int pp_read_data_block(int *count, unsigned char *output);
void pp_delay(void);

