#ifndef FM9_CONSOLA_H_
#define FM9_CONSOLA_H_
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <commons/log.h>
	#include <commons/string.h>
	#include <readline/readline.h>
	#include <readline/history.h>
	#include <shared/util.h>
	#include "FM9.h"

	void fm9_consola_init();

	void fm9_procesar_comando(char*);

	void (*fm9_dump_pid)(unsigned int);
	void _fm9_dump_pid_seg_pura(unsigned int id);

	char** fm9_consola_autocompletar();
	char* fm9_consola_autocompletar_command_generator();
	extern char** completion_matches(char *, rl_compentry_func_t *);

#endif /* FM9_CONSOLA_H_ */
