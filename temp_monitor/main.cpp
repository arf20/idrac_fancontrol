#include <iostream>
#include <cstring>

void die(const std::string& msg) {
	std::cout << msg << std::endl;
	exit(1);
}

extern "C" {
	#include <OpenIPMI/ipmiif.h>
	#include <OpenIPMI/ipmi_smi.h>
	#include <OpenIPMI/ipmi_err.h>
	#include <OpenIPMI/ipmi_auth.h>
	#include <OpenIPMI/ipmi_lan.h>
	#include <OpenIPMI/ipmi_posix.h>
	#include <OpenIPMI/ipmi_fru.h>
	#include <OpenIPMI/internal/ipmi_sensor.h>
}

#include <iostream>

static os_handler_t *os_hnd;

// On new (change) entity, search all sensors of it
static void entity_change_handler(enum ipmi_update_e op, ipmi_domain_t *domain, ipmi_entity_t *entity, void *cb_data) {
	int rv;
	
	int id = ipmi_entity_get_entity_id(entity);
	int instance = ipmi_entity_get_entity_instance(entity);
	
	if (op == IPMI_ADDED) {
		std::cout << "Entity added: " << id << "." << instance << " " << ipmi_entity_get_entity_id_string(entity) << std::endl;
	}
}

// On connection done, search all entities in the system
void setup_done_handler(ipmi_domain_t *domain, int err, unsigned int conn_num, unsigned int port_num, int still_connected, void *user_data) {
	std::cout << "Setup done" << std::endl;
	
	int rv;
	
	if (rv = ipmi_domain_add_entity_update_handler(domain, entity_change_handler, domain))
		die(std::string("Error in ipmi_domain_add_entity_update_handler: ") + strerror(rv));
}

int main(int argc, char **argv) {
	// Allocate OS handler
	os_hnd = ipmi_posix_setup_os_handler();
	if (!os_hnd) die("Cound not allocate OS handler.");
	
	// Initialize OpenIPMI two times to detect bugs
	int rv;
	if (rv = ipmi_init(os_hnd))
		die(std::string("Error in OpenIPMI initialization: ") + strerror(rv));
		
	ipmi_shutdown();
	
	if (rv = ipmi_init(os_hnd))
		die(std::string("Error in OpenIPMI initialization (2): ") + strerror(rv));
		
	// Parse command arguments (connection stuff)
	int curr_arg = 1;
	ipmi_args_t *args;
	if (rv = ipmi_parse_args2(&curr_arg, argc, argv, &args))
		die(std::string("Error parsing command argument ") + std::to_string(curr_arg) + strerror(rv));
		
	// Setup arguments connection
	ipmi_con_t *con;
	if (rv = ipmi_args_setup_con(args, os_hnd, NULL, &con))
		die(std::string("Error in ipmi_args_setup_con: ") + strerror(rv));
		
	// Open domain
	std::cout << "Opening domain..." << std::endl;
	if (rv = ipmi_open_domain("", &con, 1, setup_done_handler, NULL, NULL, NULL, NULL, 0, NULL))
		die(std::string("Error in ipmi_open_domain: ") + strerror(rv));
		
	// Main loop of events
	while (true)
		os_hnd->perform_one_op(os_hnd, NULL);
		
	// Free
	os_hnd->free_os_handler(os_hnd);
}