from scapy.all import *
import sys

dst_ip = sys.argv[1]

for port in range(1, 65535):
    ans, unans = sr(IP(dst=dst_ip) / TCP(sport=777, dport=port, flags="S"))
    for s, r in ans:
        try:
            if "SA" in str(r[TCP].flags):
                print("This port:" + str(port) + " - OPEN")
        except:
            # print("This port:" + str(port) + "- CLOSE")
            print("-")
