# Authentication placeholder — not implemented

The current prototype has no account system or secure identity. The server accepts join handshakes from reachable UDP clients and assigns transient IDs, so it is for loopback/trusted-LAN testing only.

Before public play, define session identity, authenticated connection flow, token lifetime, replay protection, secrets handling and account recovery. Suggested files: session_identity.hpp, auth_service.hpp/.cpp, token_store.hpp/.cpp, auth_tests.cpp. Use a mature password-hashing mechanism if passwords are ever stored; do not build cryptography from scratch and do not reuse the historical test server.
