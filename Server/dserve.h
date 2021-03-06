/*

dserve.h is a common header for dserve

dserve is a tool for performing REST queries from WhiteDB using a cgi
protocol over http(s). Results are given in the json or csv format.

See http://whitedb.org/tools.html for a detailed manual.

Copyright (c) 2013, Tanel Tammet

This software is under MIT licence unless linked with WhiteDB: 
see dserve.c for details.
*/

/* ====== select windows/linux dependent includes and options ======= */

#define SERVEROPTION // remove this for cgi/command line only: no need for threads/sockets in this case

#if _MSC_VER
#include <dbapi.h> // set this to "../Db/dbapi.h" if whitedb is not installed
#define snprintf _snprintf
#else
#include <whitedb/dbapi.h> 
#endif

#ifdef SERVEROPTION
#if _MSC_VER
// windows, see http://msdn.microsoft.com/en-us/library/windows/desktop/ms738566%28v=vs.85%29.aspx
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h> // windows.h only with lean_and_mean before it
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <strsafe.h>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "User32.lib") // required by win_err_handle only
#define THREADPOOL 0 // threadpools are not implemented for windows
#define CLOSE_CHECK_THRESHOLD 0 // always set this to 0 for windows
#else
// linux
#include <pthread.h>
#define THREADPOOL 1 // set to 0 for no threadpool (instead, new thread for each connection)
#define CLOSE_CHECK_THRESHOLD 10000 // close immediately after shutdown for msg len less than this
#endif
#endif

#ifdef USE_OPENSSL
#include <openssl/opensslconf.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
// define KEY_FILE and CERT_FILE to overrule values normally given in conf file
// create both by: 
// openssl req -x509 -nodes -days 365 -newkey rsa:2048 
//   -keyout exampleprivatekey.key -out examplecertificate.crt
// or from this by: openssl rsa -in exampleprivatekey.key -out exampleprivatekey.pem
// #define KEY_FILE "/home/tanel/whitedb/Server/exampleprivatekey.key" // overrule conf file
// #define CERT_FILE "/home/tanel/whitedb/Server/examplecertificate.crt" // overrule conf file
#endif

/* =============== configuration macros =================== */



//#define CONF_FILE "/home/tanel/whitedb/Server/conf.txt"

#define DEFAULT_DATABASE "1000" // used if none explicitly given and not overruled by conf file
#define DEFAULT_DATABASE_SIZE 10000000 // used if none explicitly given and not overruled by conf file
  // set DEFAULT_DATABASE_SIZE to 0 to inhibit automatic database creation upon insert
#define MAX_DATABASE_SIZE 1000000000 // limit if not overruled by conf file
  // set MAX_DATABASE_SIZE to 0 to inhibit any database creation
 
// print level

#define INFOPRINT // if set, neutral info printed to stderr, no info otherwise
#define WARNPRINT // if set, warnings printed to stderr, no warnprint otherwise
#define ERRPRINT // if set, errors printed to stderr, no errprint otherwise

// server/connection configuration

//#define DEFAULT_PORT 8080 // define this to run as a server on that port if no params given
//#define USE_OPENSSL // define this to build a https server: normally defined in compiler flags
#define MULTI_THREAD // removing this creates a simple iterative server
#define MAX_THREADS 8 // size of threadpool and max nr of threads in an always-new-thread model
#define QUEUE_SIZE 100 // task queue size for threadpool
#define TIMEOUT_SECONDS 2 // used for cgi and command line only
#define CATCH_SIGNALS // remove this to leave system error signals unhandled

// header row templates: XXXXXXXXXX replaced with actual content length

#define JSON_CONTENT_TYPE "Content-Type: application/json\r\n\r\n"
#define CSV_CONTENT_TYPE "Content-Type: text/csv\r\n\r\n"
#define CONTENT_LENGTH "Content-Length: %d\r\n"
#define HEADER_TEMPLATE "HTTP/1.0 200 OK\r\n\
Server: dserve\r\n\
Access-Control-Allow-Origin: *\r\n\
Connection: Close\r\n\
Cache-Control: no-cache, must-revalidate\r\n\
Pragma: no-cache\r\n\
Content-Length: XXXXXXXXXX \r\n\
Content-Type: text/plain\r\n\r\n"

// limits

#define MAXQUERYLEN 2000 // query string length limit for GET
#define MAXPARAMS 100 // max number of cgi params in query
#define MAXCOUNT 100000 // max number of result records
#define MAXIDS 1000 // max number of rec id-s in recids query
#define MAXLINE 10000 // server query input buffer and one header line max
#define MAXLINES 1000 // server query input: max nr of header lines
#define CONF_BUF_SIZE 1000 // initial conf buf size, incremented as necessary
#define MAX_CONF_BUF_SIZE 10000000 // max conf file size
#define CONF_VALS_SIZE 2 // initial size of conf value array
#define MAX_CONF_VALS_SIZE 1000000 // max size of conf value array
#define ERRBUF_LEN 200 // limit for simple param-checking error strings

// QUERY PARSING

#define JSONP_PARAM "jsonp" // a jsonp padding parameter: use as jsonp=mycallback
#define NOACTION_PARAM "_" // an allowed additional cgi parameter with no effect: use as _=123
//#define ALLOW_UNKNOWN_PARAMS // define this to allow any unrecognized params

// result output/print settings

#define HELP_PARAM "--help" // for dserve --help
#define INITIAL_MALLOC 1000 // initially malloced result size
#define MAX_MALLOC 100000000 // max malloced result size
#define MIN_STRLEN 100 // fixed-len obj strlen, add this to strlen for print-space need
#define STRLEN_FACTOR 6 // might need 6*strlen for json encoding
#define DOUBLE_FORMAT "%g" // snprintf format for printing double
#define JS_NULL "[]" 
#define CSV_SEPARATOR ',' // must be a single char
#define MAX_DEPTH_DEFAULT 100 // can be increased
#define MAX_DEPTH_HARD 10000 // too deep rec nesting will cause stack overflow in the printer
#define HTTP_LISTENQ  1024 // server only: second arg to listen: listening queue length
   // may want to use SOMAXCONN instead of 1024 in windows
#define HTTP_HEADER_SIZE 1000 // server only: buffer size for header
#define HTTP_ERR_BUFSIZE 1000 // server only: buffer size for errstr

// QUERY PARAMETERS

/*
#define QUERY_OP_PARAM "op"
#define QUERY_DB_PARAM "db"
#define QUERY_OP_PARAM "op"
#define QUERY_OP_PARAM "op"
*/

// normal nonterminating error strings

#define NOQUERY_ERR "no query"
#define LONGQUERY_ERR "too long query"
#define MALFQUERY_ERR "malformed query"

#define UNKNOWN_PARAM_ERR "unrecognized parameter: %s"
#define UNKNOWN_PARAM_VALUE_ERR "unrecognized value %s for parameter %s"
#define NO_OP_ERR "no op given: use op=opname for opname in search,insert,..."
#define UNKNOWN_OP_ERR "unrecognized op: use op=search or op=recids"
#define NO_FIELD_ERR "no field given"
#define NO_VALUE_ERR "no value given"
#define CANNOT_ALLOC_ERR "cannot allocate global data\n"
#define DB_PARAM_ERR "use db=name with a numeric name for a concrete database"
#define DB_ATTACH_ERR "no database found: use db=name with a numeric name for a concrete database"
#define FIELD_ERR "unrecognized field: use an integer starting from 0"
#define COND_ERR "unrecognized compare: use equal, not_equal, lessthan, greater, ltequal or gtequal"
#define INTYPE_ERR "unrecognized type: use null, int, double, str, char or record "
#define INVALUE_ERR "did not find a value to use for comparison"
#define INVALUE_TYPE_ERR "value does not match type"
#define DECODE_ERR "field data decoding failed"
#define DELETE_ERR "record deletion failed"
#define DB_NO_SIZE_ERR "database size not given"
#define DB_BIG_SIZE_ERR "database size too big"
#define DB_EXISTS_ALREADY_ERR "database exists already"
#define DB_NOT_EXISTS_ERR "database does not exist"
#define DB_CREATE_ERR "database creation failed"
#define DB_DROP_ERR "database dropping failed"
#define DB_NAME_ERR "incorrect or missing database name"
#define DB_AUTHORIZE_ERR "access to database not authorized"

#define HTTP_METHOD_ERR "method given in http not implemented: use GET"
#define HTTP_REQUEST_ERR "incorrect http request"
#define HTTP_NOQUERY_ERR "no query found"
#define WRITEN_ERROR "writen error\n"

// formatting normal err messages 

#define JS_TYPE_ERR "\"\""  // currently this will be shown also for empty string
//#define NORMAL_ERR_FORMAT "[\"%s\"]" // normal non-terminate error string is put in here
#define NORMAL_ERR_FORMAT "\"ERROR: %s\"" // normal non-terminate error string is put in here
#define JSONP_ERR_FORMAT "%s(\"ERROR: %s\");" // jsonp non-terminate error string is put in here

// normally one request terminating error strings 

#define MALLOC_ERR "cannot allocate enough memory for result string"
#define CGI_QUERY_ERR "cannot get query string: maybe bad/missing content-length?"
#define NOT_AUTHORIZED_ERR "query not authorized"
#define NOT_AUTHORIZED_INSERT_CREATE_ERR "database missing and creation of new database not authorized"
#define QUERY_ERR "query creation failed"
#define MISSING_JSON_ERR "input json missing"
#define JSON_ERR "json parsing failed"
#define DB_CREATE_ERR "database creation failed"
#define RECIDS_COMBINED_ERR "search by record ids cannot be combined with search by fields"

// globally terminating error strings

#define TIMEOUT_ERR "timeout"
#define INTERNAL_ERR "internal error"
#define LOCK_ERR "database locked"
#define INCONSISTENT_ERR "database inconsistent"
#define LOCK_RELEASE_ERR "releasing read lock failed: database may be in deadlock"

#define WSASTART_ERR "WSAStartup failed\n"
#define MUTEX_ERROR "Error initializing pthread mutex, cond or attr\n"
#define THREAD_CREATE_ERR "Cannot create a thread: %s\n"
#define PORT_LISTEN_ERR "Cannot open port for listening: %s\n"
#define SETSOCKOPT_READT_ERR "Setsockopt for read timeout failed\n"
#define SETSOCKOPT_WRITET_ERR "Setsockopt for write timeout failed\n"
#define THREADPOOL_UNLOCK_ERR "Threadpool unlock failure\n"
#define COND_WAIT_FAIL_ERR "pthread_cond_wait failure\n"
#define THREADPOOL_LOCK_ERR "Threadpool lock failure \n"
#define TERMINATE_ERR "dserve terminating\n"
#define TERMINATE_NOGLOB_ERR "dserve terminating: no global data found\n"

#define CONF_OPEN_ERR "Cannot open configuration file %s \n"
#define CONF_MALLOC_ERR "Cannot malloc for configuration file reading\n"
#define CONF_READ_ERR "Cannot read from configuration file %s\n"
#define CONF_VAL_ERR "Unknown key %s in configuration file\n"
#define CONF_SIZE_ERR "MAX_CONF_BUF_SIZE too small to read the the configuration file %s\n"
#define CONF_VALNR_ERR "MAX_CONF_VALS_SIZE too small for the list of conf values\n"
#define NO_KEY_FILE_ERR "key_file not given in configuration for https\n"
#define NO_CERT_FILE_ERR "cert_file not given in configuration for https\n"

// warnings and info

#define CONN_ACCEPT_WARN "Cannot accept connection: %s.\n"
#define SHUTDOWN_WARN "Shutting down.\n"
#define SHUTDOWN_THREAD_WARN "Shutting down thread.\n"
#define COND_SIGNAL_FAIL_WARN "pthread_cond_signal failure.\n"
#define READING_FAILED_WARN "Failed to read input.\n"
#define CONTENT_LENGTH_MISSING_WARN "Content-length missing.\n"
#define CONTENT_LENGTH_BIG_WARN "Content-length too big.\n"

#define THREADPOOL_INFO "Running multithreaded with a threadpool.\n"
#define MULTITHREAD_INFO "Running multithreaded without threadpool.\n"

// internal values

#define READ "r"
#define READ_LOCK_TYPE 1
#define WRITE_LOCK_TYPE 2

#define ADMIN_LEVEL 0
#define WRITE_LEVEL 1
#define READ_LEVEL  2

#define CONTENT_TYPE_UNKNOWN     0 // this and following determined by "Content-Type:" 
#define CONTENT_TYPE_URLENCODED  1 // application/x-www-form-urlencoded
#define CONTENT_TYPE_JSON        2 // application/json

#define GET_METHOD_CODE  1  // GET request code for tdata->method 
#define POST_METHOD_CODE 2  // POST request code code for tdata->method 

#define COUNT_CODE 0 // passed as last arg to generic search
#define SEARCH_CODE 1 // passed as last arg to generic search
#define DELETE_CODE 2 // passed as last arg to generic search
#define UPDATE_CODE 3 // passed as last arg to generic search

#define BAD_WG_VALUE  WG_ILLEGAL // 0xff used for returning encoding failures

// err codes from sysexit.h project

#define ERR_EX_NOINPUT 66      // required file was missing or unreadable
#define ERR_EX_UNAVAILABLE 69  // an external service or program failed
#define ERR_EX_SOFTWARE 70     // hard software errors from catching a signal
#define ERR_EX_TEMPFAIL 75     // temporary failure, perhaps not really an error
#define ERR_EX_CONFIG  78      // configuration errors

#define CONF_DEFAULT_DBASE "default_dbase"
#define CONF_DEFAULT_DBASE_SIZE "default_dbase_size"
#define CONF_MAX_DBASE_SIZE "max_dbase_size"
#define CONF_DBASES "dbases"
#define CONF_ADMIN_IPS "admin_ips"
#define CONF_WRITE_IPS "write_ips"
#define CONF_READ_IPS "read_ips"
#define CONF_ADMIN_TOKENS "admin_tokens"
#define CONF_WRITE_TOKENS "write_tokens"
#define CONF_READ_TOKENS "read_tokens"
#define CONF_KEY_FILE "key_file"
#define CONF_CERT_FILE "cert_file"

/*   ========== global structures =============  */

// each thread (or a single cgi/command line) has its own thread_data block

typedef struct thread_data * thread_data_p;

struct thread_data{  
  // thread type, database, locks
  int    isserver; // 1 if run as a server, 0 if not
  int    iscgi; // 1 if run as a cgi program, 0 if not
  int    realthread; // 1 if thread, 0 if not
  int    thread_id; // 0,1,..
  struct common_data *common; // common is shared by all threads
  struct dserve_global *global; // global is thread-independent
  void  *db; // NULL iff not attached
  char  *database; //database name
  wg_int lock_id; // 0 iff not locked
  int    lock_type; // 1 read, 2 write  
  int    inuse; // 1 if in use, 0 if not (free to reuse)
  // task details   
  int    conn; // actual socket id
#ifdef USE_OPENSSL    
  SSL    *ssl;  
#endif   
  char  *ip; // request ip
  int    port;  // request port
  int    method; // request method code: unknown 0, GET 1, POST 2, ...
  int    res;    // stored by thread
  // input data
  char  *inbuf;  // input buffer: used only by post, should be freed
  int    intype; // 0 missing content-type, 1 urlencoded, 2 json
  // printing
  char  *jsonp; // NULL or jsonp function string 
  int    format;  // 1 json, 0 csv    
  int    showid; // print record id for record: 0 no show, 1 first (extra) elem of record
  int    depth; // limit on records nested via record pointers (0: no nesting)
  int    maxdepth; // limit on printing records nested via record pointers (0: no nesting)
  int    strenc; /*
                strenc==0: nothing is escaped at all
                strenc==1: non-ascii chars and % and " urlencoded
                strenc==2: json utf-8 encoding, not ascii-safe
                strenc==3: csv encoding, only " replaced for ""
              */
  char   *buf; // address of the whole string buffer start (not the start itself)
  char   *bufptr;  // address of the next place in buf to write into
  int    bufsize; // buffer length
};

// a single dserve_global is created as a global var dsglobal

typedef struct dserve_global * dserve_global_p;

struct dserve_global{
  struct dserve_conf *conf;
  int                maxthreads;  
  struct thread_data threads_data[MAX_THREADS];  
};

// configuration data read from file, kept as sized_strlst for each kind
// with each sized_strlst containing char* array of conf vals

struct sized_strlst{
  int size; // vals array size: not all have to be used
  int used; // nr of used els in vals
  char** vals; // actual array of char* to vals
};

typedef struct dserve_conf * dserve_conf_p;

struct dserve_conf{
  struct sized_strlst default_dbase;
  struct sized_strlst default_dbase_size;
  struct sized_strlst max_dbase_size;
  struct sized_strlst dbases;
  struct sized_strlst admin_ips;
  struct sized_strlst write_ips;
  struct sized_strlst read_ips;
  struct sized_strlst admin_tokens;
  struct sized_strlst write_tokens;
  struct sized_strlst read_tokens;
  struct sized_strlst key_file;
  struct sized_strlst cert_file;
};

#ifdef SERVEROPTION

#if _MSC_VER
// windows

#define ssize_t int
#define socklen_t int

// task queue elements

typedef struct {
  int  conn; 
} common_task_t;

// common information pointed to from each thread data block: lock, queue, etc

struct common_data{
  void*           *threads;
  void*           mutex;    
  void*           cond;  
  common_task_t   *queue;
  int             thread_count;
  int             queue_size;
  int             head;
  int             tail;
  int             count;
  int             started;
  int             shutdown;  
};

#else
// linux
// task queue elements

typedef struct {
  int  conn; 
#ifdef USE_OPENSSL
  SSL    *ssl; 
#endif  
} common_task_t;

// common information pointed to from each thread data block: lock, queue, etc

struct common_data{
  pthread_t       *threads;
  pthread_mutex_t mutex;    
  pthread_cond_t  cond;  
  common_task_t   *queue;
  int             thread_count;
  int             queue_size;
  int             head;
  int             tail;
  int             count;
  int             started;
  int             shutdown;  
};
#endif // win or linux server
#else
// no serveroption
typedef struct {
  int  conn; 
} common_task_t;

struct common_data{
  void*           *threads;
  void*           mutex;    
  void*           cond;  
  common_task_t   *queue;
  int             thread_count;
  int             queue_size;
  int             head;
  int             tail;
  int             count;
  int             started;
  int             shutdown;  
};
#endif // serveroption or no serveroption

/* =============== global protos =================== */

// in dserve.c:

char* process_query(char* inquery, thread_data_p tdata); 
void print_final(char* str, thread_data_p tdata);
void* op_attach_database(thread_data_p tdata,char* database,int accesslevel);
int op_detach_database(thread_data_p tdata, void* db);

// in dserve_net.c:

int run_server(int port, struct dserve_global * globalptr);
char* make_http_errstr(char* str, thread_data_p tdata);

// in dserve_util.c:

wg_int encode_incomp(void* db, char* incomp);
wg_int encode_intype(void* db, char* intype);
wg_int encode_invalue(void* db, char* invalue, wg_int type);
int isint(char* s);
int isdbl(char* s);
int parse_query(char* query, int ql, char* params[], char* values[]);
char* urldecode(char *indst, char *src);

int sprint_record(void *db, wg_int *rec, thread_data_p tdata);                   
char* sprint_value(void *db, wg_int enc, thread_data_p tdata);
int sprint_string(char* bptr, int limit, char* strdata, int strenc);
int sprint_blob(char* bptr, int limit, char* strdata, int strenc);
int sprint_append(char** buf, char* str, int l);

char* str_new(int len);
int str_guarantee_space(thread_data_p tdata, int needed);

int load_configuration(char* path, struct dserve_conf *conf);
int add_conf_key_val(struct dserve_conf *conf, char* key, char* val);
int add_slval(struct sized_strlst *lst, char* val);
void print_conf(struct dserve_conf *conf);
void print_conf_slval(struct sized_strlst *lst, char* key);
int authorize(int level,thread_data_p tdata,char* database,char* token);

void print_help(void);
void infoprint(char* fmt, char* param);
void warnprint(char* fmt, char* param);
void errprint(char* fmt, char* param);

char* errhalt(char* str, thread_data_p tdata);
char* err_clear_detach_halt(char* errstr, thread_data_p tdata);

void terminate(void);
void termination_handler(int signal);
void timeout_handler(int signal);
void clear_detach_final(int signal);


#if _MSC_VER
#ifdef SERVEROPTION
void usleep(__int64 usec);
void win_err_handler(LPTSTR lpszFunction);
#endif
#endif
