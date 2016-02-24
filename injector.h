
int inject_frame( Frame * f, Port * p){


    memcpy(EthII->src_addr , p->mac, 6 );
    if (p && f) {
        int sent_bytes = pcap_inject(p->handle, f->eth_header, f->length);
        if (sent_bytes == f->length) {

        	sprintf(log_b, "\t< -- Port %i (%s) sending %i bytes", p->id, p->name, sent_bytes);
        	my_log(log_b);
            return 1;
        } else {
        	my_log("ERROR !!! Failed to send frame with specific size :(");
            return 0;
        }
    } else {
        my_log("port or frame null");
        return 0;
    }
}
