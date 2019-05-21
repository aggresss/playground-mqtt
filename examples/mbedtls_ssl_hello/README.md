# note for tls_client2

## tls_client2 nbio test

nbio = 0 阻塞； =1 非阻塞； =2 自定义io

```
./tls_client2 server_name=localhost server_port=4433 auth_mode=optional nbio=1 event=1
```
## tls_client2 step

- 0. Initialize the RNG and the session data
- 1.1. Load the trusted CA
- 1.2. Load own certificate and private key
- 2. Start the connection
- 3. Setup stuff
- 4. Handshake
- 5. Verify the server certificate
- 6. Write the GET request
- 7. Read the HTTP response
- 7b. Simulate hard reset and reconnect from same port?
- 7c. Continue doing data exchanges?
- 8. Done, cleanly close the connection
- 9. Reconnect?

## tls_client2 help

```
./tls_client2 --help

 usage: ssl_client2 param=<>...

 acceptable parameters:
    server_name=%s      default: localhost
    server_addr=%s      default: given by name
    server_port=%d      default: 4433
    request_page=%s     default: "."
    request_size=%d     default: about 34 (basic request)
                        (minimum: 0, max: 20000)
                        If 0, in the first exchange only an empty
                        application data message is sent followed by
                        a second non-empty message before attempting
                        to read a response from the server
    debug_level=%d      default: 0 (disabled)
    nbio=%d             default: 0 (blocking I/O)
                        options: 1 (non-blocking), 2 (added delays)
    event=%d            default: 0 (loop)
                        options: 1 (level-triggered, implies nbio=1),
    read_timeout=%d     default: 0 ms (no timeout)
    max_resend=%d       default: 0 (no resend on timeout)

    dtls=%d             default: 0 (TLS)
    hs_timeout=%d-%d    default: (library default: 1000-60000)
                        range of DTLS handshake timeouts in millisecs
    mtu=%d              default: (library default: unlimited)
    dgram_packing=%d    default: 1 (allowed)
                        allow or forbid packing of multiple
                        records within a single datgram.

    auth_mode=%s        default: (library default: none)
                        options: none, optional, required
    ca_file=%s          The single file containing the top-level CA(s) you fully trust
                        default: "" (pre-loaded)
    ca_path=%s          The path containing the top-level CA(s) you fully trust
                        default: "" (pre-loaded) (overrides ca_file)
    crt_file=%s         Your own cert and chain (in bottom to top order, top may be omitted)
                        default: "" (pre-loaded)
    key_file=%s         default: "" (pre-loaded)

    psk=%s              default: "" (in hex, without 0x)
    psk_identity=%s     default: "Client_identity"

    allow_legacy=%d     default: (library default: no)
    renegotiation=%d    default: 0 (disabled)
    renegotiate=%d      default: 0 (disabled)
    exchanges=%d        default: 1
    reconnect=%d        default: 0 (disabled)
    reco_delay=%d       default: 0 seconds
    reconnect_hard=%d   default: 0 (disabled)
    tickets=%d          default: 1 (enabled)
    max_frag_len=%d     default: 16384 (tls default)
                        options: 512, 1024, 2048, 4096
    trunc_hmac=%d       default: library default
    alpn=%s             default: "" (disabled)
                        example: spdy/1,http/1.1
    fallback=0/1        default: (library default: off)
    extended_ms=0/1     default: (library default: on)
    etm=0/1             default: (library default: on)
    curves=a,b,c,d      default: "default" (library default)
                        example: "secp521r1,brainpoolP512r1"
                        - use "none" for empty list
                        - see mbedtls_ecp_curve_list()
                          for acceptable curve names
    recsplit=0/1        default: (library default: on)
    dhmlen=%d           default: (library default: 1024 bits)

    arc4=%d             default: (library default: 0)
    allow_sha1=%d       default: 0
    min_version=%s      default: (library default: tls1)
    max_version=%s      default: (library default: tls1_2)
    force_version=%s    default: "" (none)
                        options: ssl3, tls1, tls1_1, tls1_2, dtls1, dtls1_2

    force_ciphersuite=<name>    default: all enabled
    query_config=<name>         return 0 if the specified
                                configuration macro is defined and 1
                                otherwise. The expansion of the macro
                                is printed if it is defined
 acceptable ciphersuite names:
 TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256 TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256
 TLS-DHE-RSA-WITH-CHACHA20-POLY1305-SHA256  TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384
 TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384      TLS-DHE-RSA-WITH-AES-256-GCM-SHA384
 TLS-ECDHE-ECDSA-WITH-AES-256-CCM           TLS-DHE-RSA-WITH-AES-256-CCM
 TLS-ECDHE-ECDSA-WITH-AES-256-CBC-SHA384    TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA384
 TLS-DHE-RSA-WITH-AES-256-CBC-SHA256        TLS-ECDHE-ECDSA-WITH-AES-256-CBC-SHA
 TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA         TLS-DHE-RSA-WITH-AES-256-CBC-SHA
 TLS-ECDHE-ECDSA-WITH-AES-256-CCM-8         TLS-DHE-RSA-WITH-AES-256-CCM-8
 TLS-ECDHE-ECDSA-WITH-CAMELLIA-256-GCM-SHA384 TLS-ECDHE-RSA-WITH-CAMELLIA-256-GCM-SHA384
 TLS-DHE-RSA-WITH-CAMELLIA-256-GCM-SHA384   TLS-ECDHE-ECDSA-WITH-CAMELLIA-256-CBC-SHA384
 TLS-ECDHE-RSA-WITH-CAMELLIA-256-CBC-SHA384 TLS-DHE-RSA-WITH-CAMELLIA-256-CBC-SHA256
 TLS-DHE-RSA-WITH-CAMELLIA-256-CBC-SHA      TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256
 TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256      TLS-DHE-RSA-WITH-AES-128-GCM-SHA256
 TLS-ECDHE-ECDSA-WITH-AES-128-CCM           TLS-DHE-RSA-WITH-AES-128-CCM
 TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA256    TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA256
 TLS-DHE-RSA-WITH-AES-128-CBC-SHA256        TLS-ECDHE-ECDSA-WITH-AES-128-CBC-SHA
 TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA         TLS-DHE-RSA-WITH-AES-128-CBC-SHA
 TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8         TLS-DHE-RSA-WITH-AES-128-CCM-8
 TLS-ECDHE-ECDSA-WITH-CAMELLIA-128-GCM-SHA256 TLS-ECDHE-RSA-WITH-CAMELLIA-128-GCM-SHA256
 TLS-DHE-RSA-WITH-CAMELLIA-128-GCM-SHA256   TLS-ECDHE-ECDSA-WITH-CAMELLIA-128-CBC-SHA256
 TLS-ECDHE-RSA-WITH-CAMELLIA-128-CBC-SHA256 TLS-DHE-RSA-WITH-CAMELLIA-128-CBC-SHA256
 TLS-DHE-RSA-WITH-CAMELLIA-128-CBC-SHA      TLS-ECDHE-PSK-WITH-CHACHA20-POLY1305-SHA256
 TLS-DHE-PSK-WITH-CHACHA20-POLY1305-SHA256  TLS-DHE-PSK-WITH-AES-256-GCM-SHA384
 TLS-DHE-PSK-WITH-AES-256-CCM               TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA384
 TLS-DHE-PSK-WITH-AES-256-CBC-SHA384        TLS-ECDHE-PSK-WITH-AES-256-CBC-SHA
 TLS-DHE-PSK-WITH-AES-256-CBC-SHA           TLS-DHE-PSK-WITH-CAMELLIA-256-GCM-SHA384
 TLS-ECDHE-PSK-WITH-CAMELLIA-256-CBC-SHA384 TLS-DHE-PSK-WITH-CAMELLIA-256-CBC-SHA384
 TLS-DHE-PSK-WITH-AES-256-CCM-8             TLS-DHE-PSK-WITH-AES-128-GCM-SHA256
 TLS-DHE-PSK-WITH-AES-128-CCM               TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA256
 TLS-DHE-PSK-WITH-AES-128-CBC-SHA256        TLS-ECDHE-PSK-WITH-AES-128-CBC-SHA
 TLS-DHE-PSK-WITH-AES-128-CBC-SHA           TLS-DHE-PSK-WITH-CAMELLIA-128-GCM-SHA256
 TLS-DHE-PSK-WITH-CAMELLIA-128-CBC-SHA256   TLS-ECDHE-PSK-WITH-CAMELLIA-128-CBC-SHA256
 TLS-DHE-PSK-WITH-AES-128-CCM-8             TLS-RSA-WITH-AES-256-GCM-SHA384
 TLS-RSA-WITH-AES-256-CCM                   TLS-RSA-WITH-AES-256-CBC-SHA256
 TLS-RSA-WITH-AES-256-CBC-SHA               TLS-ECDH-RSA-WITH-AES-256-GCM-SHA384
 TLS-ECDH-RSA-WITH-AES-256-CBC-SHA384       TLS-ECDH-RSA-WITH-AES-256-CBC-SHA
 TLS-ECDH-ECDSA-WITH-AES-256-GCM-SHA384     TLS-ECDH-ECDSA-WITH-AES-256-CBC-SHA384
 TLS-ECDH-ECDSA-WITH-AES-256-CBC-SHA        TLS-RSA-WITH-AES-256-CCM-8
 TLS-RSA-WITH-CAMELLIA-256-GCM-SHA384       TLS-RSA-WITH-CAMELLIA-256-CBC-SHA256
 TLS-RSA-WITH-CAMELLIA-256-CBC-SHA          TLS-ECDH-RSA-WITH-CAMELLIA-256-GCM-SHA384
 TLS-ECDH-RSA-WITH-CAMELLIA-256-CBC-SHA384  TLS-ECDH-ECDSA-WITH-CAMELLIA-256-GCM-SHA384
 TLS-ECDH-ECDSA-WITH-CAMELLIA-256-CBC-SHA384 TLS-RSA-WITH-AES-128-GCM-SHA256
 TLS-RSA-WITH-AES-128-CCM                   TLS-RSA-WITH-AES-128-CBC-SHA256
 TLS-RSA-WITH-AES-128-CBC-SHA               TLS-ECDH-RSA-WITH-AES-128-GCM-SHA256
 TLS-ECDH-RSA-WITH-AES-128-CBC-SHA256       TLS-ECDH-RSA-WITH-AES-128-CBC-SHA
 TLS-ECDH-ECDSA-WITH-AES-128-GCM-SHA256     TLS-ECDH-ECDSA-WITH-AES-128-CBC-SHA256
 TLS-ECDH-ECDSA-WITH-AES-128-CBC-SHA        TLS-RSA-WITH-AES-128-CCM-8
 TLS-RSA-WITH-CAMELLIA-128-GCM-SHA256       TLS-RSA-WITH-CAMELLIA-128-CBC-SHA256
 TLS-RSA-WITH-CAMELLIA-128-CBC-SHA          TLS-ECDH-RSA-WITH-CAMELLIA-128-GCM-SHA256
 TLS-ECDH-RSA-WITH-CAMELLIA-128-CBC-SHA256  TLS-ECDH-ECDSA-WITH-CAMELLIA-128-GCM-SHA256
 TLS-ECDH-ECDSA-WITH-CAMELLIA-128-CBC-SHA256 TLS-RSA-PSK-WITH-CHACHA20-POLY1305-SHA256
 TLS-RSA-PSK-WITH-AES-256-GCM-SHA384        TLS-RSA-PSK-WITH-AES-256-CBC-SHA384
 TLS-RSA-PSK-WITH-AES-256-CBC-SHA           TLS-RSA-PSK-WITH-CAMELLIA-256-GCM-SHA384
 TLS-RSA-PSK-WITH-CAMELLIA-256-CBC-SHA384   TLS-RSA-PSK-WITH-AES-128-GCM-SHA256
 TLS-RSA-PSK-WITH-AES-128-CBC-SHA256        TLS-RSA-PSK-WITH-AES-128-CBC-SHA
 TLS-RSA-PSK-WITH-CAMELLIA-128-GCM-SHA256   TLS-RSA-PSK-WITH-CAMELLIA-128-CBC-SHA256
 TLS-PSK-WITH-CHACHA20-POLY1305-SHA256      TLS-PSK-WITH-AES-256-GCM-SHA384
 TLS-PSK-WITH-AES-256-CCM                   TLS-PSK-WITH-AES-256-CBC-SHA384
 TLS-PSK-WITH-AES-256-CBC-SHA               TLS-PSK-WITH-CAMELLIA-256-GCM-SHA384
 TLS-PSK-WITH-CAMELLIA-256-CBC-SHA384       TLS-PSK-WITH-AES-256-CCM-8
 TLS-PSK-WITH-AES-128-GCM-SHA256            TLS-PSK-WITH-AES-128-CCM
 TLS-PSK-WITH-AES-128-CBC-SHA256            TLS-PSK-WITH-AES-128-CBC-SHA
 TLS-PSK-WITH-CAMELLIA-128-GCM-SHA256       TLS-PSK-WITH-CAMELLIA-128-CBC-SHA256
 TLS-PSK-WITH-AES-128-CCM-8
Last error was: -0xFFFFFFFF - UNKNOWN ERROR CODE (0001)
```
