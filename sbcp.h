//The header files to define the message info for SBCP protocol


#define SBCP_JOIN 2
#define SBCP_FWD  3
#define SBCP_SEND 4

#define ATTR_REASON 		1
#define ATTR_USERNAME 		2
#define ATTR_CLIENTCOUNT 	3
#define ATTR_MESSAGE  		4


//the SBCP attribute
struct attr_sbcp{
    uint16_t type;
    uint16_t length;
    char* payload;
};

//The SBCP message
struct msg_sbcp{
//    uint16_t vrsn_type; //high 9 bits is protocol version and low 7 bits is the type
	uint8_t version;
	uint8_t type;
    uint16_t length;
    struct attr_sbcp* payload;
};

//pack the version and type into the header of SBCP message
/*
void pack_version_type(struct msg_sbcp *msg, uint16_t version, uint16_t type){
	msg->vrsn_type = version <<
}
*/

