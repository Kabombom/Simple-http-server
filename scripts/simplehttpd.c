/*
 * -- simplehttpd.c --
 * A (very) simple HTTP server
 *
 * Sistemas Operativos 2014/2015
 */

#include "../includes/header.h"

int main(int argc, char ** argv) {
  struct sockaddr_in client_name;
  socklen_t client_name_len = sizeof(client_name);
  int port;

  create_processes();
  create_shared_memory();
  attach_shared_memory();
  configuration_start();
  create_scheduler_threads();

  /*Testing shared mem
  if (config_pid == 0) {
    config -> serverport = 50001;
    printf("%d\n", config->serverport);
  }
  if (statistics_pid == 0) {
    config -> serverport = 50002;
    printf("%d\n", config->serverport);
  }
  ENDS*/

  signal(SIGINT,catch_ctrlc);

  port = config -> serverport;
  printf("Listening for HTTP requests on port %d\n", port);

  // Configure listening port
  // #If port given is invalid, exit
  if ((socket_conn = fireup(port)) == -1) {
    exit(1);
  }

  // Serve requests
  while (1) {
    // Accept connection on socket
    // #Exit if error occurs while connecting
    if ( (new_conn = accept(socket_conn, (struct sockaddr *)&client_name, &client_name_len)) == -1 ) {
      printf("Error accepting connection\n");
      exit(1);
    }

    // Identify new client by address and port
    identify(new_conn);

    // Process request
    get_request(new_conn);

    // Verify if request is for a page or script
    if(!strncmp(req_buf, CGI_EXPR, strlen(CGI_EXPR))) {
      execute_script(new_conn);
    }
    else {
      // Search file with html page and send to client
      send_page(new_conn);
    }

    // Terminate child processes
    terminate_processes();

    //Clean shared mem
    delete_shared_memory();

    // Terminate connection with client
    close(new_conn);
  }
}

// Processes request from client
void get_request(int socket) {
  int i, j;
  int found_get;

  //GET_EXPR it's the get request
  //strncmp compares the strings not more than strlen(GET_EXPR) characters
  found_get = 0;
  while (read_line(socket, SIZE_BUF) > 0) {
    if(!strncmp(buf, GET_EXPR, strlen(GET_EXPR))) {
      // GET received, extract the requested page/script
      found_get = 1;
      i = strlen(GET_EXPR);
      j = 0;

      while( (buf[i] != ' ') && (buf[i] != '\0') ) {
        req_buf[j++] = buf[i++];
      }
      req_buf[j] = '\0';
    }
  }

  // Currently only supports GET
  if(!found_get) {
    printf("Request from client without a GET\n");
    exit(1);
  }
  // If no particular page is requested then we consider htdocs/index.html
  if(!strlen(req_buf)) {
    sprintf(req_buf,"index.html");
  }

  #if DEBUG
  printf("get_request: client requested the following page: %s\n",req_buf);
  #endif

  return;
}


// Send message header (before html page) to client
void send_header(int socket) {
  #if DEBUG
  printf("send_header: sending HTTP header to client\n");
  #endif
  sprintf(buf, HEADER_1);
  send(socket, buf, strlen(HEADER_1), 0);
  sprintf(buf, SERVER_STRING);
  send(socket, buf, strlen(SERVER_STRING), 0);
  sprintf(buf, HEADER_2);
  send(socket, buf, strlen(HEADER_2), 0);
  return;
}


// Execute script in /cgi-bin
void execute_script(int socket) {
  // Currently unsupported, return error code to client
  cannot_execute(socket);
  return;
}


// Send html page to client
void send_page(int socket) {
  FILE * fp;

  // Searchs for page in directory htdocs
  sprintf(buf_tmp,"htdocs/%s", req_buf);

  #if DEBUG
  printf("send_page: searching for %s\n",buf_tmp);
  #endif

  // Verifies if file exists
  if((fp = fopen(buf_tmp, "rt")) == NULL) {
    // Page not found, send error to client
    printf("send_page: page %s not found, alerting client\n", buf_tmp);
    not_found(socket);
  }
  else {
    // Page found, send to client

    // First send HTTP header back to client
    send_header(socket);

    printf("send_page: sending page %s to client\n", buf_tmp);
    while(fgets(buf_tmp, SIZE_BUF, fp)) {
      send(socket, buf_tmp, strlen(buf_tmp), 0);
    }

    // Close file
    fclose(fp);
  }

  return;
}


// Identifies client (address and port) from socket
void identify(int socket) {
  char ipstr[INET6_ADDRSTRLEN];
  socklen_t len;
  struct sockaddr_in *s;
  int port;
  struct sockaddr_storage addr;

  len = sizeof addr;
  //Returns the address of the peer connected to the specified socket
  getpeername(socket, (struct sockaddr*)&addr, &len);

  // Assuming only IPv4
  //ntohs converts 16bits quantities between network byte order and host byte order.
  //inet_ntop converts an address *src  from network_format to presentation format
  s = (struct sockaddr_in *)&addr;
  port = ntohs(s -> sin_port);
  inet_ntop(AF_INET, &s -> sin_addr, ipstr, sizeof ipstr);

  printf("identify: received new request from %s port %d\n", ipstr, port);
  return;
}


// Reads a line (of at most 'n' bytes) from socket
int read_line(int socket,int n) {
  int n_read;
  int not_eol;
  int ret;
  char new_char;

  n_read = 0;
  not_eol = 1;


  while (n_read<n && not_eol) {
    ret = read(socket, &new_char, sizeof(char));
    if (ret == -1) {
      printf("Error reading from socket (read_line)");
      return -1;
    }
    else if (ret == 0) {
      return 0;
    }
    else if (new_char=='\r') {
      not_eol = 0;
      // consumes next byte on buffer (LF)
      read(socket, &new_char, sizeof(char));
      continue;
    }
    else {
      buf[n_read] = new_char;
      n_read++;
    }
  }

  buf[n_read] = '\0';
  #if DEBUG
  printf("read_line: new line read from client socket: %s\n", buf);
  #endif

  return n_read;
}


// Creates, prepares and returns new socket
int fireup(int port) {
  int new_sock;
  struct sockaddr_in name;

  // Creates socket
  if ((new_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    printf("Error creating socket\n");
    return -1;
  }

  // Binds new socket to listening port
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(new_sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
    printf("Error binding to socket\n");
    return -1;
  }

  // Starts listening on socket
  if (listen(new_sock, 5) < 0) {
    printf("Error listening to socket\n");
    return -1;
  }

  return(new_sock);
}


// Sends a 404 not found status message to client (page not found)
void not_found(int socket) {
  sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,SERVER_STRING);
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"Content-Type: text/html\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"<HTML><TITLE>Not Found</TITLE>\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"<BODY><P>Resource unavailable or nonexistent.\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"</BODY></HTML>\r\n");
  send(socket,buf, strlen(buf), 0);

  return;
}


// Send a 5000 internal server error (script not configured for execution)
void cannot_execute(int socket) {
  sprintf(buf,"HTTP/1.0 500 Internal Server Error\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"Content-type: text/html\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"\r\n");
  send(socket,buf, strlen(buf), 0);
  sprintf(buf,"<P>Error prohibited CGI execution.\r\n");
  send(socket,buf, strlen(buf), 0);

  return;
}


// Closes socket before closing
void catch_ctrlc(int sig) {
  printf("Server terminating\n");
  close(socket_conn);
  terminate_processes();
  delete_shared_memory();
  exit(0);
}

// Creates shared memory
void create_shared_memory() { 
  shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0777);
  if (shmid < 0) {
    perror("Error creating shared memory");
    exit(1);
  }
}

// Attach shared memory
void attach_shared_memory() {
  config = (config_struct *) shmat(shmid, NULL, 0);
  if (config == (void *) - 1) {
    perror("Error attaching shared memory");
    exit(1);
  }
}

// Delete shared memory
void delete_shared_memory() {
  printf("Cleaning shared memory\n");
  shmdt(config);
  shmctl(shmid, IPC_RMID, NULL);
}

void config_function() {
  printf("COnfiguration process %d\n", config_pid);
}

void statistics() {
  printf("Statistics process %d\n", statistics_pid);
}

// Create necessary processes
void create_processes() {
  parent_pid = getpid();

  config_pid = fork();
  if (config_pid < 0) {
    perror("Error creating configuration process\n");
    exit(1);
  } else {
    config_function();
  }
  
  if (getpid() == parent_pid) {

    statistics_pid = fork();
    if (statistics_pid < 0) {
      perror("Error creating stats process\n"); 
      exit(1);
    } else {
      parent_pid = getppid();
      statistics();
    }
  }
}

// Terminate child processes
void terminate_processes() {
  kill(config_pid, SIGKILL);
  kill(statistics_pid, SIGKILL);
}

