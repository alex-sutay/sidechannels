From github.com/bbbrumley/portsmash
# Summary

This is a proof-of-concept exploit of the PortSmash microarchitecture attack, tracked by [CVE-2018-5407](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-5407).

More technical details about the PortSmash side-channel are available in [this manuscript](https://eprint.iacr.org/2018/1060).

![Alt text](parse_raw_simple.png?raw=true "sample PortSmash visualization tooling")

# License

This software is licensed under the terms of the [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0).

Check [LICENSE](LICENSE) and [NOTICE](NOTICE) for more details.

# Setup

## Prerequisites

A CPU featuring SMT (e.g. Hyper-Threading) is the only requirement.

This exploit code should work out of the box on Skylake and Kaby Lake. For other SMT architectures, customizing the strategies and/or waiting times in `spy` is likely needed.

## OpenSSL

Download and install OpenSSL 1.1.0h or lower:

    cd /usr/local/src
    wget https://www.openssl.org/source/openssl-1.1.0h.tar.gz
    tar xzf openssl-1.1.0h.tar.gz
    cd openssl-1.1.0h/
    export OPENSSL_ROOT_DIR=/usr/local/ssl
    ./config -d shared --prefix=$OPENSSL_ROOT_DIR --openssldir=$OPENSSL_ROOT_DIR -Wl,-rpath=$OPENSSL_ROOT_DIR/lib
    make -j8
    make test
    sudo checkinstall --strip=no --stripso=no --pkgname=openssl-1.1.0h-debug --provides=openssl-1.1.0h-debug --default make install_sw

If you use a different path, you'll need to make changes to `Makefile` and `sync.sh`.

# Tooling

## freq.sh

Turns off frequency scaling and TurboBoost.

## sync.sh

Sync trace through pipes. It has two victims, one of which should be active at a time:

1. The stock `openssl` running `dgst` command to produce a P-384 signature.
2. A harness `ecc` that calls scalar multiplication directly with a known key. (Useful for profiling.)

The script will generate a P-384 key pair in `secp384r1.pem` if it does not already exist.

The script outputs `data.bin` which is what `openssl dgst` signed, and you should be able to verify the ECDSA signature `data.sig` afterwards with

    openssl dgst -sha512 -verify secp384r1.pem -signature data.sig data.bin

In the `ecc` tool case, `data.bin` and `secp384r1.pem` are meaningless and `data.sig` is not created.

For the `taskset` commands in `sync.sh`, the cores need to be two logical cores of the same physical core; sanity check with

    $ grep '^processor\|^core id' /proc/cpuinfo
    processor : 0
    core id   : 0
    processor : 1
    core id   : 1
    processor : 2
    core id   : 2
    processor : 3
    core id   : 3
    processor : 4
    core id   : 0
    processor : 5
    core id   : 1
    processor : 6
    core id   : 2
    processor : 7
    core id   : 3

So the script is currently configured for logical cores 3 and 7 (`processor`) that both map to physical core 3 (`core_id`).

## spy

Measurement process that outputs measurements in `timings.bin`. To change the `spy` strategy, check the port defines in `spy.h`. Only one strategy should be active at build time.

Note that `timings.bin` is actually raw clock cycle counter values, not latencies. Look in `parse_raw_simple.py` to understand the data format if necessary.

## ecc

Victim harness for running OpenSSL scalar multiplication with known inputs. Example:

    ./ecc M 4 deadbeef0123456789abcdef00000000c0ff33

Will execute 4 consecutive calls to `EC_POINT_mul` with the given hex scalar.

## parse_raw_simple.py

Quick and dirty hack to view 1D traces. The top plot is the raw trace. Everything below is a different digital filter of the raw trace for viewing purposes. Zoom and pan are your friends here.

You might have to adjust the `CEIL` variable if the plots are too aggressively clipped.

Python packages:

    sudo apt-get install python-numpy python-matplotlib python-scipy

Optional but recommended to view peaks:

    sudo pip install --upgrade scipy

# Usage

Turn off frequency scaling:

    ./freq.sh

Make sure everything builds:

    make clean
    make

Take a measurement:

    ./sync.sh

View the trace:

    python parse_raw_simple.py timings.bin

You can play around with one victim at a time in `sync.sh`. Sample output for the `openssl dgst` victim is in `parse_raw_simple.png`.

# Credits

## Authors

* Alejandro Cabrera Aldaya (Universidad Tecnológica de la Habana (CUJAE), Habana, Cuba)
* Billy Bob Brumley (Tampere University, Tampere, Finland)
* Sohaib ul Hassan (Tampere University, Tampere, Finland)
* Cesar Pereida García (Tampere University, Tampere, Finland)
* Nicola Tuveri (Tampere University, Tampere, Finland)

## Funding

This project has received funding from the European Research Council (ERC) under the European Union's Horizon 2020 research and innovation programme (grant agreement No 804476).
