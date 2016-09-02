//The header files to define the message info for SBCP protocol

//the SBCP attribute
struct attr_sbcp{
    uint16_t type;
    uint16_t length;
    char* payload;
};

//The SBCP message
struct msg_sbcp{
    uint16_t vrsn_type; //high 9 bits is protocol version and low 7 bits is the type
    uint16_t len;
    struct attr_sbcp* payload;
};


