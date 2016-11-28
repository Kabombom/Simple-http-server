/*
* A (very) simple HTTP server
* Sistemas Operativos 2016/2017
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

  // Create Buffer
  create_buffer();

  create_semaphores(1);
  //Catch ctr-c
  signal(SIGINT, catch_ctrlc);

  port = config->serverport;

  create_pipe_thread();
  create_scheduler_threads();

  printf("Listening for HTTP requests on port %d\n", port);

  // Configure listening port
  // If port given is invalid, exit
  if ((socket_conn = fireup(port)) == -1) {
    exit(1);
  }

  // Serve requests
  while (1) {
    // Accept connection on socket
    // Exit if error occurs while connecting
    if ( (new_conn = accept(socket_conn, (struct sockaddr *)&client_name, &client_name_len)) == -1 ) {
      printf("Error accepting connection\n");
      exit(1);
    }

    // Identify new client by address and port
    identify(new_conn);

    // Process request
    get_request(new_conn);

    // Add request to buffer if there is space in buffer
    if (requests_buffer->current_size == BUFFER_SIZE) {
      perror("No buffer space available.\n");
      terminate();
      close(new_conn);
      exit(1);
    }
    else {
      Request *req = (Request*) malloc(sizeof(Request));
      req->required_file = req_buf;
      req->get_request_time = time(NULL);
      add_request_to_buffer(req);
    }

  }

  terminate();
  // Terminate connection with client
  close(new_conn);
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

char *get_filename(char *file_path) {
  int i;
  int passed_dot = 0;
  char *filename = (char *) malloc(100*sizeof(char));

  for (i = 0; i < strlen(file_path); i++) {
    if (file_path[i] == 46 && passed_dot == 0) {
      passed_dot++;
    } else if (file_path[i] == 46 && passed_dot > 0) {
      return filename;
    }
    filename[i] = file_path[i];
  }

  return filename;
}

// Execute script in /cgi-bin
void execute_script(int socket, char *required_file) {
  char command[200] = "gzip -d ";
  int run_unzip;
  FILE *fp;

  sprintf(buf_tmp, "htdocs/%s", required_file);
  strcat(command, buf_tmp);

  // Verifies if file exists
  if((fp = fopen(buf_tmp, "rt")) == NULL) {
    // Page not found, send error to client
    printf("send_page: page %s not found, alerting client\n", buf_tmp);
    not_found(socket);
  }
  else {
    run_unzip = system(command);
    char *filename = get_filename(buf_tmp);

    // Page found, send to client
    // First send HTTP header back to client
    send_header(socket);

    printf("send_page: sending page %s to client\n", filename);

    fclose(fp);
    fp = fopen(filename, "rt");

    while(fgets(filename, 100, fp)) {
      send(socket, filename, strlen(filename), 0);
    }

    // Close file
    fclose(fp);
  }

  // cannot_execute(socket);
  return;
}


// Send html page to client
void send_page(int socket, char *required_file) {
  FILE * fp;

  // Searchs for page in directory htdocs
  sprintf(buf_tmp,"htdocs/%s", required_file);

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
  // ntohs converts 16bits quantities between network byte order and host byte order.
  // inet_ntop converts an address *src  from network_format to presentation format
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
  printf(" Server terminating\n");
  close(socket_conn);
  terminate();
  exit(0);
}

// Creates shared memory
void create_shared_memory() {
  shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0777);
  if (shmid < 0) {
    perror("Error creating shared memory.");
    exit(1);
  }
}

// Attach shared memory
void attach_shared_memory() {
  config = (config_struct *) shmat(shmid, NULL, 0);
  if (config == (void *) - 1) {
    perror("Error attaching shared memory.");
    exit(1);
  }
}


// Delete shared memory
void delete_shared_memory() {
  printf("Cleaning shared memory.\n");
  shmdt(config);
  shmctl(shmid, IPC_RMID, NULL);
}


// Statistics process  function
void statistics() {
  printf("Statistics id %d and parent id %d\n", statistics_pid, parent_pid);
}

// Create necessary processes
void create_processes() {
  parent_pid = getpid();

  if ((statistics_pid = fork()) == 0) {
    statistics_pid = getpid();
    printf("%d\n", statistics_pid);
    statistics();
    exit(0);
  } else if (statistics_pid == -1){
    perror("Error creating statistics process\n");
    exit(1);
  }
}

// Terminate child processes
void terminate_processes() {
  printf("Terminating processes...\n");
  kill(statistics_pid, SIGKILL);
}

// Create pool of threads
void create_new_threads(config_struct_aux config_aux) {
  signal(SIGUSR2, terminate_thread);
  int i;
  long ids[atoi(config_aux.change)];

  // Create threads
  for (i = config->thread_pool; i < atoi(config_aux.change); i++) {
    ids[i] = i;
    if (pthread_create(&thread_pool[i], NULL, scheduler_thread_routine, (void *)ids[i]) != 0) {
      perror("Error creating thread");
    }
  }
}

void handle_console_comands(config_struct_aux config_aux) {
  int new_number_of_threads;
  switch (config_aux.option) {
    case 1:
      printf("Option 1 not implemented yet\n");
      break;
    case 2:
      new_number_of_threads = atoi(config_aux.change);

      if(new_number_of_threads > config->thread_pool) {
        thread_pool = realloc(thread_pool, new_number_of_threads);
        create_new_threads(config_aux);
        config->thread_pool = new_number_of_threads;
      }
      else if(new_number_of_threads < config->thread_pool) {
        int i;
        for (i = new_number_of_threads; i < config->thread_pool; i++) {
          if(pthread_kill(thread_pool[i], SIGUSR1) != 0) {
            printf("Error deleting thread\n");
          }
          pthread_join(thread_pool[i], NULL);
        }
        thread_pool = realloc(thread_pool, new_number_of_threads);
        config->thread_pool = new_number_of_threads;
      }
      break;
    case 3:
      printf("Option 3 not implemented yet\n");
      break;
  }
}

void *thread_pipe_routine() {
  signal(SIGUSR1, terminate_thread);
  start_pipe();
  read_from_pipe();
  return NULL;
}

void create_pipe_thread() {
  // Create threads
  if (pthread_create(&pipe_thread, NULL, thread_pipe_routine, (void *)25) != 0) {
    perror("Error creating thread");
  }
}

void start_pipe() {
  printf("----STARTING PIPE----\n");
  // Creates the named pipe if it doesn't exist yet
  if (mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0 && (errno != EEXIST)) {
    perror("Cannot create pipe: ");
    exit(0);
  }
  printf("Named pipe created.\n");
}

void read_from_pipe() {
  printf("----READING PIPE----\n");
  // Opens the pipe for reading
  int fd, pipe_received_values;
  if ((fd = open(PIPE_NAME, O_RDONLY)) < 0) {
      perror("Cannot open pipe for reading: ");
      exit(0);
  }

  config_struct_aux config_aux;
  while(1) {
    if((pipe_received_values = read(fd, &config_aux, sizeof(config_struct_aux))) > 0) {
      printf("=================\n");
      printf("%d %s\n", config_aux.option, config_aux.change);
      printf("=================\n");
      handle_console_comands(config_aux);
    }
  }
}

// Create semaphores
void create_semaphores(int number_of_sems) {
  semid = sem_get(number_of_sems, 1); // Creates a new array of semaphores with init_val = 1
  if (semid == -1) {
    perror("Failed to create semaphores");
    exit(1);
  }
}

// Delete semaphores
void delete_semaphores() {
  printf("Deleting semaphores...\n");
  sem_close(semid);
}

void terminate_thread() {
  printf("Terminating thread...\n");
  pthread_exit(0);
}

// Threads routine
void *scheduler_thread_routine() {
  while(1) {
    sem_wait(semid, 0);
    pthread_mutex_lock(&mutex);
    if (requests_buffer->request != NULL) {
      printf("===================\n");
      printf("\n");
      Request *req = remove_request_from_buffer();
      // Verify if request is for a page or script
      if(!strncmp(req->required_file, CGI_EXPR, strlen(CGI_EXPR))) {
        execute_script(new_conn, req->required_file);
      }
      else {
        // Search file with html page and send to client
        send_page(new_conn, req->required_file);
      }
    }
    pthread_mutex_unlock(&mutex);
    sem_post(semid, 0);
  }
  return NULL;
}

// Create pool of threads
void create_scheduler_threads() {
  signal(SIGUSR1, terminate_thread);
  int i;
  long ids[config -> thread_pool];
  // Create pool of threads
  thread_pool = malloc(sizeof(pthread_t) * config->thread_pool);

  if (thread_pool == NULL) {
    perror("Error allocating memory for threads\n");
  }

  // Create threads
  for (i = 0; i < config->thread_pool; i++) {
    ids[i] = i;
    if (pthread_create(&thread_pool[i], NULL, scheduler_thread_routine, (void *)ids[i]) != 0) {
      perror("Error creating thread");
    }
  }
}

// Delete threads
void delete_scheduler_threads() {
  printf("Cleaning up scheduler...\n");
  int i;
  for (i = 0; i < config->thread_pool; i++) {
    pthread_join(thread_pool[i], NULL);
  }
}

// When program terminates, clean resources
void terminate() {
  int i;

  if(pthread_kill(pipe_thread, SIGUSR1) != 0) {
    printf("Error deleting console thread\n");
  }
  for (i = 0; i < config->thread_pool; i++) {
    if(pthread_kill(thread_pool[i], SIGUSR1) != 0) {
      printf("Error deleting thread\n");
    }
  }

  // To kill a thread use pthread_kill. But that has to be done before pthread_join, otherwise the thread has already exited
  pthread_join(pipe_thread, NULL);
  for (int i = 0; i < config->thread_pool; i++) {
    pthread_join(thread_pool[i], NULL);
  }

  //wait(NULL);
  terminate_processes();
  delete_shared_memory();
  delete_buffer();
  delete_semaphores();
  free(thread_pool);
}
