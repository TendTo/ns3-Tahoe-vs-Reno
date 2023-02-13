# TCP comparison

This is a comparison between two old TCP variants: TCP Tahoe and TCP Reno, using the ns-3 simulator.

## TCP Tahoe

TCP Tahoe is the first TCP variant to be implemented in ns-3. It is a simple TCP variant and uses a very simple mechanism to handle congestion.
It is a good starting point to understand how TCP works.

## TCP Reno

TCP Reno is the first TCP variant to be standardized. It is a more complex TCP variant than TCP Tahoe and uses a more complex mechanism to handle congestion.
It uses a fast retransmit mechanism to detect packet loss and a fast recovery mechanism to recover from packet loss and follows up with a fast recovery phase.

## Comparison

The comparison is done using the ns-3 simulator. The comparison is done using the following parameters:

| Parameter                           | Default value | Min value     | Max value       |
| ----------------------------------- | ------------- | ------------- | --------------- |
| Queue size                          | 100 packets   | 1 packet      | 1,000 packets   |
| Queue type                          | DropTailQueue | DropTailQueue | DropTailQueue   |
| Link rate                           | 1 Gbps        | 1 Mbps        | 10 Gbps         |
| Link delay                          | 10 ms         | 1 ms          | 100 ms          |
| Packet size                         | 1,000 bytes   | 1 byte        | 1,000 bytes     |
| TCP segment size                    | 1,000 bytes   | 1 byte        | 1,000 bytes     |
| TCP window size                     | 10,000 bytes  | 1 byte        | 1,000,000 bytes |
| TCP slow start threshold            | 64,000 bytes  | 1 byte        | 1,000,000 bytes |
| TCP initial congestion window       | 1,000 bytes   | 1 byte        | 1,000,000 bytes |
| TCP maximum segment lifetime        | 60 seconds    | 1 second      | 1,000 seconds   |
| TCP maximum retransmission attempts | 3             | 1             | 10              |

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
