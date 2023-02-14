# TCP comparison

This is a comparison between two old TCP variants: TCP Tahoe and TCP Reno, using the ns-3 simulator.

## TCP Tahoe

TCP Tahoe is the first TCP variant to be implemented in ns-3. It is a simple TCP variant and uses a very simple mechanism to handle congestion.
It is a good starting point to understand how TCP works.

## TCP Reno

TCP Reno is the first TCP variant to be standardized. It is a more complex TCP variant than TCP Tahoe and uses a more complex mechanism to handle congestion.
It uses a fast retransmit mechanism to detect packet loss and a fast recovery mechanism to recover from packet loss and follows up with a fast recovery phase.

## Usage

The simulation is highly configurable. The following options are available:

```bash
./p2p-project
Usage: ./p2p-project [options]

Program Options:
    --transport_prot:      Transport protocol to use: TcpTahoe, TcpReno [TcpTahoe]
    --s_buf_size:          Sender buffer size (bytes) [131072]
    --r_buf_size:          Receiver buffer size (bytes) [131072]
    --cwnd:                Initial congestion window (segments) [1]
    --ssthresh:            Initial slow start threshold (segments) [65535]
    --mtu:                 Size of IP packets to send (bytes) [1500]
    --sack:                Enable SACK [true]
    --nagle:               Enable Nagle algorithm [false]
    --error_p:             Packet error rate [0]
    --s_bandwidth:         Sender link bandwidth [10Mbps]
    --s_delay:             Sender link delay [40ms]
    --r_bandwidth:         Receiver link bandwidth [10Mbps]
    --r_delay:             Receiver link delay [40ms]
    --tcp_queue_size:      TCP queue size (packets) [25]
    --run:                 Run id [0]
    --n_flows:             Number of flows [1]
    --duration:            Duration of the simulation (s) [3]
    --max_mbytes_to_send:  Maximum number of megabytes to send (MB) [0]
    --prefix_file_name:    Prefix file name [P2P-project]
    --ascii_tracing:       Enable ASCII tracing [false]
    --pcap_tracing:        Enable Pcap tracing [false]

General Arguments:
    --PrintGlobals:              Print the list of globals.
    --PrintGroups:               Print the list of groups.
    --PrintGroup=[group]:        Print all TypeIds of group.
    --PrintTypeIds:              Print all TypeIds.
    --PrintAttributes=[typeid]:  Print all attributes of typeid.
    --PrintVersion:              Print the ns-3 version.
    --PrintHelp:                 Print this help message.
```

The comparison is done using the following scenarios:

| Scenario | Description                                                                   |
| -------- | ----------------------------------------------------------------------------- |
| 1        | TCP Tahoe vs TCP Reno                                                         |
| 2        | TCP Tahoe vs TCP Reno with a 10% packet loss                                  |
| 3        | TCP Tahoe vs TCP Reno with a 10% packet loss and a 10% delay                  |
| 4        | TCP Tahoe vs TCP Reno with a 10% packet loss and a 10% delay and a 10% jitter |

## Results

The results are shown in the following graphs:

### Scenario 1

![Scenario 1](./docs/img/scenario-1.png)

### Scenario 2

![Scenario 2](./docs/img/scenario-1.png)

### Scenario 3

![Scenario 3](./docs/img/scenario-2.png)

### Scenario 4

![Scenario 4](./docs/img/scenario-4.png)

## Conclusion

TCP Tahoe and TCP Reno are very similar. The main difference is that TCP Reno uses a fast retransmit mechanism to detect packet loss and a fast recovery mechanism to recover from packet loss and follows up with a fast recovery phase.

## References

- [ns-3](https://www.nsnam.org/)
- [TCP congestion control](https://datatracker.ietf.org/doc/html/rfc5681)
