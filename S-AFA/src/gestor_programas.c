#include "gestor_programas.h"

static void gestor_procesar_comando(char*);

void gestor_consola_iniciar(){
	char * linea;
	printf("Bienvenido! Ingrese \"ayuda\" para ver una lista con todos los comandos disponibles \n");
	while(1) {
		linea = readline("S-AFA> ");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "salir", 5)) {
			free(linea);
			break;
		}
		gestor_procesar_comando(linea);
		free(linea);
	}
	exit(EXIT_SUCCESS);
}

static void gestor_procesar_comando(char* linea){
	void split_liberar(char** split){
		unsigned int i = 0;
		for(;split[i] != NULL;i++){
			free(split[i]);
		}
		free(split);
	}
	int split_cant_elem(char**split){
		int i = 0;
		for(;split[i] != NULL; i++);
		return i;
	}

	char** argv = string_split(linea, " ");
	int argc = split_cant_elem(argv);

	/* Comando ayuda */
	if(!strcmp(linea, "ayuda")){
		printf("\nejecutar [ruta]:\tEjecuta el escriptorio ubicado en la ruta\n");
		printf("status:\t\t\tMuestra el estado de cada cola\n");
		printf("status [pcb_id]:\tMuestra todos los datos del DTB especificado\n");
		printf("finalizar [pcb_id]:\tFinaliza la ejecucion de un GDT especificado\n");
		printf("metricas:\t\tMuestra distintas metricas del sistema\n");
		printf("metricas [pcb_id]:\tMuestra distintas metricas del sistema y del DTB especificado\n");
		printf("salir:\t\t\tCierra la consola\n\n");
		split_liberar(argv);
	}
	/* Comando ejecutar [ruta]*/
	else if(argc == 2 && !strcmp(argv[0],"ejecutar")){
		planificador_crear_dtb_y_encolar(argv[1]);
		split_liberar(argv);
	}
	/* Comando status*/
	else if(argc == 1 && !strcmp(argv[0], "status")){
		printf("CANT_PROCESOS: %d, MULTIPROGRAMACION: %d", cant_procesos, config_get_int_value(config, "MULTIPROGRAMACION"));
		printf("\nNEW:%d READY:%d BLOCK:%d EXEC: %d EXIT:%d \n",
				cola_new->elements_count,
				cola_ready->elements_count,
				cola_block->elements_count,
				cola_exec->elements_count,
				cola_exit->elements_count);
		int i;
		printf("\n-----------------Cola NEW-----------------:\n");
		for(i = 0; i < cola_new->elements_count; i++){
			dtb_mostrar(list_get(cola_new, i), "NEW");
		}
		printf("\n-----------------Cola READY-----------------:\n");
		for(i = 0; i < cola_ready->elements_count; i++){
			dtb_mostrar(list_get(cola_ready, i), "READY");
		}
		printf("\n-----------------Cola BLOCK-----------------:\n");
		for(i = 0; i < cola_block->elements_count; i++){
			dtb_mostrar(list_get(cola_block, i), "BLOCK");
		}
		printf("\n-----------------Cola EXEC-----------------:\n");
		for(i = 0; i < cola_exec->elements_count; i++){
			dtb_mostrar(list_get(cola_exec, i), "EXEC");
		}
		printf("\n-----------------Cola EXIT-----------------:\n");
		for(i = 0; i < cola_exit->elements_count; i++){
			dtb_mostrar(list_get(cola_exit, i), "EXIT");
		}
		split_liberar(argv);
	}
	/* Comando status [pcb_id] */
	else if(argc == 2 && !strcmp(argv[0], "status")){
		char* estado_actual;
		t_dtb* dtb = planificador_encontrar_dtb( (unsigned) atoi(argv[1]) , &estado_actual);
		if(dtb != NULL)
			dtb_mostrar(dtb, estado_actual);
		else
			printf("No se encontro el proceso con ID = %s\n\n", argv[1]);
		dtb_destroy(dtb);
		free(estado_actual);
		split_liberar(argv);
	}
	/* Comando finalizar [pcb_id] */
	else if(argc == 2 && !strcmp(argv[0], "finalizar")){
		if(!strcmp(argv[1], "0"))
			printf("No se puede finalizar el proceso dummy\n\n");
		else if(atoi(argv[1]) == 0)
			printf("Se debe ingresar el numero de ID\n\n");
		else if(planificador_finalizar_dtb((unsigned) atoi(argv[1])))
			printf("El proceso con ID = %s ha sido finalizado satisfactoriamente\n\n", argv[1]);
		else
			printf("No se encontro el proceso con ID = %s\n\n", argv[1]);
		split_liberar(argv);
	}
	/* Comando metricas */
	else if(argc == 1 && !strcmp(argv[0], "metricas")){

		split_liberar(argv);
	}
	/* Comando metricas [pcb_id] */
	else if(argc == 2 && !strcmp(argv[0], "metricas")){

		split_liberar(argv);
	}
	/* Error al ingresar comando */
	else{
		printf("No se pudo reconocer el comando\n\n");
		split_liberar(argv);
	}
}


