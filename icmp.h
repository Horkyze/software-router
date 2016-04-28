uint16_t icmp_checksum(uint16_t *buffer, int size)
{
    unsigned long cksum=0;
    while(size >1)
    {
        cksum+=*buffer++;
        size -=sizeof(u_short);
    }
    if(size )
    {
        cksum += *(u_char*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (uint16_t)(~cksum);
}

void icmp_reply(Frame * f){

    inject_frame(f, f->p);
}

void incoming_icmp(Frame * f){

}
