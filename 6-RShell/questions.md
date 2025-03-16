1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

It's done through messaging framing protocols. To handle partial reads, the client buffers and continues reading until all expected data is received.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol should implement explicit message framing with headers and message type indicators. Something like incomplete reads could happen without proper boundary handling, which would make it hard to reliably match commands with responses.

3. Describe the general differences between stateful and stateless protocols.

Stateful: Maintains session info, remembers client context between requests, provides richer functionality but requires more resources.
Stateless: Treats each request independently with no session storage, more scalable.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

This is used when speed and low latency are more important than delivery. An example of this would be real-time applications like streaming or gaming.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

OSs provide the sockets API as the primary abstraction for network communications. This treats network connections like files through file descriptors.