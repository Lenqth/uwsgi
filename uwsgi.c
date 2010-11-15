/*

 *** uWSGI ***

 Copyright (C) 2009-2010 Unbit S.a.s. <info@unbit.it>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/


#include "uwsgi.h"

struct uwsgi_server uwsgi;

extern char **environ;

static char *short_options = NULL;

static char *base_short_options = "s:p:t:x:d:l:v:b:mcaCTiMhrR:z:A:Q:L:y:";

UWSGI_DECLARE_EMBEDDED_PLUGINS

static struct option long_base_options[] = {
	{"socket", required_argument, 0, 's'},
	{"processes", required_argument, 0, 'p'},
	{"harakiri", required_argument, 0, 't'},
#ifdef UWSGI_XML
	{"xmlconfig", required_argument, 0, 'x'},
#endif
	{"daemonize", required_argument, 0, 'd'},
	{"listen", required_argument, 0, 'l'},
	{"max-vars", required_argument, 0, 'v'},
	{"buffer-size", required_argument, 0, 'b'},
	{"memory-report", no_argument, 0, 'm'},
	{"cgi-mode", no_argument, 0, 'c'},
	{"abstract-socket", no_argument, 0, 'a'},
	{"chmod-socket", optional_argument, 0, 'C'},
#ifdef UWSGI_THREADING
	{"enable-threads", no_argument, 0, 'T'},
#endif
	{"single-interpreter", no_argument, 0, 'i'},
	{"master", no_argument, 0, 'M'},
	{"help", no_argument, 0, 'h'},
	{"reaper", no_argument, 0, 'r'},
	{"max-requests", required_argument, 0, 'R'},
	{"socket-timeout", required_argument, 0, 'z'},
	{"sharedarea", required_argument, 0, 'A'},
#ifdef UWSGI_SPOOLER
	{"spooler", required_argument, 0, 'Q'},
#endif
	{"disable-logging", no_argument, 0, 'L'},

	{"pidfile", required_argument, 0, LONG_ARGS_PIDFILE},
	{"chroot", required_argument, 0, LONG_ARGS_CHROOT},
	{"gid", required_argument, 0, LONG_ARGS_GID},
	{"uid", required_argument, 0, LONG_ARGS_UID},
#ifdef UWSGI_INI
	{"ini", required_argument, 0, LONG_ARGS_INI},
#endif
#ifdef UWSGI_YAML
	{"yaml", required_argument, 0, 'y'},
	{"yml", required_argument, 0, 'y'},
#endif
#ifdef UWSGI_LDAP
	{"ldap", required_argument, 0, LONG_ARGS_LDAP},
	{"ldap-schema", no_argument, 0, LONG_ARGS_LDAP_SCHEMA},
	{"ldap-schema-ldif", no_argument, 0, LONG_ARGS_LDAP_SCHEMA_LDIF},
#endif
	{"no-server", no_argument, &uwsgi.no_server, 1},
	{"no-defer-accept", no_argument, &uwsgi.no_defer_accept, 1},
	{"limit-as", required_argument, 0, LONG_ARGS_LIMIT_AS},
	{"limit-post", required_argument, 0, LONG_ARGS_LIMIT_POST},
	{"no-orphans", no_argument, &uwsgi.no_orphans, 1},
	{"prio", required_argument, 0, LONG_ARGS_PRIO},
	{"post-buffering", required_argument, 0, LONG_ARGS_POST_BUFFERING},
	{"post-buffering-bufsize", required_argument, 0, LONG_ARGS_POST_BUFFERING_SIZE},
	{"upload-progress", required_argument, 0, LONG_ARGS_UPLOAD_PROGRESS},
	{"no-default-app", no_argument, &uwsgi.no_default_app, 1},
#ifdef UWSGI_UDP
	{"udp", required_argument, 0, LONG_ARGS_UDP},
#endif
#ifdef UWSGI_MULTICAST
	{"multicast", required_argument, 0, LONG_ARGS_MULTICAST},
#endif
#ifdef UWSGI_SNMP
	{"snmp", no_argument, 0, LONG_ARGS_SNMP},
	{"snmp-community", required_argument, 0, LONG_ARGS_SNMP_COMMUNITY},
#endif
	{"check-interval", required_argument, 0, LONG_ARGS_CHECK_INTERVAL},

	{"binary-path", required_argument, 0, LONG_ARGS_BINARY_PATH},
#ifdef UWSGI_PROXY
	{"proxy", required_argument, 0, LONG_ARGS_PROXY},
	{"proxy-node", required_argument, 0, LONG_ARGS_PROXY_NODE},
	{"proxy-max-connections", required_argument, 0, LONG_ARGS_PROXY_MAX_CONNECTIONS},
#endif
#ifdef UWSGI_ASYNC
	{"async", required_argument, 0, LONG_ARGS_ASYNC},
#endif
	{"logto", required_argument, 0, LONG_ARGS_LOGTO},
	{"logdate", no_argument, &uwsgi.logdate, 1},
	{"log-zero", no_argument, 0, LONG_ARGS_LOG_ZERO},
	{"log-slow", required_argument, 0, LONG_ARGS_LOG_SLOW},
	{"log-4xx", no_argument, 0, LONG_ARGS_LOG_4xx},
	{"log-5xx", no_argument, 0, LONG_ARGS_LOG_5xx},
	{"log-big", required_argument, 0, LONG_ARGS_LOG_BIG},
	{"log-sendfile", required_argument, 0, LONG_ARGS_LOG_SENDFILE},
	{"chdir", required_argument, 0, LONG_ARGS_CHDIR},
	{"chdir2", required_argument, 0, LONG_ARGS_CHDIR2},
	{"mount", required_argument, 0, LONG_ARGS_MOUNT},
	{"grunt", no_argument, &uwsgi.grunt, 1},
	{"threads", required_argument, 0, LONG_ARGS_THREADS},
	{"vhost", no_argument, &uwsgi.vhost, 1},
	{"vhost-host", no_argument, 0, LONG_ARGS_VHOSTHOST},
#ifdef UWSGI_ROUTING
	{"routing", no_argument, &uwsgi.routing, 1},
#endif

#ifdef UWSGI_HTTP
	{"http", required_argument, 0, LONG_ARGS_HTTP},
	{"http-only", no_argument, &uwsgi.http_only, 1},
	{"http-var", required_argument, 0, LONG_ARGS_HTTP_VAR},
	{"http-modifier1", required_argument, 0, LONG_ARGS_HTTP_MODIFIER1},
#endif
	{"close-on-exec", no_argument, &uwsgi.close_on_exec, 1},
	{"mode", required_argument, 0, LONG_ARGS_MODE},
	{"env", required_argument, 0, LONG_ARGS_ENV},
	{"vacuum", no_argument, &uwsgi.vacuum, 1},
#ifdef __linux__
	{"cgroup", required_argument, 0, LONG_ARGS_CGROUP},
	{"cgroup-opt", required_argument, 0, LONG_ARGS_CGROUP_OPT},
#endif
	{"loop", required_argument, 0, LONG_ARGS_LOOP},
	{"plugins", required_argument, 0, LONG_ARGS_PLUGINS},
	{"remap-modifier", required_argument, 0, LONG_ARGS_REMAP_MODIFIER},
	{"version", no_argument, 0, LONG_ARGS_VERSION},
	{0, 0, 0, 0}
};

int find_worker_id(pid_t pid)
{
	int i;
	for (i = 1; i <= uwsgi.numproc; i++) {
		if (uwsgi.workers[i].pid == pid)
			return i;
	}

	return -1;
}


void warn_pipe()
{
	struct wsgi_request *wsgi_req = current_wsgi_req();

	if (uwsgi.async < 2 && wsgi_req->uri_len > 0) {
		uwsgi_log("SIGPIPE: writing to a closed pipe/socket/fd (probably the client disconnected) on request %.*s !!!\n", wsgi_req->uri_len, wsgi_req->uri);
	} else {
		uwsgi_log("SIGPIPE: writing to a closed pipe/socket/fd (probably the client disconnected) !!!\n");
	}
}

void gracefully_kill()
{
	uwsgi_log("Gracefully killing worker %d...\n", uwsgi.mypid);
	if (UWSGI_IS_IN_REQUEST) {
		uwsgi.workers[uwsgi.mywid].manage_next_request = 0;
	} else {
		reload_me();
	}
}

void reload_me()
{
	exit(UWSGI_RELOAD_CODE);
}

void end_me()
{
	exit(UWSGI_END_CODE);
}

void goodbye_cruel_world()
{
	uwsgi_log("...The work of process %d is done. Seeya!\n", getpid());
	exit(0);
}

void kill_them_all()
{
	int i;
	uwsgi.to_hell = 1;
	uwsgi_log("SIGINT/SIGQUIT received...killing workers...\n");
	for (i = 1; i <= uwsgi.numproc; i++) {
		kill(uwsgi.workers[i].pid, SIGINT);
	}
}

void grace_them_all()
{
	int i;
	uwsgi.to_heaven = 1;
	uwsgi_log("...gracefully killing workers...\n");
	for (i = 1; i <= uwsgi.numproc; i++) {
		kill(uwsgi.workers[i].pid, SIGHUP);
	}
}

void reap_them_all()
{
	int i;
	uwsgi.to_heaven = 1;
	uwsgi_log("...brutally killing workers...\n");
	for (i = 1; i <= uwsgi.numproc; i++) {
		kill(uwsgi.workers[i].pid, SIGTERM);
	}
}

void harakiri()
{

	uwsgi_log("\nF*CK !!! i must kill myself (pid: %d app_id: %d)...\n", uwsgi.mypid, uwsgi.wsgi_req->app_id);

	if (!uwsgi.master_process) {
		uwsgi_log("*** if you want your workers to be automatically respawned consider enabling the uWSGI master process ***\n");
	}
	exit(0);
}

void stats()
{
	//fix this for better logging(this cause races)
	struct uwsgi_app *ua = NULL;
	int i;

	uwsgi_log("*** pid %d stats ***\n", getpid());
	uwsgi_log("\ttotal requests: %llu\n", uwsgi.workers[0].requests);
	for (i = 0; i < uwsgi.apps_cnt; i++) {
		ua = &uwsgi.apps[i];
		if (ua) {
			uwsgi_log("\tapp %d requests: %d\n", i, ua->requests);
		}
	}
	uwsgi_log("\n");
}

void what_i_am_doing()
{

	struct wsgi_request *wsgi_req = current_wsgi_req();

	if (uwsgi.async < 2 && wsgi_req->uri_len > 0) {

		if (uwsgi.shared->options[UWSGI_OPTION_HARAKIRI] > 0 && uwsgi.workers[uwsgi.mywid].harakiri < time(NULL)) {
			uwsgi_log("HARAKIRI: --- uWSGI worker %d (pid: %d) WAS managing request %.*s since %.*s ---\n", (int) uwsgi.mywid, (int) uwsgi.mypid, wsgi_req->uri_len, wsgi_req->uri, 24, ctime((const time_t *) & wsgi_req->start_of_request.tv_sec));
		} else {
			uwsgi_log("SIGUSR2: --- uWSGI worker %d (pid: %d) is managing request %.*s since %.*s ---\n", (int) uwsgi.mywid, (int) uwsgi.mypid, wsgi_req->uri_len, wsgi_req->uri, 24, ctime((const time_t *) & wsgi_req->start_of_request.tv_sec));
		}
	}
}


pid_t masterpid;
struct timeval last_respawn;


int unconfigured_hook(struct wsgi_request * wsgi_req)
{
	uwsgi_log("-- unavailable modifier requested: %d --\n", wsgi_req->uh.modifier1);
	return -1;
}

static void unconfigured_after_hook(struct wsgi_request * wsgi_req)
{
	return;
}

struct uwsgi_plugin unconfigured_plugin = {

	.name = "unconfigured",
	.request = unconfigured_hook,
	.after_request = unconfigured_after_hook,
};

static void vacuum(void)
{

	int i;

	if (uwsgi.vacuum) {
		if (getpid() == masterpid) {
			if (chdir(uwsgi.cwd)) {
				uwsgi_error("chdir()");
			}
			for (i = 0; i < uwsgi.sockets_cnt; i++) {
				if (uwsgi.sockets[i].family == AF_UNIX) {
					if (unlink(uwsgi.sockets[i].name)) {
						uwsgi_error("unlink()");
					} else {
						uwsgi_log("VACUUM: unix socket %s removed.\n", uwsgi.sockets[i].name);
					}
				}
			}
			if (uwsgi.pidfile && !uwsgi.uid) {
				if (unlink(uwsgi.pidfile)) {
					uwsgi_error("unlink()");
				} else {
					uwsgi_log("VACUUM: pidfile removed.\n");
				}
			}
		}
	}
}

int main(int argc, char *argv[], char *envp[])
{

	int i,
	 j;

	int rlen;

	int uwsgi_will_starts = 0;

	pid_t pid;

	FILE *pidfile;

	char *env_reloads;
	unsigned int reloads = 0;
	char env_reload_buf[11];
	
	char *plugins_requested;


#ifdef UWSGI_HTTP
	pid_t http_pid;
#endif

#ifdef UNBIT
	//struct uidsec_struct us;
#endif

	struct sockaddr_un usa;
	struct sockaddr *gsa;
	struct sockaddr_in *isa;
	socklen_t socket_type_len;

#ifdef UWSGI_DEBUG
	struct utsname uuts;
	int so_bufsize;
	socklen_t so_bufsize_len;
#endif

	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	//initialize masterpid with a default value
	masterpid = getpid();

	memset(&uwsgi, 0, sizeof(struct uwsgi_server));
	uwsgi.cwd = uwsgi_get_cwd();

	atexit(vacuum);


#ifdef UWSGI_DEBUG
#ifdef __sun__
	if (uname(&uuts) < 0) {
#else
	if (uname(&uuts)) {
#endif
		uwsgi_error("uname()");
	} else {
		uwsgi_log("SYSNAME: %s\nNODENAME: %s\nRELEASE: %s\nVERSION: %s\nMACHINE: %s\n",
			  uuts.sysname,
			  uuts.nodename,
			  uuts.release,
			  uuts.version,
			  uuts.machine);
	}
#endif


	uwsgi.shared = (struct uwsgi_shared *) mmap(NULL, sizeof(struct uwsgi_shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	if (!uwsgi.shared) {
		uwsgi_error("mmap()");
		exit(1);
	}
	memset(uwsgi.shared, 0, sizeof(struct uwsgi_shared));

#ifdef UWSGI_SPOOLER
	//set the spooler frequency to 30 seconds by default
	uwsgi.shared->spooler_frequency = 30;
#endif

	for (i = 0; i <= 0xFF; i++) {
		uwsgi.p[i] = &unconfigured_plugin;
	}

	uwsgi.cores = 1;

	uwsgi.apps_cnt = 1;
	uwsgi.default_app = -1;

	uwsgi.buffer_size = 4096;
	uwsgi.numproc = 1;

	uwsgi.async = 1;
	uwsgi.listen_queue = 64;

	uwsgi.max_vars = MAX_VARS;
	uwsgi.vec_size = 4 + 1 + (4 * MAX_VARS);

	uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT] = 4;
	uwsgi.shared->options[UWSGI_OPTION_LOGGING] = 1;


	gettimeofday(&uwsgi.start_tv, NULL);

	setlinebuf(stdout);

	uwsgi.rl.rlim_cur = 0;
	uwsgi.rl.rlim_max = 0;


	env_reloads = getenv("UWSGI_RELOADS");
	if (env_reloads) {
		//convert env value to int
		reloads = atoi(env_reloads);
		reloads++;
		//convert reloads to string
		rlen = snprintf(env_reload_buf, 10, "%u", reloads);
		if (rlen > 0) {
			env_reload_buf[rlen] = 0;
			if (setenv("UWSGI_RELOADS", env_reload_buf, 1)) {
				uwsgi_error("setenv()");
			}
		}
		uwsgi.is_a_reload = 1;
	} else {
		if (setenv("UWSGI_RELOADS", "0", 1)) {
			uwsgi_error("setenv()");
		}
	}

	uwsgi.binary_path = argv[0];

	uwsgi_register_loop("simple", simple_loop);
	uwsgi_register_loop("async", complex_loop);

	//initialize embedded plugins
	UWSGI_LOAD_EMBEDDED_PLUGINS

	// now a bit of magic, if the argv[0] contains a _ try to automatically load a plugin
	//uwsgi_log("executable name: %s\n", argv[0]);
	char *p = strtok(argv[0], "_");
	plugins_requested = NULL;
	while (p != NULL) {
		p = strtok(NULL, "_");
		if (p) plugins_requested = p;
	}
	
	if (plugins_requested) {
		uwsgi_log("plugin = %s\n", plugins_requested);
		uwsgi_load_plugin(0, plugins_requested, NULL, 0);
	}

	plugins_requested = getenv("UWSGI_PLUGINS");
	if (plugins_requested) {
		char *p = strtok(plugins_requested, ",");
		while (p != NULL) {
			uwsgi_load_plugin(-1, p, NULL, 0);
			p = strtok(NULL, ",");
		}
	}
	build_options();

	while ((i = getopt_long(argc, argv, short_options, uwsgi.long_options, &uwsgi.option_index)) != -1) {
		if (i == 0) {
			add_exported_option(0, (char *)uwsgi.long_options[uwsgi.option_index].name);
		}
		else {
			manage_opt(i, optarg);
		}
	}



	   if (optind < argc) {
	   	char *lazy = argv[optind];
		if (lazy[0] != '[') {
			if (!strcmp(lazy+strlen(lazy)-4, ".xml")) {
				uwsgi.xml_config = lazy;
			}
#ifdef UWSGI_INI
			else if (!strcmp(lazy+strlen(lazy)-4, ".ini")) {
				uwsgi.ini = lazy;
			}
#endif
#ifdef UWSGI_YAML
			else if (!strcmp(lazy+strlen(lazy)-4, ".yml")) {
				uwsgi.yaml = lazy;
			}
#endif
			// manage magic mountpoint
			else if (lazy[0] == '/' && strchr(lazy,'=')) {
			}
			else {
				int magic = 0;
				for(i =0; i < uwsgi.gp_cnt; i++) {
                			if (uwsgi.gp[i]->magic) {
                        			if (uwsgi.gp[i]->magic(NULL, lazy)) {
							magic = 1;
							break;
						}
                			}
				}
				if (!magic) {
        				for (i = 0; i < 0xFF; i++) {
                				if (uwsgi.p[i]->magic) {
                        				if (uwsgi.p[i]->magic(NULL, lazy)) {
								magic = 1;
								break;
							}
                				}
        				}
				}
			}
		}
	}

#ifdef UWSGI_XML
	if (uwsgi.xml_config != NULL) {
		uwsgi_xml_config(uwsgi.wsgi_req, 0);
	}
#endif
#ifdef UWSGI_INI
	if (uwsgi.ini != NULL) {
		uwsgi_ini_config(uwsgi.ini);
	}
#endif
#ifdef UWSGI_YAML
	if (uwsgi.yaml != NULL) {
		uwsgi_yaml_config(uwsgi.yaml);
	}
#endif
#ifdef UWSGI_LDAP
	if (uwsgi.ldap != NULL) {
		uwsgi_ldap_config();
	}
#endif

	//parse environ
	parse_sys_envs(environ);

	//call after_opt hooks

	if (uwsgi.binary_path == argv[0]) {
		uwsgi.binary_path = malloc(strlen(argv[0]) + 1);
		if (uwsgi.binary_path == NULL) {
			uwsgi_error("malloc()");
			exit(1);
		}
		memcpy(uwsgi.binary_path, argv[0], strlen(argv[0]) + 1);
	}

	if (!uwsgi.no_initial_output) {
		if (uwsgi.shared->options[UWSGI_OPTION_CGI_MODE] == 0) {
			uwsgi_log("*** Starting uWSGI %s (%dbit) on [%.*s] ***\n", UWSGI_VERSION, (int) (sizeof(void *)) * 8, 24, ctime((const time_t *) &uwsgi.start_tv.tv_sec));
		} else {
			uwsgi_log("*** Starting uWSGI %s (CGI mode) (%dbit) on [%.*s] ***\n", UWSGI_VERSION, (int) (sizeof(void *)) * 8, 24, ctime((const time_t *) &uwsgi.start_tv.tv_sec));
		}

#ifdef UWSGI_DEBUG
	uwsgi_log("***\n*** You are running a DEBUG version of uWSGI, plese disable DEBUG in uwsgiconfig.py and recompile it ***\n***\n");
#endif

	uwsgi_log("compiled with version: %s\n", __VERSION__);

#ifdef __BIG_ENDIAN__
	uwsgi_log("*** big endian arch detected ***\n");
#endif

	}

	if (uwsgi.pidfile && !uwsgi.is_a_reload) {
		uwsgi_log("writing pidfile to %s\n", uwsgi.pidfile);
		pidfile = fopen(uwsgi.pidfile, "w");
		if (!pidfile) {
			uwsgi_error("fopen");
			exit(1);
		}
		if (fprintf(pidfile, "%d\n", (int) getpid()) < 0) {
			uwsgi_log("could not write pidfile.\n");
		}
		fclose(pidfile);
	}
	uwsgi_as_root();

	if (!uwsgi.no_initial_output) {
		if (!uwsgi.master_process) {
			uwsgi_log(" *** WARNING: you are running uWSGI without its master process manager ***\n");
		}
	}
#ifndef __OpenBSD__

	if (uwsgi.rl.rlim_max > 0) {
		uwsgi_log("limiting address space of processes...\n");
		if (setrlimit(RLIMIT_AS, &uwsgi.rl)) {
			uwsgi_error("setrlimit()");
		}
	}
	if (uwsgi.prio != 0) {
#ifdef __HAIKU__
		if (set_thread_priority(find_thread(NULL), uwsgi.prio) == B_BAD_THREAD_ID) {
			uwsgi_error("set_thread_priority()");
#else
		if (setpriority(PRIO_PROCESS, 0, uwsgi.prio)) {
			uwsgi_error("setpriority()");
#endif

		} else {
			uwsgi_log("scheduler priority set to %d\n", uwsgi.prio);
		}
	}
	if (!getrlimit(RLIMIT_AS, &uwsgi.rl)) {
		//check for overflow
		if (uwsgi.rl.rlim_max != RLIM_INFINITY && !uwsgi.no_initial_output) {
			uwsgi_log("your process address space limit is %lld bytes (%lld MB)\n", (long long) uwsgi.rl.rlim_max, (long long) uwsgi.rl.rlim_max / 1024 / 1024);
		}
	}
#endif

	uwsgi.page_size = getpagesize();

	if (!uwsgi.no_initial_output) {
		uwsgi_log("your memory page size is %d bytes\n", uwsgi.page_size);
	}

	if (uwsgi.buffer_size > 65536) {
		uwsgi_log("invalid buffer size.\n");
		exit(1);
	}
	sanitize_args();

#ifdef UWSGI_HTTP
	if (uwsgi.http && !uwsgi.is_a_reload) {
		char *tcp_port = strchr(uwsgi.http, ':');
		if (tcp_port) {
			uwsgi.http_server_port = tcp_port + 1;
			uwsgi.http_fd = bind_to_tcp(uwsgi.http, uwsgi.listen_queue, tcp_port);
#ifdef UWSGI_DEBUG
			uwsgi_debug("HTTP FD: %d\n", uwsgi.http_fd);
#endif
		} else {
			uwsgi_log("invalid http address.\n");
			exit(1);
		}

		if (uwsgi.http_fd < 0) {
			uwsgi_log("unable to create http server socket.\n");
			exit(1);
		}
		if (!uwsgi.sockets[0].name) {

			uwsgi.sockets[0].name = malloc(64);
			if (!uwsgi.sockets[0].name) {
				uwsgi_error("malloc()");
				exit(1);
			}
			uwsgi.sockets_cnt++;
			snprintf(uwsgi.sockets[0].name, 64, "%d_%d.sock", (int) time(NULL), (int) getpid());
			uwsgi_log("using %s as uwsgi protocol socket\n", uwsgi.sockets[0].name);
		}
		if (uwsgi.http_only) {
			http_loop();
			//never here
			exit(1);
		}
		http_pid = fork();

		if (http_pid > 0) {
			masterpid = http_pid;
			http_loop();
			//never here
			exit(1);
		} else if (http_pid < 0) {
			uwsgi_error("fork()");
			exit(1);
		}
		if (uwsgi.pidfile && !uwsgi.is_a_reload) {
			uwsgi_log("updating pidfile with pid %d\n", (int) getpid());
			pidfile = fopen(uwsgi.pidfile, "w");
			if (!pidfile) {
				uwsgi_error("fopen");
				exit(1);
			}
			if (fprintf(pidfile, "%d\n", (int) getpid()) < 0) {
				uwsgi_log("could not update pidfile.\n");
			}
			fclose(pidfile);
		}
		close(uwsgi.http_fd);
	}
#endif


	if (uwsgi.async > 1) {
		if (!getrlimit(RLIMIT_NOFILE, &uwsgi.rl)) {
			if ((unsigned long) uwsgi.rl.rlim_cur < (unsigned long) uwsgi.async) {
				uwsgi_log("- your current max open files limit is %lu, this is lower than requested async cores !!! -\n", (unsigned long) uwsgi.rl.rlim_cur);
				if (uwsgi.rl.rlim_cur < uwsgi.rl.rlim_max && (unsigned long) uwsgi.rl.rlim_max > (unsigned long) uwsgi.async) {
					unsigned long tmp_nofile = (unsigned long) uwsgi.rl.rlim_cur;
					uwsgi.rl.rlim_cur = uwsgi.async;
					if (!setrlimit(RLIMIT_NOFILE, &uwsgi.rl)) {
						uwsgi_log("max open files limit reset to %lu\n", (unsigned long) uwsgi.rl.rlim_cur);
						uwsgi.async = uwsgi.rl.rlim_cur;
					} else {
						uwsgi.async = (int) tmp_nofile;
					}
				} else {
					uwsgi.async = uwsgi.rl.rlim_cur;
				}

				uwsgi_log("- async cores set to %d -\n", uwsgi.async);
			}
		}
	}

	uwsgi.wsgi_requests = malloc(sizeof(struct wsgi_request *) * uwsgi.cores);
	if (uwsgi.wsgi_requests == NULL) {
		uwsgi_log("unable to allocate memory for requests.\n");
		exit(1);
	}
	for (i = 0; i < uwsgi.cores; i++) {
		uwsgi.wsgi_requests[i] = malloc(sizeof(struct wsgi_request));
		if (uwsgi.wsgi_requests[i] == NULL) {
			uwsgi_log("unable to allocate memory for requests.\n");
			exit(1);
		}
		memset(uwsgi.wsgi_requests[i], 0, sizeof(struct wsgi_request));
	}

	uwsgi.async_buf = malloc(sizeof(char *) * uwsgi.cores);
	if (!uwsgi.async_buf) {
		uwsgi_error("malloc()");
		exit(1);
	}
	if (uwsgi.post_buffering > 0) {
		uwsgi.async_post_buf = malloc(sizeof(char *) * uwsgi.cores);
		if (!uwsgi.async_post_buf) {
			uwsgi_error("malloc()");
			exit(1);
		}
		if (!uwsgi.post_buffering_bufsize) {
			uwsgi.post_buffering_bufsize = 8192;
		}
	}
	for (i = 0; i < uwsgi.cores; i++) {
		uwsgi.async_buf[i] = malloc(uwsgi.buffer_size);
		if (!uwsgi.async_buf[i]) {
			uwsgi_error("malloc()");
			exit(1);
		}
		if (uwsgi.post_buffering > 0) {
			uwsgi.async_post_buf[i] = malloc(uwsgi.post_buffering_bufsize);
			if (!uwsgi.async_post_buf[i]) {
				uwsgi_error("malloc()");
				exit(1);
			}
		}
	}


	//by default set wsgi_req to the first slot
	uwsgi.wsgi_req = uwsgi.wsgi_requests[0];

	if (uwsgi.cores > 1) {
		uwsgi_log("allocated %llu bytes (%llu KB) for %d cores per worker.\n", (uint64_t) (sizeof(struct wsgi_request) * uwsgi.cores),
			  (uint64_t) ((sizeof(struct wsgi_request) * uwsgi.cores) / 1024),
			  uwsgi.cores);
	}
	if (uwsgi.vhost) {
		uwsgi_log("VirtualHosting mode enabled.\n");
		uwsgi.apps_cnt = 0;
	}

	/* plugin initialization */
	for(i =0; i < uwsgi.gp_cnt; i++) {
		if (uwsgi.gp[i]->init) {
			uwsgi.gp[i]->init();
		}
	}
	for (i = 0; i < 0xFF; i++) {
		if (uwsgi.p[i]->init) {
			uwsgi.p[i]->init();
		}
	}

#ifdef UWSGI_EMBEDDED
	if (uwsgi.sharedareasize > 0) {
#ifndef __OpenBSD__
		uwsgi.sharedareamutex = mmap(NULL, sizeof(pthread_mutexattr_t) + sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		if (!uwsgi.sharedareamutex) {
			uwsgi_error("mmap()");
			exit(1);
		}
#else
		uwsgi_log("***WARNING*** the sharedarea on OpenBSD is not SMP-safe. Beware of race conditions !!!\n");
#endif
		uwsgi.sharedarea = mmap(NULL, uwsgi.page_size * uwsgi.sharedareasize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		if (uwsgi.sharedarea) {
			uwsgi_log("shared area mapped at %p, you can access it with uwsgi.sharedarea* functions.\n", uwsgi.sharedarea);

#ifdef __APPLE__
			memset(uwsgi.sharedareamutex, 0, sizeof(OSSpinLock));
#else
#if !defined(__OpenBSD__) && !defined(__NetBSD__)
			if (pthread_mutexattr_init((pthread_mutexattr_t *) uwsgi.sharedareamutex)) {
				uwsgi_log("unable to allocate mutexattr structure\n");
				exit(1);
			}
			if (pthread_mutexattr_setpshared((pthread_mutexattr_t *) uwsgi.sharedareamutex, PTHREAD_PROCESS_SHARED)) {
				uwsgi_log("unable to share mutex\n");
				exit(1);
			}
			if (pthread_mutex_init((pthread_mutex_t *) uwsgi.sharedareamutex + sizeof(pthread_mutexattr_t), (pthread_mutexattr_t *) uwsgi.sharedareamutex)) {
				uwsgi_log("unable to initialize mutex\n");
				exit(1);
			}
#endif
#endif

		} else {
			uwsgi_error("mmap()");
			exit(1);
		}

	}
#endif



	uwsgi.current_wsgi_req = simple_current_wsgi_req;


	// call nagios here

#ifdef UWSGI_THREADING
	if (uwsgi.has_threads) {
		uwsgi.current_wsgi_req = threaded_current_wsgi_req;
		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->enable_threads)
				uwsgi.p[i]->enable_threads();
		}
	}
#endif

	if (!uwsgi.no_server) {

		//check for inherited sockets
			if (uwsgi.is_a_reload) {
				for (i = 0; i < uwsgi.sockets_cnt; i++) {
					//a bit overengineering
						if (uwsgi.sockets[i].name != NULL) {

						for (j = 3; j < sysconf(_SC_OPEN_MAX); j++) {
							socket_type_len = sizeof(struct sockaddr_un);
							gsa = (struct sockaddr *) & usa;
							if (!getsockname(j, gsa, &socket_type_len)) {
								if (gsa->sa_family == AF_UNIX) {
									if (!strcmp(usa.sun_path, uwsgi.sockets[i].name)) {
										uwsgi.sockets[i].fd = j;
										uwsgi.sockets[i].family = AF_UNIX;
										uwsgi.sockets[i].bound = 1;
										uwsgi.sockets_poll[i].fd = uwsgi.sockets[i].fd;
										uwsgi.sockets_poll[i].events = POLLIN;
										uwsgi_will_starts = 1;
										uwsgi_log("uwsgi socket %d inherited UNIX address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
									}
								} else if (gsa->sa_family == AF_INET) {
									char *computed_addr;
									char computed_port[6];
									isa = (struct sockaddr_in *) & usa;
									char ipv4a[INET_ADDRSTRLEN + 1];
									memset(ipv4a, 0, INET_ADDRSTRLEN + 1);
									memset(computed_port, 0, 6);

									if (snprintf(computed_port, 6, "%d", ntohs(isa->sin_port)) > 0) {
										if (inet_ntop(AF_INET, (const void *__restrict__) &isa->sin_addr.s_addr, ipv4a, INET_ADDRSTRLEN)) {
											if (!strcmp("0.0.0.0", ipv4a)) {
												computed_addr = uwsgi_concat2(":", computed_port);
											} else {
												computed_addr = uwsgi_concat3(ipv4a, ":", computed_port);
											}
											if (!strcmp(computed_addr, uwsgi.sockets[i].name)) {
												uwsgi.sockets[i].fd = j;
												uwsgi.sockets[i].family = AF_INET;
												uwsgi.sockets[i].bound = 1;
												uwsgi.sockets_poll[i].fd = uwsgi.sockets[i].fd;
												uwsgi.sockets_poll[i].events = POLLIN;
												uwsgi_will_starts = 1;
												uwsgi_log("uwsgi socket %d inherited INET address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
											}
											free(computed_addr);
										}
									}
								}
							}
						}
					}
				}

				//now close all the unbound fd
					for (j = 3; j < sysconf(_SC_OPEN_MAX); j++) {
					int useless = 1;
					socket_type_len = sizeof(struct sockaddr_un);
					gsa = (struct sockaddr *) & usa;
					if (!getsockname(j, gsa, &socket_type_len)) {
						for (i = 0; i < uwsgi.sockets_cnt; i++) {
							if (uwsgi.sockets[i].fd == j && uwsgi.sockets[i].bound) {
								useless = 0;
								break;
							}
						}
					}
					if (useless)
						close(j);
				}
			}
		//now bind all the unbound sockets
			for (i = 0; i < uwsgi.sockets_cnt; i++) {
			if (!uwsgi.sockets[i].bound) {
				char *tcp_port = strchr(uwsgi.sockets[i].name, ':');
				if (tcp_port == NULL) {
					uwsgi.sockets[i].fd = bind_to_unix(uwsgi.sockets[i].name, uwsgi.listen_queue, uwsgi.chmod_socket, uwsgi.abstract_socket);
					uwsgi.sockets[i].family = AF_UNIX;
					uwsgi_log("uwsgi socket %d bound to UNIX address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
				} else {
					uwsgi.sockets[i].fd = bind_to_tcp(uwsgi.sockets[i].name, uwsgi.listen_queue, tcp_port);
					uwsgi.sockets[i].family = AF_INET;
					uwsgi_log("uwsgi socket %d bound to TCP address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
				}

				if (uwsgi.sockets[i].fd < 0) {
					uwsgi_log("unable to create server socket on: %s\n", uwsgi.sockets[i].name);
					exit(1);
				}
			}
			uwsgi.sockets[i].bound = 1;
			uwsgi.sockets_poll[i].fd = uwsgi.sockets[i].fd;
			uwsgi.sockets_poll[i].events = POLLIN;
			uwsgi_will_starts = 1;
		}

		int zero_used = 0;
		for (i = 0; i < uwsgi.sockets_cnt; i++) {
			if (uwsgi.sockets[i].bound && uwsgi.sockets[i].fd == 0) {
				zero_used = 1;
				break;
			}
		}

		if (!zero_used) {
			socket_type_len = sizeof(struct sockaddr_un);
			gsa = (struct sockaddr *) & usa;
			if (!getsockname(0, gsa, &socket_type_len)) {
				if (uwsgi.sockets_cnt < 8) {
					uwsgi.sockets_cnt++;
					uwsgi.sockets[uwsgi.sockets_cnt - 1].fd = 0;
					uwsgi.sockets[uwsgi.sockets_cnt - 1].bound = 1;
					uwsgi.sockets[uwsgi.sockets_cnt - 1].family = gsa->sa_family;
					//uwsgi.sockets[uwsgi.sockets_cnt - 1].name = uwsgi_get_socket_name(gsa->sa_family, gsa);
					uwsgi.sockets_poll[uwsgi.sockets_cnt - 1].fd = 0;
					uwsgi.sockets_poll[uwsgi.sockets_cnt - 1].events = POLLIN;
					uwsgi_will_starts = 1;
					uwsgi_log("uwsgi socket %d inherited INET address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
				} else {
					uwsgi_log("too many socket defined, i cannot map fd 0\n");
				}
			} else {
				int fd = open("/dev/null", O_RDONLY);
				if (fd < 0) {
					uwsgi_error("open()");
					exit(1);
				}
				if (fd != 0) {
					if (dup2(fd, 0)) {
						uwsgi_error("dup2()");
						exit(1);
					}
				}
			}

		}
#ifdef UWSGI_PROXY
		if (uwsgi.proxy_socket_name) {
			uwsgi.shared->proxy_pid = proxy_start(uwsgi.master_process);
			uwsgi_will_starts = 1;
		}
#endif

#ifdef UWSGI_UDP
		if (uwsgi.udp_socket) {
			uwsgi_will_starts = 1;
		}
#endif


	}
	if (!uwsgi_will_starts && !uwsgi.no_server) {
		uwsgi_log("The -s/--socket option is missing and stdin is not a socket.\n");
		exit(1);
	}
#ifdef UWSGI_DEBUG
	for (i = 0; i < uwsgi.sockets_cnt; i++) {
		so_bufsize_len = sizeof(int);
		if (getsockopt(uwsgi.sockets[i].fd, SOL_SOCKET, SO_RCVBUF, &so_bufsize, &so_bufsize_len)) {
			uwsgi_error("getsockopt()");
		} else {
			uwsgi_debug("uwsgi socket %d SO_RCVBUF size: %d\n", i, so_bufsize);
		}

		so_bufsize_len = sizeof(int);
		if (getsockopt(uwsgi.sockets[i].fd, SOL_SOCKET, SO_SNDBUF, &so_bufsize, &so_bufsize_len)) {
			uwsgi_error("getsockopt()");
		} else {
			uwsgi_debug("uwsgi socket %d SO_SNDBUF size: %d\n", i, so_bufsize);
		}
	}
#endif

#ifdef UWSGI_ASYNC
	if (uwsgi.async > 1) {
#ifdef __linux__
		uwsgi.async_events = malloc(sizeof(struct epoll_event) * uwsgi.async);
#elif defined(__sun__)
		uwsgi.async_events = malloc(sizeof(struct pollfd) * uwsgi.async);
#else
		uwsgi.async_events = malloc(sizeof(struct kevent) * uwsgi.async);
#endif
		if (!uwsgi.async_events) {
			uwsgi_error("malloc()");
			exit(1);
		}
	}
#endif




#ifndef UNBIT
	uwsgi_log("your server socket listen backlog is limited to %d connections\n", uwsgi.listen_queue);
#endif


	memset(uwsgi.apps, 0, sizeof(uwsgi.apps));

	uwsgi.workers = (struct uwsgi_worker *) mmap(NULL, sizeof(struct uwsgi_worker) * (uwsgi.numproc + 1 + uwsgi.grunt), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	if (!uwsgi.workers) {
		uwsgi_error("mmap()");
		exit(1);
	}
	memset(uwsgi.workers, 0, sizeof(struct uwsgi_worker) * uwsgi.numproc + 1);

	uwsgi.mypid = getpid();
	masterpid = uwsgi.mypid;

	if (uwsgi.cores > 1) {
		for (i = 1; i < uwsgi.numproc + 1; i++) {
			uwsgi.workers[i].cores = (struct uwsgi_core **) mmap(NULL, sizeof(struct uwsgi_core *) * uwsgi.cores, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
			if (!uwsgi.workers[i].cores) {
				uwsgi_error("mmap()");
				exit(1);
			}
			memset(uwsgi.workers[i].cores, 0, sizeof(struct uwsgi_core *) * uwsgi.cores);

			for (j = 0; j < uwsgi.cores; j++) {
				uwsgi.workers[i].cores[j] = (struct uwsgi_core *) mmap(NULL, sizeof(struct uwsgi_core), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
				if (!uwsgi.workers[i].cores[j]) {
					uwsgi_error("mmap()");
					exit(1);
				}
				memset(uwsgi.workers[i].cores[j], 0, sizeof(struct uwsgi_core));
			}
		}
	}
	uwsgi.workers[0].pid = masterpid;

	/*

	   uwsgi.shared->hooks[0] = uwsgi_request_wsgi;
	   uwsgi.shared->after_hooks[0] = uwsgi_after_request_wsgi;

	   uwsgi.shared->hooks[UWSGI_MODIFIER_ADMIN_REQUEST] = uwsgi_request_admin;	//10
#ifdef UWSGI_SPOOLER
uwsgi.shared->hooks[UWSGI_MODIFIER_SPOOL_REQUEST] = uwsgi_request_spooler;	//17
#endif
uwsgi.shared->hooks[UWSGI_MODIFIER_EVAL] = uwsgi_request_eval;	//22
uwsgi.shared->hooks[UWSGI_MODIFIER_FASTFUNC] = uwsgi_request_fastfunc;	//26

uwsgi.shared->hooks[UWSGI_MODIFIER_MANAGE_PATH_INFO] = uwsgi_request_wsgi;	// 30
uwsgi.shared->after_hooks[UWSGI_MODIFIER_MANAGE_PATH_INFO] = uwsgi_after_request_wsgi;	// 30

uwsgi.shared->hooks[UWSGI_MODIFIER_MESSAGE_MARSHAL] = uwsgi_request_marshal;	//33
uwsgi.shared->hooks[UWSGI_MODIFIER_PING] = uwsgi_request_ping;	//100
*/

	uwsgi_log("*** Operational MODE: ");
	if (uwsgi.threads > 1) {
		if (uwsgi.numproc > 1) {
			uwsgi_log("preforking+threaded");
		} else {
			uwsgi_log("threaded");
		}
	}
#ifdef UWSGI_ASYNC
	else if (uwsgi.async > 1) {
		if (uwsgi.numproc > 1) {
			uwsgi_log("preforking+async");
		} else {
			uwsgi_log("async");
		}
	}
#endif
	else if (uwsgi.numproc > 1) {
		uwsgi_log("preforking");
	} else {
		uwsgi_log("single process");
	}

	uwsgi_log(" ***\n");

#ifdef UWSGI_ERLANG
	if (uwsgi.erlang_node) {
		uwsgi.erlang_nodes = 1;
		uwsgi.erlangfd = init_erlang(uwsgi.erlang_node, uwsgi.erlang_cookie);
	}
#endif

#ifdef UWSGI_SNMP
	if (uwsgi.snmp) {
		//snmp_init();
	}
#endif


	//init apps hook
	for (i = 0; i < 0xFF; i++) {
		if (uwsgi.p[i]->init_apps) {
			uwsgi.p[i]->init_apps();
		}
	}

	/*parse xml for <app> tags */
#ifdef UWSGI_XML
	if (uwsgi.xml_round2 && uwsgi.xml_config != NULL) {
		uwsgi_xml_config(uwsgi.wsgi_req, 1);
	}
#endif

	for (i = 0; i < uwsgi.mounts_cnt; i++) {
		char *what = strchr(uwsgi.mounts[i], '=');
		if (what) {
			what[0] = 0;
			what++;
			uwsgi.wsgi_req->script_name = uwsgi.mounts[i];
			uwsgi.wsgi_req->script_name_len = strlen(uwsgi.mounts[i]);
			//init_uwsgi_app(LOADER_MOUNT, what, uwsgi.wsgi_req, 0);
		} else {
			uwsgi_log("invalid mountpoint: %s\n", uwsgi.mounts[i]);
			exit(1);
		}
	}


	if (uwsgi.no_server) {
		uwsgi_log("no-server mode requested. Goodbye.\n");
		exit(0);
	}
	//is this a proxy only worker ?

	if (!uwsgi.master_process && uwsgi.numproc == 0) {
		exit(0);
	}
	if (!uwsgi.single_interpreter) {
		uwsgi_log("*** uWSGI is running in multiple interpreter mode ***\n");
	}

	if (uwsgi.master_process) {
		if (uwsgi.is_a_reload) {
			uwsgi_log("gracefully (RE)spawned uWSGI master process (pid: %d)\n", uwsgi.mypid);
		} else {
			uwsgi_log("spawned uWSGI master process (pid: %d)\n", uwsgi.mypid);
		}
	}
#ifdef UWSGI_SPOOLER
	if (uwsgi.spool_dir != NULL && uwsgi.numproc > 0) {
		uwsgi.shared->spooler_pid = spooler_start();
	}
#endif

#ifdef UWSGI_ROUTING
	routing_setup();
#endif

	if (!uwsgi.master_process) {
		if (uwsgi.numproc == 1) {
			uwsgi_log("spawned uWSGI worker 1 (and the only) (pid: %d, cores: %d)\n", masterpid, uwsgi.cores);
		} else {
			uwsgi_log("spawned uWSGI worker 1 (pid: %d, cores: %d)\n", masterpid, uwsgi.cores);
		}
		uwsgi.workers[1].pid = masterpid;
		uwsgi.workers[1].id = 1;
		uwsgi.workers[1].last_spawn = time(NULL);
		uwsgi.workers[1].manage_next_request = 1;
		uwsgi.mywid = 1;
		gettimeofday(&last_respawn, NULL);
		uwsgi.respawn_delta = last_respawn.tv_sec;
	}
	for (i = 2 - uwsgi.master_process; i < uwsgi.numproc + 1; i++) {
		pid = fork();
		if (pid == 0) {
			uwsgi.mypid = getpid();
			uwsgi.workers[i].pid = uwsgi.mypid;
			uwsgi.workers[i].id = i;
			uwsgi.workers[i].last_spawn = time(NULL);
			uwsgi.workers[i].manage_next_request = 1;
			uwsgi.mywid = i;
			break;
		} else if (pid < 1) {
			uwsgi_error("fork()");
			exit(1);
		} else {
			uwsgi_log("spawned uWSGI worker %d (pid: %d, cores: %d)\n", i, pid, uwsgi.cores);
			gettimeofday(&last_respawn, NULL);
			uwsgi.respawn_delta = last_respawn.tv_sec;
		}
	}


	if (getpid() == masterpid && uwsgi.master_process == 1) {
		master_loop(argv, environ);
		//from now on the process is a real worker
	}
	for (i = 0; i < 0xFF; i++) {
		if (uwsgi.p[i]->post_fork) {
			uwsgi.p[i]->post_fork();
		}
	}

	//postpone the queue initialization as kevent
	//do not pass kfd after fork()
#ifdef UWSGI_ASYNC
	if (uwsgi.async > 1) {
		uwsgi.async_queue = async_queue_init(uwsgi.sockets[0].fd);
		if (uwsgi.async_queue < 0) {
			exit(1);
		}
	}
#endif



	uwsgi.async_hvec = malloc(sizeof(struct iovec *) * uwsgi.cores);
	if (uwsgi.async_hvec == NULL) {
		uwsgi_log("unable to allocate memory for iovec.\n");
		exit(1);
	}
	for (i = 0; i < uwsgi.cores; i++) {
		uwsgi.async_hvec[i] = malloc(sizeof(struct iovec) * uwsgi.vec_size);
	}

	if (uwsgi.shared->options[UWSGI_OPTION_HARAKIRI] > 0 && !uwsgi.master_process) {
		signal(SIGALRM, (void *) &harakiri);
	}
	signal(SIGHUP, (void *) &gracefully_kill);
	signal(SIGINT, (void *) &end_me);
	signal(SIGTERM, (void *) &reload_me);


	signal(SIGUSR1, (void *) &stats);

	signal(SIGUSR2, (void *) &what_i_am_doing);


	signal(SIGPIPE, (void *) &warn_pipe);

	//initialization done

	if (uwsgi.chdir2) {
		if (chdir(uwsgi.chdir2)) {
			uwsgi_error("chdir()");
			exit(1);
		}
	}
#ifdef __linux__
	if (uwsgi.master_process && uwsgi.no_orphans) {
		if (prctl(PR_SET_PDEATHSIG, SIGINT)) {
			uwsgi_error("prctl()");
		}
	}
#endif


#ifdef UWSGI_ERLANG
	if (uwsgi.erlang_nodes > 0) {
		if (uwsgi.numproc <= uwsgi.erlang_nodes) {
			uwsgi_log("You do not have enough worker for Erlang. Please respawn with at least %d processes.\n", uwsgi.erlang_nodes + 1);
		} else if (uwsgi.mywid > (uwsgi.numproc - uwsgi.erlang_nodes)) {
			uwsgi_log("Erlang mode enabled for worker %d.\n", uwsgi.mywid);
			erlang_loop(uwsgi.wsgi_req);
			//NEVER HERE
				exit(1);
		}
	}
	close(uwsgi.erlangfd);
#endif

#ifdef UWSGI_ASYNC
		uwsgi.async_running = -1;
#endif



	//re - initialize wsgi_req(can be full of init_uwsgi_app data)
	for (i = 0; i < uwsgi.cores; i++) {
		memset(uwsgi.wsgi_requests[i], 0, sizeof(struct wsgi_request));
	}


	// eventually remap plugins
	if (uwsgi.remap_modifier) {
		char *map = strtok(uwsgi.remap_modifier, ",");
                while (map != NULL) {
			char *colon = strchr(map, ':');
			if (colon) {
				colon[0] = 0;
				int rm_src = atoi(map);
				int rm_dst = atoi(colon+1);
				uwsgi.p[rm_dst] = uwsgi.p[rm_src]; 
			}	
                        map = strtok(NULL, ",");
                }
	}

	if (uwsgi.loop) {
		void (*u_loop) (void) = uwsgi_get_loop(uwsgi.loop);
		uwsgi_log("running %s loop %p\n", uwsgi.loop, u_loop);
		u_loop();
		uwsgi_log("done\n");
		goto end;
	} else {
		if (uwsgi.threads > 1) {
			pthread_attr_t pa;
			pthread_t *a_thread;
			int ret;

			ret = pthread_attr_init(&pa);
			if (ret) {
				uwsgi_log("pthread_attr_init() = %d\n", ret);
				exit(1);
			}
			ret = pthread_attr_setdetachstate(&pa, PTHREAD_CREATE_DETACHED);
			if (ret) {
				uwsgi_log("pthread_attr_setdetachstate() = %d\n", ret);
				exit(1);
			}
			if (pthread_key_create(&uwsgi.tur_key, NULL)) {
				uwsgi_error("pthread_key_create()");
				exit(1);
			}
			for (i = 1; i < uwsgi.threads; i++) {
				long j = i;
				a_thread = malloc(sizeof(pthread_t));
				pthread_create(a_thread, &pa, simple_loop, (void *) j);
			}
		}

		if (uwsgi.async < 2) {
			long y = 0;
			simple_loop((void *) y);
		} else {
			complex_loop();
		}

	}

end:
	if (uwsgi.workers[uwsgi.mywid].manage_next_request == 0) {
		reload_me();
	} else {
		goodbye_cruel_world();
	}

	// never here
	return 0;
	}

#ifdef UWSGI_PROXY
	pid_t proxy_start(int has_master) {

		pid_t pid;

		char *tcp_port = strchr(uwsgi.proxy_socket_name, ':');

		if (tcp_port == NULL) {
			uwsgi.proxyfd = bind_to_unix(uwsgi.proxy_socket_name, UWSGI_LISTEN_QUEUE, uwsgi.chmod_socket, uwsgi.abstract_socket);
		} else {
			uwsgi.proxyfd = bind_to_tcp(uwsgi.proxy_socket_name, UWSGI_LISTEN_QUEUE, tcp_port);
			tcp_port[0] = ':';
		}

		if (uwsgi.proxyfd < 0) {
			uwsgi_log("unable to create the server socket.\n");
			exit(1);
		}
		if (!has_master && uwsgi.numproc == 0) {
			uwsgi_proxy(uwsgi.proxyfd);
			//never here
				exit(1);
		} else {
			pid = fork();
			if (pid < 0) {
				uwsgi_error("fork()");
				exit(1);
			} else if (pid > 0) {
				close(uwsgi.proxyfd);
				return pid;
				//continue with uWSGI spawn...
			} else {
				uwsgi_proxy(uwsgi.proxyfd);
				//never here
					exit(1);
			}
		}
	}
#endif

	static int manage_base_opt(int i, char *optarg) {

		char *p;

		switch (i) {

		case 0:
			return 1;
		case LONG_ARGS_VHOSTHOST:
			uwsgi.vhost = 1;
			uwsgi.vhost_host = 1;
			return 1;
		case LONG_ARGS_LOOP:
			uwsgi.loop = optarg;
			return 1;
		case LONG_ARGS_REMAP_MODIFIER:
			uwsgi.remap_modifier = optarg;
			return 1;
		case LONG_ARGS_PLUGINS:
			p = strtok(optarg, ",");
			while (p != NULL) {
#ifdef UWSGI_DEBUG
				uwsgi_debug("loading plugin %s\n", p);
#endif
				uwsgi_load_plugin(-1, p, NULL, 0);
				p = strtok(NULL, ",");
			}
			 build_options();
			return 1;
		case LONG_ARGS_CHDIR:
			if (chdir(optarg)) {
				uwsgi_error("chdir()");
				exit(1);
			}
			return 1;
		case LONG_ARGS_CHDIR2:
			uwsgi.chdir2 = optarg;
			return 1;
#ifdef UWSGI_HTTP
		case LONG_ARGS_HTTP:
			uwsgi.http = optarg;
			return 1;
#endif
#ifdef UWSGI_LDAP
		case LONG_ARGS_LDAP:
			uwsgi.ldap = optarg;
			return 1;
		case LONG_ARGS_LDAP_SCHEMA:
			uwsgi_ldap_schema_dump();
			return 1;
		case LONG_ARGS_LDAP_SCHEMA_LDIF:
			uwsgi_ldap_schema_dump_ldif();
			return 1;
#endif
		case LONG_ARGS_MODE:
			uwsgi.mode = optarg;
			return 1;
		case LONG_ARGS_ENV:
			if (putenv(optarg)) {
				uwsgi_error("putenv()");
			}
			return 1;
#ifdef UWSGI_THREADING
		case LONG_ARGS_THREADS:
			uwsgi.threads = atoi(optarg);
			return 1;
#endif
#ifdef UWSGI_ASYNC
		case LONG_ARGS_ASYNC:
			uwsgi.async = atoi(optarg);
			return 1;
#endif
		case LONG_ARGS_LOGTO:
			logto(optarg);
			return 1;
		case LONG_ARGS_VERSION:
			fprintf(stdout, "uWSGI %s\n", UWSGI_VERSION);
			exit(0);
#ifdef UWSGI_SNMP
		case LONG_ARGS_SNMP:
			uwsgi.snmp = 1;
			return 1;
		case LONG_ARGS_SNMP_COMMUNITY:
			uwsgi.snmp = 1;
			uwsgi.snmp_community = optarg;
			return 1;
#endif
		case LONG_ARGS_PIDFILE:
			uwsgi.pidfile = optarg;
			return 1;
#ifdef UWSGI_UDP
		case LONG_ARGS_UDP:
			uwsgi.udp_socket = optarg;
			uwsgi.master_process = 1;
			return 1;
#endif
#ifdef UWSGI_MULTICAST
		case LONG_ARGS_MULTICAST:
			uwsgi.multicast_group = optarg;
			uwsgi.master_process = 1;
			return 1;
#endif
		case LONG_ARGS_CHROOT:
			uwsgi.chroot = optarg;
			return 1;
		case LONG_ARGS_GID:
			uwsgi.gid = atoi(optarg);
			if (!uwsgi.gid) {
				struct group *ugroup = getgrnam(optarg);
				if (ugroup) {
					uwsgi.gid = ugroup->gr_gid;
				} else {
					uwsgi_log("group %s not found.\n", optarg);
					exit(1);
				}
			}
			return 1;
		case LONG_ARGS_UID:
			uwsgi.uid = atoi(optarg);
			if (!uwsgi.uid) {
				struct passwd *upasswd = getpwnam(optarg);
				if (upasswd) {
					uwsgi.uid = upasswd->pw_uid;
				} else {
					uwsgi_log("user %s not found.\n", optarg);
					exit(1);
				}
			}
			return 1;
		case LONG_ARGS_BINARY_PATH:
			uwsgi.binary_path = optarg;
			return 1;
#ifdef UWSGI_PROXY
		case LONG_ARGS_PROXY_NODE:
			uwsgi_cluster_add_node(optarg, 1);
			return 1;
		case LONG_ARGS_PROXY:
			uwsgi.proxy_socket_name = optarg;
			return 1;
#endif
#ifdef UWSGI_ERLANG
		case LONG_ARGS_ERLANG:
			uwsgi.erlang_node = optarg;
			return 1;
		case LONG_ARGS_ERLANG_COOKIE:
			uwsgi.erlang_cookie = optarg;
			return 1;
#endif
#ifdef UWSGI_HTTP
		case LONG_ARGS_HTTP_VAR:
			if (uwsgi.http_vars_cnt < 63) {
				uwsgi.http_vars[uwsgi.http_vars_cnt] = optarg;
				uwsgi.http_vars_cnt++;
			} else {
				uwsgi_log("you can specify at most 64 --http-var options\n");
			}
			return 1;
		case LONG_ARGS_HTTP_MODIFIER1:
			uwsgi.http_modifier1 = (uint8_t) atoi(optarg);
			return 1;
#endif
#ifdef __linux__
		case LONG_ARGS_CGROUP:
			uwsgi.cgroup = optarg;
			return 1;
		case LONG_ARGS_CGROUP_OPT:
			if (uwsgi.cgroup_opt_cnt < 63) {
				uwsgi.cgroup_opt[uwsgi.cgroup_opt_cnt] = optarg;
				uwsgi.cgroup_opt_cnt++;
			} else {
				uwsgi_log("you can specify at most 64 --cgroup_opt options\n");
			}
			return 1;
#endif
		case LONG_ARGS_LIMIT_AS:
			uwsgi.rl.rlim_cur = (atoi(optarg)) * 1024 * 1024;
			uwsgi.rl.rlim_max = uwsgi.rl.rlim_cur;
			return 1;
		case LONG_ARGS_LIMIT_POST:
			uwsgi.limit_post = (int) strtol(optarg, NULL, 10);
			return 1;
		case LONG_ARGS_PRIO:
			uwsgi.prio = (int) strtol(optarg, NULL, 10);
			return 1;
		case LONG_ARGS_POST_BUFFERING:
			uwsgi.post_buffering = atoi(optarg);
			return 1;
		case LONG_ARGS_POST_BUFFERING_SIZE:
			uwsgi.post_buffering_bufsize = atoi(optarg);
			return 1;
		case LONG_ARGS_UPLOAD_PROGRESS:
			uwsgi.upload_progress = optarg;
			return 1;
#ifdef UWSGI_YAML
		case 'y':
			uwsgi.yaml = optarg;
			return 1;
#endif
#ifdef UWSGI_INI
		case LONG_ARGS_INI:
			uwsgi.ini = optarg;
			return 1;
/*
		case LONG_ARGS_EVAL_CONFIG:
			uwsgi.eval = optarg;
			return 1;
*/
#ifdef UWSGI_PASTE
		case LONG_ARGS_INI_PASTE:
			uwsgi.ini = optarg;
			if (uwsgi.ini[0] != '/') {
				uwsgi.paste = malloc(7 + strlen(uwsgi.cwd) + 1 + strlen(uwsgi.ini) + 1);
				if (uwsgi.paste == NULL) {
					uwsgi_error("malloc()");
					exit(1);
				}
				memset(uwsgi.paste, 0, 7 + strlen(uwsgi.cwd) + strlen(uwsgi.ini) + 1);
				memcpy(uwsgi.paste, "config:", 7);
				memcpy(uwsgi.paste + 7, uwsgi.cwd, strlen(uwsgi.cwd));
				uwsgi.paste[7 + strlen(uwsgi.cwd)] = '/';
				memcpy(uwsgi.paste + 7 + strlen(uwsgi.cwd) + 1, uwsgi.ini, strlen(uwsgi.ini));
			} else {
				uwsgi.paste = malloc(7 + strlen(uwsgi.ini) + 1);
				if (uwsgi.paste == NULL) {
					uwsgi_error("malloc()");
					exit(1);
				}
				memset(uwsgi.paste, 0, 7 + strlen(uwsgi.ini) + 1);
				memcpy(uwsgi.paste, "config:", 7);
				memcpy(uwsgi.paste + 7, uwsgi.ini, strlen(uwsgi.ini));
			}
			return 1;
#endif
#endif
#ifdef UWSGI_PASTE
		case LONG_ARGS_PASTE:
			uwsgi.paste = optarg;
			return 1;
#endif
		case LONG_ARGS_CHECK_INTERVAL:
			uwsgi.shared->options[UWSGI_OPTION_MASTER_INTERVAL] = atoi(optarg);
			return 1;
		case 'A':
			uwsgi.sharedareasize = atoi(optarg);
			return 1;
		case 'L':
			uwsgi.shared->options[UWSGI_OPTION_LOGGING] = 0;
			return 1;
		case LONG_ARGS_LOG_ZERO:
			uwsgi.shared->options[UWSGI_OPTION_LOG_ZERO] = 1;
			return 1;
		case LONG_ARGS_LOG_SLOW:
			uwsgi.shared->options[UWSGI_OPTION_LOG_SLOW] = atoi(optarg);
			return 1;
		case LONG_ARGS_LOG_4xx:
			uwsgi.shared->options[UWSGI_OPTION_LOG_4xx] = 1;
			return 1;
		case LONG_ARGS_LOG_5xx:
			uwsgi.shared->options[UWSGI_OPTION_LOG_5xx] = 1;
			return 1;
		case LONG_ARGS_LOG_BIG:
			uwsgi.shared->options[UWSGI_OPTION_LOG_BIG] = atoi(optarg);
			return 1;
		case LONG_ARGS_LOG_SENDFILE:
			uwsgi.shared->options[UWSGI_OPTION_LOG_SENDFILE] = 1;
			return 1;
		case LONG_ARGS_MOUNT:
			if (uwsgi.mounts_cnt < MAX_APPS) {
				uwsgi.mounts[uwsgi.mounts_cnt] = optarg;
				uwsgi.mounts_cnt++;
			} else {
				uwsgi_log("you can specify at most %d --mount options\n", MAX_APPS);
			}
			return 1;
#ifdef UWSGI_SPOOLER
		case 'Q':
			uwsgi.spool_dir = malloc(PATH_MAX);
			if (!uwsgi.spool_dir) {
				uwsgi_error("malloc()");
				exit(1);
			}
			if (access(optarg, R_OK | W_OK | X_OK)) {
				uwsgi_error("[spooler directory] access()");
				exit(1);
			}
			if (!realpath(optarg, uwsgi.spool_dir)) {
				uwsgi_error("realpath()");
				exit(1);
			}
			uwsgi.master_process = 1;
			return 1;
#endif

		case 'd':
			if (!uwsgi.is_a_reload) {
				daemonize(optarg);
			}
			return 1;
		case 's':
			if (uwsgi.sockets_cnt < 8) {
				uwsgi.sockets[uwsgi.sockets_cnt].name = optarg;
				uwsgi.sockets_cnt++;
			} else {
				uwsgi_log("you can specify at most 8 --socket options\n");
			}
			return 1;
#ifdef UWSGI_XML
		case 'x':
			uwsgi.xml_config = optarg;
			return 1;
#endif
		case 'l':
			uwsgi.listen_queue = atoi(optarg);
			return 1;
		case 'v':
			uwsgi.max_vars = atoi(optarg);
			uwsgi.vec_size = 4 + 1 + (4 * uwsgi.max_vars);
			return 1;
		case 'p':
			uwsgi.numproc = atoi(optarg);
			return 1;
		case 'r':
			uwsgi.shared->options[UWSGI_OPTION_REAPER] = 1;
			return 1;
		case 'm':
			uwsgi.shared->options[UWSGI_OPTION_MEMORY_DEBUG] = 1;
			return 1;
		case 't':
			uwsgi.shared->options[UWSGI_OPTION_HARAKIRI] = atoi(optarg);
			return 1;
		case 'b':
			uwsgi.buffer_size = atoi(optarg);
			return 1;
		case 'c':
			uwsgi.shared->options[UWSGI_OPTION_CGI_MODE] = 1;
			return 1;
		case 'a':
			uwsgi.abstract_socket = 1;
			return 1;
		case 'C':
			uwsgi.chmod_socket = 1;
			if (optarg) {
				if (strlen(optarg) != 3) {
					uwsgi_log("invalid chmod value: %s\n", optarg);
					exit(1);
				}
				for (i = 0; i < 3; i++) {
					if (optarg[i] < '0' || optarg[i] > '7') {
						uwsgi_log("invalid chmod value: %s\n", optarg);
						exit(1);
					}
				}

				uwsgi.chmod_socket_value = (uwsgi.chmod_socket_value << 3) + (optarg[0] - '0');
				uwsgi.chmod_socket_value = (uwsgi.chmod_socket_value << 3) + (optarg[1] - '0');
				uwsgi.chmod_socket_value = (uwsgi.chmod_socket_value << 3) + (optarg[2] - '0');
			}
			return 1;
		case 'M':
			uwsgi.master_process = 1;
			return 1;
		case 'R':
			uwsgi.shared->options[UWSGI_OPTION_MAX_REQUESTS] = atoi(optarg);
			return 1;
		case 'z':
			if (atoi(optarg) > 0) {
				uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT] = atoi(optarg);
			}
			return 1;
		case 'T':
			uwsgi.has_threads = 1;
			uwsgi.shared->options[UWSGI_OPTION_THREADS] = 1;
			return 1;
		case 'i':
			uwsgi.single_interpreter = 1;
			return 1;
		case 'h':
			fprintf(stdout, "Usage: %s [options...]\n\
\t-s|--socket <name>\t\tpath (or name) of UNIX/TCP socket to bind to\n\
\t-l|--listen <num>\t\tset socket listen queue to <n> (default 64, maximum is system dependent)\n\
\t-z|--socket-timeout <sec>\tset socket timeout to <sec> seconds (default 4 seconds)\n\
\t-b|--buffer-size <n>\t\tset buffer size to <n> bytes\n\
\t-L|--disable-logging\t\tdisable request logging (only errors or server messages will be logged)\n\
\t-x|--xmlconfig <path>\t\tpath of xml config file\n\
\t-w|--module <module>\t\tname of python config module\n\
\t-t|--harakiri <sec>\t\tset harakiri timeout to <sec> seconds\n\
\t-p|--processes <n>\t\tspawn <n> uwsgi worker processes\n\
\t-O|--optimize <n>\t\tset python optimization level to <n>\n\
\t-v|--max-vars <n>\t\tset maximum number of vars/headers to <n>\n\
\t-A|--sharedarea <n>\t\tcreate a shared memory area of <n> pages\n\
\t-c|--cgi-mode\t\t\tset cgi mode\n\
\t-C|--chmod-socket[=NNN]\t\tchmod socket to 666 or NNN\n\
\t-m|--memory-report\t\tenable memory usage report\n\
\t-i|--single-interpreter\t\tsingle interpreter mode\n\
\t-a|--abstract-socket\t\tset socket in the abstract namespace (Linux only)\n\
\t-T|--enable-threads\t\tenable threads support\n\
\t-M|--master\t\t\tenable master process manager\n\
\t-H|--home <path>\t\tset python home/virtualenv\n\
\t-h|--help\t\t\tthis help\n\
\t-r|--reaper\t\t\tprocess reaper (call waitpid(-1,...) after each request)\n\
\t-R|--max-requests\t\tmaximum number of requests for each worker\n\
\t-j|--test\t\t\ttest if uWSGI can import a module\n\
\t-Q|--spooler <dir>\t\trun the spooler on directory <dir>\n\
\t--callable <callable>\t\tset the callable (default 'application')\n\
\t--pidfile <file>\t\twrite the masterpid to <file>\n\
\t--chroot <dir>\t\t\tchroot to directory <dir> (only root)\n\
\t--gid <id/groupname>\t\tsetgid to <id/groupname> (only root)\n\
\t--uid <id/username>\t\tsetuid to <id/username> (only root)\n\
\t--chdir <dir>\t\t\tchdir to <dir> before app loading\n\
\t--chdir2 <dir>\t\t\tchdir to <dir> after module loading\n\
\t--no-server\t\t\tinitialize the uWSGI server then exit. Useful for testing and using uwsgi embedded module\n\
\t--no-defer-accept\t\tdisable the no-standard way to defer the accept() call (TCP_DEFER_ACCEPT, SO_ACCEPTFILTER...)\n\
\t--paste <config:/egg:>\t\tload applications using paste.deploy.loadapp()\n\
\t--check-interval <sec>\t\tset the check interval (in seconds) of the master process\n\
\t--pythonpath <dir>\t\tadd <dir> to PYTHONPATH\n\
\t--python-path <dir>\t\tadd <dir> to PYTHONPATH\n\
\t--pp <dir>\t\t\tadd <dir> to PYTHONPATH\n\
\t--pyargv <args>\t\t\tassign args to python sys.argv\n\
\t--limit-as <MB>\t\t\tlimit the address space of processes to MB megabytes\n\
\t--limit-post <bytes>\t\tlimit HTTP content_length size to <bytes>\n\
\t--post-buffering <bytes>\tbuffer HTTP POST request higher than <bytes> to disk\n\
\t--post-buffering-bufsize <b>\tset the buffer size to <b> bytes for post-buffering\n\
\t--prio <N>\t\t\tset process priority/nice to N\n\
\t--no-orphans\t\t\tautomatically kill workers on master's dead\n\
\t--udp <ip:port>\t\t\tbind master process to udp socket on ip:port\n\
\t--multicast <group>\t\tset multicast group\n\
\t--snmp\t\t\t\tenable SNMP support in the UDP server\n\
\t--snmp-community <value>\tset SNMP community code to <value>\n\
\t--erlang <name@ip>\t\tenable the Erlang server with node name <node@ip>\n\
\t--erlang-cookie <cookie>\tset the erlang cookie to <cookie>\n\
\t--nagios\t\t\tdo a nagios check\n\
\t--binary-path <bin-path>\tset the path for the next reload of uWSGI (needed for chroot environments)\n\
\t--proxy <socket>\t\trun the uwsgi proxy on socket <socket>\n\
\t--proxy-node <socket>\t\tadd the node <socket> to the proxy\n\
\t--proxy-max-connections <n>\tset the max number of concurrent connections mnaged by the proxy\n\
\t--wsgi-file <file>\t\tload the <file> wsgi file\n\
\t--file <file>\t\t\tuse python file instead of python module for configuration\n\
\t--eval <code>\t\t\tevaluate code for app configuration\n\
\t--async <n>\t\t\tenable async mode with n core\n\
\t--logto <logfile|addr>\t\tlog to file/udp\n\
\t--logdate\t\t\tadd timestamp to loglines\n\
\t--log-zero\t\t\tlog requests with 0 response size\n\
\t--log-slow <t>\t\t\tlog requests slower than <t> milliseconds\n\
\t--log-4xx\t\t\tlog requests with status code 4xx\n\
\t--log-5xx\t\t\tlog requests with status code 5xx\n\
\t--log-big <n>\t\t\tlog requests bigger than <n> bytes\n\
\t--log-sendfile\t\t\tlog sendfile() requests\n\
\t--ignore-script-name\t\tdisable uWSGI management of SCRIPT_NAME\n\
\t--no-default-app\t\tdo not fallback unknown SCRIPT_NAME requests\n\
\t--ini <inifile>\t\t\tpath of ini config file\n\
\t--ini-paste <inifile>\t\tpath of ini config file that contains paste configuration\n\
\t--ldap <url>\t\t\turl of LDAP uWSGIConfig resource\n\
\t--ldap-schema\t\t\tdump uWSGIConfig LDAP schema\n\
\t--ldap-schema-ldif\t\tdump uWSGIConfig LDAP schema in LDIF format\n\
\t--grunt\t\t\t\tenable grunt workers\n\
\t--ugreen\t\t\tenable uGreen support\n\
\t--ugreen-stacksize <n>\t\tset uGreen stacksize to <n>\n\
\t--stackless\t\t\tenable usage of tasklet (only on Stackless Python)\n\
\t--no-site\t\t\tdo not import site.py on startup\n\
\t--vhost\t\t\t\tenable virtual hosting\n\
\t--mount MOUNTPOINT=app\t\tadda new app under MOUNTPOINT\n\
\t--routing\t\t\tenable uWSGI advanced routing\n\
\t--http <addr>\t\t\tstart embedded HTTP server on <addr>\n\
\t--http-only\t\t\tstart only the embedded HTTP server\n\
\t--http-var KEY[=VALUE]\t\tadd var KEY to uwsgi requests made by the embedded HTTP server\n\
\t--catch-exceptions\t\tprint exceptions in the browser\n\
\t--mode\t\t\t\tset configuration mode\n\
\t--env KEY=VALUE\t\t\tset environment variable\n\
\t--vacuum\t\t\tclear the environment on exit (remove UNIX sockets and pidfiles)\n\
\t--ping <addr>\t\t\tping a uWSGI server (returns 1 on failure 0 on success)\n\
\t--ping-timeout <n>\t\tset ping timeout to <n>\n\
\t--cgroup <group>\t\trun the server in <group> cgroup (Linux only)\n\
\t--cgroup-opt KEY=VAL\t\tset cgroup option (Linux only)\n\
\t--version\t\t\tprint server version\n\
\t-d|--daemonize <logfile|addr>\tdaemonize and log into <logfile> or udp <addr>\n", uwsgi.binary_path);
			return 0;
		}

		return 0;
	}

	void manage_opt(int i, char *optarg) {

		int j;

		if (manage_base_opt(i, optarg)) {
			add_exported_option( i, optarg );
			return;
		}

		for (j = 0; j < 0xFF; j++) {
			if (uwsgi.p[j]->manage_opt) {
				if (uwsgi.p[j]->manage_opt(i, optarg)) {
					add_exported_option( i, optarg );
					return;
				}
			}
		}

		for (j = 0; j < uwsgi.gp_cnt; j++) {
			if (uwsgi.gp[j]->manage_opt) {
				if (uwsgi.gp[j]->manage_opt(i, optarg)) {
					add_exported_option( i, optarg );
					return;
				}
			}
		}

		//never here
		exit(1);

	}

	void uwsgi_cluster_add_node(char *nodename, int workers) {

		int i;
		struct uwsgi_cluster_node *ucn;
		char *tcp_port;

		if (strlen(nodename) > 100) {
			uwsgi_log("invalid cluster node name %s\n", nodename);
			return;
		}
		 tcp_port = strchr(nodename, ':');
		if (tcp_port == NULL) {
			fprintf(stdout, "invalid cluster node name %s\n", nodename);
			return;
		}
		for (i = 0; i < MAX_CLUSTER_NODES; i++) {
			ucn = &uwsgi.shared->nodes[i];

			if (ucn->name[0] == 0) {
				memcpy(ucn->name, nodename, strlen(nodename) + 1);
				ucn->workers = workers;
				ucn->ucn_addr.sin_family = AF_INET;
				ucn->ucn_addr.sin_port = htons(atoi(tcp_port + 1));
				tcp_port[0] = 0;
				if (nodename[0] == 0) {
					ucn->ucn_addr.sin_addr.s_addr = INADDR_ANY;
				} else {
					uwsgi_log("%s\n", nodename);
					ucn->ucn_addr.sin_addr.s_addr = inet_addr(nodename);
				}

				ucn->last_seen = time(NULL);

				return;
			}
		}

		uwsgi_log("unable to add node %s\n", nodename);
	}


void build_options() {
		int i;
		struct option *lopt,
		*aopt;
		int opt_count = count_options(long_base_options);
		int short_opt_size = strlen(base_short_options);
		char *so_ptr;

		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->short_options) {
				short_opt_size += strlen(uwsgi.p[i]->short_options);
			}
		}

		for(i=0;i<uwsgi.gp_cnt;i++) {
			if (uwsgi.gp[i]->short_options) {
				short_opt_size += strlen(uwsgi.gp[i]->short_options);
			}
		}

		if (short_options) {
			free(short_options);
		}
		short_options = malloc(short_opt_size + 1);
		if (!short_options) {
			uwsgi_error("malloc()");
			exit(1);
		}
		memcpy(short_options, base_short_options, strlen(base_short_options));
		so_ptr = short_options + strlen(base_short_options);

		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->short_options) {
				memcpy(so_ptr, uwsgi.p[i]->short_options, strlen(uwsgi.p[i]->short_options));
				so_ptr += strlen(uwsgi.p[i]->short_options);
			}
		}

		for (i = 0; i < uwsgi.gp_cnt; i++) {
			if (uwsgi.gp[i]->short_options) {
				memcpy(so_ptr, uwsgi.gp[i]->short_options, strlen(uwsgi.gp[i]->short_options));
				so_ptr += strlen(uwsgi.gp[i]->short_options);
			}
		}

		*so_ptr = 0;

		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->options) {
				opt_count += count_options(uwsgi.p[i]->options);
			}
		}

		for (i = 0; i < uwsgi.gp_cnt; i++) {
			if (uwsgi.gp[i]->options) {
				opt_count += count_options(uwsgi.gp[i]->options);
			}
		}

		if (uwsgi.long_options) {
			free(uwsgi.long_options);
		}
		uwsgi.long_options = malloc(sizeof(struct option) * (opt_count + 1));
		if (!uwsgi.long_options) {
			uwsgi_error("malloc()");
			exit(1);
		}
		opt_count = 0;
		lopt = long_base_options;
		while ((aopt = lopt)) {
			if (!aopt->name)
				break;
			uwsgi.long_options[opt_count].name = aopt->name;
			uwsgi.long_options[opt_count].has_arg = aopt->has_arg;
			uwsgi.long_options[opt_count].flag = aopt->flag;
			uwsgi.long_options[opt_count].val = aopt->val;
			opt_count++;
			lopt++;
		}

		for (i = 0; i < 0xFF; i++) {
			lopt = uwsgi.p[i]->options;
			if (!lopt)
				continue;

			while ((aopt = lopt)) {
				if (!aopt->name)
					break;
				uwsgi.long_options[opt_count].name = aopt->name;
				uwsgi.long_options[opt_count].has_arg = aopt->has_arg;
				uwsgi.long_options[opt_count].flag = aopt->flag;
				uwsgi.long_options[opt_count].val = aopt->val;
				opt_count++;
				lopt++;
			}

		}

		for (i = 0; i < uwsgi.gp_cnt; i++) {
			lopt = uwsgi.gp[i]->options;
			if (!lopt)
				continue;

			while ((aopt = lopt)) {
				if (!aopt->name)
					break;
				uwsgi.long_options[opt_count].name = aopt->name;
				uwsgi.long_options[opt_count].has_arg = aopt->has_arg;
				uwsgi.long_options[opt_count].flag = aopt->flag;
				uwsgi.long_options[opt_count].val = aopt->val;
				opt_count++;
				lopt++;
			}

		}

		uwsgi.long_options[opt_count].name = 0;
		uwsgi.long_options[opt_count].has_arg = 0;
		uwsgi.long_options[opt_count].flag = 0;
		uwsgi.long_options[opt_count].val = 0;
}
