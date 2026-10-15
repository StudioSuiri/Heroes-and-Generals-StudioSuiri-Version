# Native UDP Protocol v1

Development-only wire format. Multi-byte integers use little-endian order.

## Header (16 bytes)

| Offset | Size | Field | Rule |
|---:|---:|---|---|
| 0 | 4 | Magic | ASCII HNGN |
| 4 | 1 | Protocol version | 1 |
| 5 | 1 | Message type | 1 Hello, 2 Welcome, 3 Input, 4 Snapshot, 5 Goodbye |
| 6 | 2 | Payload length | Equals datagram length minus 16 |
| 8 | 4 | Sequence | Monotonic input sequence / server tick |
| 12 | 4 | Client ID | Zero during Hello, server-assigned afterward |
| 16 | variable | Payload | Explicit serialization; never send raw C++ struct memory |

Maximum datagram size: 1200 bytes.

## Payloads

- Hello: empty. The client repeats it until it receives Welcome and then as a low-rate keepalive.
- Welcome: faction u8, world width u16, world height u16.
- Input: move X i8, move Y i8, aim X i16, aim Y i16, buttons u8. Movement is clamped to -1/0/+1; bit 0 is fire.
- Snapshot: server tick u32, capture progress i16 (-100..100), player count u8, then player records (14 bytes each): ID u32, faction u8, X i16, Y i16, health u8, kills u16, deaths u16.
- Goodbye: empty.

## V1 limitations

This is not a finished public-game protocol. It lacks cryptographic session identity, encryption, ACK-based congestion control, reliable handling of critical events, robust rate limiting, NAT traversal, matchmaking, anti-cheat, authentication, interest management, resumption tokens and migration compatibility. Do not expose this prototype to the public internet. Extend it only with explicit, versioned messages and test fixtures.
