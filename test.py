#!/usr/bin/env python3

import datetime
import subprocess
import sys

import serial
import numpy as np
from config import Settings
import os.path


def run_test(scheme_path, scheme_name):
    subprocess.check_call(f"make clean", shell=True)
    subprocess.check_call(f"make -j 12 IMPLEMENTATION_PATH={scheme_path} bin/{scheme_name}_test.bin", shell=True)
    binary = f"bin/{scheme_name}_test.bin"
    print(binary)
    if os.path.isfile(binary) is False:
        print("Binary does not exist")
        exit()

    try:
        subprocess.check_call(f"st-flash --reset write {binary} 0x8000000", shell=True)
    except:
        print("st-flash failed --> retry")
        return run_test(scheme_path, scheme_name)

    # get serial output and wait for '#'
    with serial.Serial(Settings.SERIAL_DEVICE, 115200, timeout=10) as dev:
        logs = b""
        log = b""
        device_output = b''
        while device_output != b'#':
            device_output = dev.read()
            if device_output == b'':
                print("timeout --> retry")
                return run_test(scheme_path, scheme_name)
            log += device_output
            if device_output == b'#':
                logs += (log)
                log = b""
    return logs


def test(scheme_path, scheme_name, ignoreErrors=False):
    logs = run_test(scheme_path, scheme_name)
    if (logs.decode("utf-8")).count("OK") == 30:
        print("Success!")
    else:
        print("Failure! Did not receive 30 OKs")


now = datetime.datetime.now(datetime.timezone.utc)
print(f"% Testing implementation(s) {now}\n")

# uncomment the scheme variants that should be build and evaluated
for scheme_path in [
    # "crypto_kem/kyber768/old",
    "crypto_kem/kyber768/new",
    # "crypto_kem/kyber768/newstack",
    # "crypto_kem/kyber512/old",
    # "crypto_kem/kyber512/new",
    # "crypto_kem/kyber512/newstack",
    # "crypto_kem/kyber1024/old",
    # "crypto_kem/kyber1024/new",
    # "crypto_kem/kyber1024/newstack",
    # "crypto_kem/kyber768-90s/old",
    # "crypto_kem/kyber768-90s/new",
    # "crypto_kem/kyber768-90s/newstack",
    # "crypto_kem/kyber512-90s/old",
    # "crypto_kem/kyber512-90s/new",
    # "crypto_kem/kyber512-90s/newstack",
    # "crypto_kem/kyber1024-90s/old",
    # "crypto_kem/kyber1024-90s/new",
    # "crypto_kem/kyber1024-90s/newstack",
    # "crypto_sign/dilithium2/old",
    # "crypto_sign/dilithium2/new",
    # "crypto_sign/dilithium3/old",
    # "crypto_sign/dilithium3/new",
    # "crypto_sign/dilithium5/old",
    # "crypto_sign/dilithium5/new"
    ]:
    scheme_name = scheme_path.replace("/", "_")
    test(scheme_path, scheme_name)
