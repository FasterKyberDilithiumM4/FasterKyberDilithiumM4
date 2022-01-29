#!/usr/bin/env python3

import datetime
import subprocess
import sys
import re
import serial
import numpy as np
from config import Settings
import os.path

def toLog(name, value, k=None):
  if value > 20000:
    value = f"{round(value/1000)}k"
  else:
    value = f"{value}"
  return f"{name}: {value}\n"

def toMacro(name, value, k=None):
  if value > 20000:
    value = f"{round(value/1000):,}k"
  else:
    value = f"{value:,}"
  value = value.replace(",", "\\,")
  return f"\\DefineVar{{{name}_stack_usage}}{{{value}}}\n"

def run_bench(scheme_path, scheme_name, scheme_type):
    subprocess.check_call(f"make -j 12 IMPLEMENTATION_PATH={scheme_path} bin/{scheme_name}_stack.bin", shell=True)
    binary = f"bin/{scheme_name}_stack.bin"
    if os.path.isfile(binary) is False:
        print("Binary does not exist")
        exit()

    try:
        subprocess.check_call(f"st-flash --reset write {binary} 0x8000000", shell=True)
    except:
        print("st-flash failed --> retry")
        return run_bench(scheme_path, scheme_name, scheme_type)

    # get serial output and wait for '#'
    with serial.Serial(Settings.SERIAL_DEVICE, 115200, timeout=10) as dev:
        logs = []
        iteration = 0
        log = b""
        while iteration < 1:
            device_output = dev.read()
            if device_output == b'':
                print("timeout --> retry")
                return run_bench(scheme_path, scheme_name, scheme_type)
            sys.stdout.buffer.write(device_output)
            sys.stdout.flush()
            log += device_output
            if device_output == b'#':
                logs.append(log)
                log = b""
                iteration += 1
    return logs


def parseLogSpeed(log, ignoreErrors):
    log = log.decode(errors="ignore")
    if "error" in log.lower() and not ignoreErrors:
        raise Exception("error in scheme. this is very bad.")
    lines = str(log).splitlines()

    def get(lines, key):
        if key in lines:
            return int(lines[1+lines.index(key)])
        else:
            return None

    def cleanNullTerms(d):
        return {
            k:v
            for k, v in d.items()
            if v is not None
        }

    return cleanNullTerms({
        f"keygen":  get(lines, "keypair stack usage:"),
        f"encaps":  get(lines, "encaps stack usage:"),
        f"decaps":  get(lines, "decaps stack usage:"),
        f"sign":  get(lines, "sign stack usage:"),
        f"verify":  get(lines, "verify stack usage:")
    })

def average(results):
    avgs = dict()
    for key in results[0].keys():
        avgs[key] = int(np.array([results[i][key] for i in range(len(results))]).mean())
    return avgs


def bench(scheme_path, scheme_name, scheme_type, outfile, ignoreErrors=False):
    logs    = run_bench(scheme_path, scheme_name, scheme_type)
    results = []
    for log in logs:
        try:
            result = parseLogSpeed(log, ignoreErrors)
        except:
            breakpoint()
            print("parsing log failed -> retry")
            return bench(scheme_path, scheme_name, scheme_type, outfile)
        results.append(result)

    avgResults = average(results)
    print(f"%M4 results for {scheme_name} (type={scheme_type})", file=outfile)
    scheme_nameStripped = scheme_name.replace("-", "") 
    for key, value in avgResults.items():
        macro = toMacro(f"{scheme_nameStripped}{key}", value)
        print(macro.strip())
        print(macro, end='', file=outfile)
    print('', file=outfile, flush=True)


with open(f"stack_benchmarks.txt", "a") as outfile:

    now = datetime.datetime.now(datetime.timezone.utc)

    print(f"% Stack Benchmarking measurements written on {now};\n", file=outfile)

    subprocess.check_call(f"make clean", shell=True)

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
    # "crypto_sign/dilithium2/old",
    # "crypto_sign/dilithium2/new",
    # "crypto_sign/dilithium3/old",
    # "crypto_sign/dilithium3/new",
    # "crypto_sign/dilithium5/old",
    # "crypto_sign/dilithium5/new"
    ]:
        scheme_name = scheme_path.replace("/", "_")
        scheme_type = re.search('crypto_(.*?)_', scheme_name).group(1)
        bench(scheme_path, scheme_name, scheme_type, outfile)




