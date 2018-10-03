#ifndef PCP_H_
#define PCP_H_

	#include <shared/DTB.h>
	#include <shared/protocol.h>

	#include "S-AFA.h"

	t_list* lista_procesos_a_finalizar_en_exec;

	void pcp_iniciar();
	void pcp_gestionar_msg(t_safa_msg* msg);
	void pcp_intentar_ejecutar_dtb();
	void pcp_actualizar_dtb(t_dtb* dtb);
	void pcp_cargar_archivo(t_dtb* dtb_a_actualizar, char* path, int base);
	t_dtb* pcp_encontrar_dtb(unsigned int id);
	t_status* pcp_empaquetar_status(t_status* status);
	void pcp_mover_dtb(unsigned int id, e_estado inicio, e_estado fin);


#endif /* PCP_H_ */