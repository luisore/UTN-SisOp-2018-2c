#include "pcp.h"

void pcp_iniciar(){
	/* Tengo que esperar que haya algo en la cola de ready, y esperar que haya CPU disponible */
	while(1){
		sem_wait(&sem_cont_cola_ready); // Espero DTBs en READY
		printf("-----------Hay DTB en READY----------\n"); // DEBUG: Sacar despues
		sem_wait(&sem_cont_cpu_conexiones); // Espero CPUs para poder mandarles el DTB
		printf("-----------Hay DTB en READY y CPU disponible ----------\n"); // DEBUG: Sacar despues
		usleep(retardo_planificacion);

		pthread_mutex_lock(&sem_mutex_cpu_conexiones);
		int cpu_socket = pcp_buscar_cpu_ociosa();
		pthread_mutex_unlock(&sem_mutex_cpu_conexiones);

		pthread_mutex_lock(&sem_mutex_cola_ready);
		t_dtb* dtb_elegido = pcp_aplicar_algoritmo();
		pthread_mutex_unlock(&sem_mutex_cola_ready);

		t_msg* dtb_empaquetado = empaquetar_dtb(dtb_elegido);

		dtb_empaquetado->header->emisor = SAFA;
		dtb_empaquetado->header->tipo_mensaje = EXEC;

		list_add(cola_exec, dtb_elegido);
		log_info(logger, "Muevo a EXEC el DTB con ID: %d", dtb_elegido->gdt_id);

		log_info(logger, "PCP le manda a ejecutar un DTB a CPU");
		msg_send(cpu_socket, *dtb_empaquetado);

		msg_free(&dtb_empaquetado);

	} // Fin while(1)
}

t_dtb* pcp_aplicar_algoritmo(){
	// char* algoritmo = config_get_string_value(config, "ALGORITMO");
	// TODO: Gestionar algoritmo

	// DUMMY tiene prioridad maxima
	bool _es_dummy(void* data){
		return  ((t_dtb*) data) -> gdt_id == 0;
	}
	t_dtb* dtb = list_remove_by_condition(cola_ready, _es_dummy);
	if(dtb != NULL) { // Si encontro el DUMMY
		dtb->ruta_escriptorio = list_remove(rutas_escriptorios_dtb_dummy, 0);
		return dtb;
	}

	// Aplico FIFO:
	return (t_dtb*) list_remove(cola_ready, 0);
}

int pcp_buscar_cpu_ociosa(){
	int i;
	for(i=0;i<cpu_conexiones->elements_count; i++){
		t_conexion_cpu* cpu = list_get(cpu_conexiones, i);
		if(cpu->en_uso == 0){
			cpu->en_uso = 1;
			return cpu->socket;
		}
	}
	return -1;
}

void pcp_mover_dtb(unsigned int id, char* cola_inicio, char* cola_destino){

	bool _tiene_mismo_id(void* data){
		return  ((t_dtb*) data) -> gdt_id == id;
	}

	t_dtb* dtb;

	if(!strcmp(cola_inicio, "READY"))
		dtb = list_remove_by_condition(cola_ready, _tiene_mismo_id);
	else if(!strcmp(cola_inicio, "BLOCK"))
		dtb = list_remove_by_condition(cola_block, _tiene_mismo_id);
	else if(!strcmp(cola_inicio, "EXEC")){
		/* TODO: Si cola_destino == "EXEC", esperar a que termine la instruccion */
		dtb = list_remove_by_condition(cola_exec, _tiene_mismo_id);
	}
	else return;

	if(!strcmp(cola_destino, "EXIT")){
		log_info(logger, "Finalizo el DTB con ID: %d", id);
		cant_procesos--;
		list_add(cola_exit, dtb);
	}
	else if((!strcmp(cola_inicio, "EXEC") || !strcmp(cola_inicio, "BLOCK")) && !strcmp(cola_destino, "READY")){ //EXEC->READY o BLOCK->READY
		log_info(logger, "Muevo a READY el DTB con ID: %d", id);
		pthread_mutex_lock(&sem_mutex_cola_ready);
		list_add(cola_ready, dtb);
		pthread_mutex_unlock(&sem_mutex_cola_ready);
		sem_post(&sem_cont_cola_ready);
	}
	else if(!strcmp(cola_inicio, "EXEC") && !strcmp(cola_destino, "BLOCK")){ // EXEC->BLOCK
		log_info(logger, "Bloqueo el DTB con ID: %d", id);
		list_add(cola_block, dtb);
	}
	else if(!strcmp(cola_inicio, "READY") && !strcmp(cola_destino, "EXEC")){ // READY-> EXEC
		log_info(logger, "Muevo a EXEC el DTB con ID: %d", id);
		list_add(cola_exec, dtb);
	}

}
