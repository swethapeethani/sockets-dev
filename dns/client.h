


struct dnsheader{
    unsigned short id;
    
  unsigned char rd :1;
    unsigned char tc:1;
    unsigned char aa:1;
    unsigned char opcode:4;
    unsigned char qr:1;

    unsigned char rcode:4;
    unsigned char cd  :1;
    unsigned char ad:1;
    unsigned char z:1;
    unsigned char ra:1;

    //unsigned short flags;

    unsigned short qdcount; //entries in question section
    unsigned short ancount; //No: of Resource records in answer section
    unsigned short nscount; //No: of name server records in Authority records
    unsigned short arcount; //No: of RRecords in additional records
};

struct query{
    char *qname;
    unsigned short qtype;
    unsigned short qclass;
};

typedef struct{
    unsigned short name_offset;
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rlength;
    struct in_addr addr; //unsigned int 32 bit
} __attribute__((packed)) dns_record_a;

typedef struct{
    unsigned short name_offset;
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rlength;
    struct in6_addr addr;
} __attribute__((packed)) dns_record_aaaa;

typedef struct{
 unsigned short name_offset;
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rlength;
    unsigned char *name;
}__attribute__((packed)) dns_record_mx;

typedef struct{
 unsigned short name_offset;
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rlength;
    unsigned char *name;
}__attribute__((packed)) dns_record_cname;