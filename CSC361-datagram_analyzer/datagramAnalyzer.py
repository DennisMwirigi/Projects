import sys
import struct
from statistics import mean, stdev
from basic_structures import *

def output(src_node, dst_node, intermed_dst_addrs, protocol_values, rtts, frag_packets):
    print("The IP address of the source node:", src_node)
    print("The IP address of ultimate destination node:", dst_node)
    
    print("The IP addresses of the intermediate destination nodes:")
    for i in range(len(intermed_dst_addrs)):
        print("\tRouter " + str(i+1) + ": " + str(intermed_dst_addrs[i][0]))
    
    print("\nThe values in the protocol field of IP headers:")
    for protocol in protocol_values:
        if protocol == 1:
            print("\t" + str(protocol) + ": ICMP")
        elif protocol == 17:
            print("\t" + str(protocol) + ": UDP")
    
    for key, value in frag_packets.items():
        if value["num_fragments"] != 0:
            print("\nThe number of fragments created from the original datagram with id " + str(key) + " is: " + str(value["num_fragments"]))
            print("The offset of the last fragment is: " + str(value["last_offset"]))
    
    print()
    for dest_addr in intermed_dst_addrs:
        for key, val in rtts.items():
            if dest_addr[0] == key:
                print("The avg RTT between " + src_node + " and " + str(key)
                        + " is: " + "{:.6f}".format(val["mean_rtt"] *1000) + " ms, the s.d. is: " + "{:.6f}".format(val["std_dev"] *1000) + " ms")
    
    for key, val in rtts.items():
        if key == dst_node:
            print("The avg RTT between " + src_node + " and " + str(key)
                    + " is: " + "{:.6f}".format(val["mean_rtt"] *1000) + " ms, the s.d. is: " + "{:.6f}".format(val["std_dev"] *1000) + " ms")

# return second element of a tuple
def takeSecond(elem):
    return elem[1]

def compute_and_output_data(icmp_packets, udp_packets, protocol_values, icmp_type8_list):
    protocol_values = sorted(protocol_values)
    intermed_dst_addrs = []
    rtts = {}
    frag_pckts = {}
    source_node = ''
    dest_node = ''

    for icmp_pckt in icmp_packets:
            rtts[icmp_pckt.IP_header.src_ip] = {"rtt_list" : [],
                                                "mean_rtt" : 0.0,
                                                "std_dev" : 0.0}

    if len(icmp_type8_list) > 0:
        # windows
        for icmp_pckt in icmp_packets:
            for type8_pckt in  icmp_type8_list:
                if icmp_pckt.ICMP_header.seq_num == type8_pckt.ICMP_header.seq_num and icmp_pckt.ICMP_header.icmp_type == 11:
                    # store ip of intermediate addresses
                    if (icmp_pckt.IP_header.src_ip, 0) not in intermed_dst_addrs:
                        intermed_dst_addrs.append((icmp_pckt.IP_header.src_ip, 0))
                    # store rtt values of intermediate addresses
                    rtt = icmp_pckt.timestamp - type8_pckt.timestamp
                    rtts[icmp_pckt.IP_header.src_ip]["rtt_list"].append(rtt)
                elif icmp_pckt.ICMP_header.seq_num == type8_pckt.ICMP_header.seq_num and icmp_pckt.ICMP_header.icmp_type == 0 and icmp_pckt.ICMP_header.code == 0:
                    # store rtt values of final destination address
                    rtt = icmp_pckt.timestamp - type8_pckt.timestamp
                    rtts[icmp_pckt.IP_header.src_ip]["rtt_list"].append(rtt)

                    # define original src and final dst node for output
                    source_node = icmp_pckt.IP_header.dst_ip
                    dest_node = icmp_pckt.IP_header.src_ip
    else:
        # linux
        for pckt in udp_packets:
            if pckt.IP_header.flags == 1:
                frag_pckts[pckt.IP_header.identification] = {"num_fragments" : 0,
                                                            "last_offset" : 0}
        
        # get fragment data
        for key, value in frag_pckts.items():
            for pckt in udp_packets:
                if key == pckt.IP_header.identification:
                    value["num_fragments"] += 1
                    if pckt.IP_header.flags == 0:
                        value["last_offset"] = pckt.IP_header.frag_offset
        
        for icmp_pckt in icmp_packets:
            for udp_pckt in  udp_packets:
                if icmp_pckt.ICMP_header.src_port == udp_pckt.UDP_header.src_port and icmp_pckt.ICMP_header.icmp_type == 11:
                    # store ip of intermediate addresses
                    if (icmp_pckt.IP_header.src_ip, udp_pckt.IP_header.ttl) not in intermed_dst_addrs:
                        intermed_dst_addrs.append((icmp_pckt.IP_header.src_ip, udp_pckt.IP_header.ttl))
                    # store rtt values of intermediate addresses
                    rtt = icmp_pckt.timestamp - udp_pckt.timestamp
                    rtts[icmp_pckt.IP_header.src_ip]["rtt_list"].append(rtt)
                elif icmp_pckt.ICMP_header.src_port == udp_pckt.UDP_header.src_port and icmp_pckt.ICMP_header.icmp_type == 3 and icmp_pckt.ICMP_header.code == 3:
                    # store rtt values of final destination address
                    rtt = icmp_pckt.timestamp - udp_pckt.timestamp
                    rtts[icmp_pckt.IP_header.src_ip]["rtt_list"].append(rtt)

                    # define original src and final dst node for output
                    source_node = icmp_pckt.IP_header.dst_ip
                    dest_node = icmp_pckt.IP_header.src_ip
    
    # order by ttl
    intermed_dst_addrs.sort(key=takeSecond)    

    # get the mean and s.d using rtts collected
    for key,value in rtts.items():
        if len(value["rtt_list"]) == 0:
            value["mean_rtt"] = 0
        else:
            value["mean_rtt"] = mean(rtts[key]["rtt_list"])
        if len(rtts[key]["rtt_list"]) >= 2:
            value["std_dev"] = stdev(rtts[key]["rtt_list"])

    output(source_node, dest_node, intermed_dst_addrs, protocol_values, rtts, frag_pckts)

def get_packet_data(filename, protocol_set, udp_list, icmp_list, icmp_type8_list):
    f = open(filename, 'rb')

    global_header = f.read(24)

    magic_num = struct.unpack('I', global_header[:4])
    version_major = struct.unpack('H', global_header[4:6])
    version_minor = struct.unpack('H', global_header[6:8])
    thiszone = struct.unpack('I', global_header[8:12])
    sigfigs = struct.unpack('I', global_header[12:16])
    snaplen = struct.unpack('I', global_header[16:20])
    network = struct.unpack('I', global_header[20:24])

    num_packets = 0
    windows = 0

    packet_header = f.read(16)
    while(packet_header != b''):
        num_packets += 1

        pckt = packet()
        pckt.packet_No = num_packets

        ts_sec = packet_header[0:4]
        ts_usec = packet_header[4:8]
        incl_len = struct.unpack('I', packet_header[8:12])
        orig_len = struct.unpack('I', packet_header[12:16])

        packet_data = f.read(incl_len[0])

        # IP data
        pckt.IP_header.get_header_len(packet_data[14:15])
        pckt.IP_header.get_total_len(packet_data[16:18])
        pckt.IP_header.get_identification(packet_data[18:20])
        pckt.IP_header.get_flags(packet_data[20:21])
        pckt.IP_header.get_frag_offset(packet_data[20:22])
        pckt.IP_header.get_ttl(packet_data[22:23])
        pckt.IP_header.get_protocol(packet_data[23:24])
        pckt.IP_header.get_IP(packet_data[26:30], packet_data[30:34]) # ip src and dst addresses

        # UDP data
        if pckt.IP_header.protocol == 17:
            pckt.UDP_header.get_src_port(packet_data[34:36])
            pckt.UDP_header.get_dst_port(packet_data[36:38])
        # ICMP data
        elif pckt.IP_header.protocol == 1:
            pckt.ICMP_header.get_icmp_type(packet_data[34:35])
            pckt.ICMP_header.get_code(packet_data[35:36])

            # flag to know whether a windows file is being parsed
            if pckt.ICMP_header.icmp_type == 8 and len(icmp_list) == 0:
                windows = 1
            
            if windows:
                if pckt.ICMP_header.icmp_type == 8 or pckt.ICMP_header.icmp_type == 0:
                    pckt.ICMP_header.get_seq_num(packet_data[40:42])
                else:
                    pckt.ICMP_header.get_identification(packet_data[46:48])
                    pckt.ICMP_header.get_ttl(packet_data[50:51])
                    pckt.ICMP_header.get_seq_num(packet_data[68:70])
            # linux
            elif pckt.ICMP_header.icmp_type == 0 or pckt.ICMP_header.icmp_type == 3 or pckt.ICMP_header.icmp_type == 11:
                pckt.ICMP_header.get_identification(packet_data[46:48])
                pckt.ICMP_header.get_ttl(packet_data[50:51])
                pckt.ICMP_header.get_src_port(packet_data[62:64])
                pckt.ICMP_header.get_dst_port(packet_data[64:66])

        # some packet data
        pckt.timestamp_set(ts_sec, ts_usec, 0)

        # adding values to corresponding collection store
        if pckt.IP_header.protocol == 17:
            udp_list.append(pckt)
            protocol_set.add(pckt.IP_header.protocol)
        
        if pckt.IP_header.protocol == 1:
            if pckt.ICMP_header.icmp_type == 8:
                icmp_type8_list.append(pckt)
            elif pckt.ICMP_header.icmp_type == 0 or pckt.ICMP_header.icmp_type == 3 or pckt.ICMP_header.icmp_type == 11:
                icmp_list.append(pckt)
            protocol_set.add(pckt.IP_header.protocol)
        

        packet_header = f.read(16)
    # end of while loop

    f.close()

def main():
    filename = sys.argv[1]

    protocol_values = set()
    udp_packets = []
    icmp_packets = []
    icmp_type8_packets = []

    get_packet_data(filename, protocol_values, udp_packets, icmp_packets, icmp_type8_packets)

    compute_and_output_data(icmp_packets, udp_packets, protocol_values, icmp_type8_packets)

if __name__ == "__main__": main()