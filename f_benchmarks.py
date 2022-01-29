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
  value = f"{value}"
  return f"{name}: {value}\n"

def toMacro(name, value, k=None):
  value = f"{value:,}"
  value = value.replace(",", "\\,")
  return f"\\DefineVar{{{name}}}{{{value}}}\n"

def run_bench(scheme_path, scheme_name, scheme_type, iterations, nohash):
    cflags=""
    if nohash:
        cflags = "CFLAGS=-Dnohash"
    subprocess.check_call(f"{cflags} make -j 12 IMPLEMENTATION_PATH={scheme_path} CRYPTO_ITERATIONS={iterations} bin/{scheme_name}_f_speed.bin", shell=True)
    binary = f"bin/{scheme_name}_f_speed.bin"
    if os.path.isfile(binary) is False:
        print("Binary does not exist")
        exit()
    try:
        subprocess.check_call(f"st-flash --reset write {binary} 0x8000000", shell=True)
    except:
        print("st-flash failed --> retry")
        return run_bench(scheme_path, scheme_name, scheme_type, iterations, nohash)

    # get serial output and wait for '#'
    with serial.Serial(Settings.SERIAL_DEVICE, 115200, timeout=10) as dev:
        logs = []
        iteration = 0
        log = b""
        while iteration < iterations:
            device_output = dev.read()
            if device_output == b'':
                print("timeout --> retry")
                return run_bench(scheme_path, scheme_name, scheme_type, iterations, nohash)
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
        f"_MatrixVectorMulMataccHash":  get(lines, "MatrixVectorMul cycles (matacc,hash):"),
        f"_MatrixVectorMulMataccNoHash":  get(lines, "MatrixVectorMul cycles (matacc,nohash):"),
        f"_InnerProdEnc":  get(lines, "Inner prod cycles (enc):"),
        f"_InnerProdDec":  get(lines, "Inner prod cycles (dec):"),
        f"_NTT":  get(lines, "NTT cycles:"),
        f"_iNTT":  get(lines, "iNTT cycles:"),
        f"_kyberNTT":  get(lines, "kyber NTT cycles:"),
        f"_kyberiNTT":  get(lines, "kyber iNTT cycles:"),
        f"_smallNTT":  get(lines, "small NTT cycles:"),
        f"_smalliNTT":  get(lines, "small iNTT cycles:"),
        f"_kyberbasemul":  get(lines, "kyber basemul cycles:"),
        f"_smallbasemul":  get(lines, "small basemul cycles:"),
        f"_doublebasemulasm":  get(lines, "doublebasemul_asm cycles:"),
        f"_doublebasemulasmacc":  get(lines, "doublebasemul_asm_acc cycles:"),
        f"_doublebasemulasm_s":  get(lines, "doublebasemul_asm_wrapper cycles:"),
        f"_doublebasemulasmacc_s":  get(lines, "doublebasemul_asm_acc_wrapper cycles:"),
        f"_doublebasemulasmcache1632":  get(lines, "doublebasemul_asm_cache_16_32_wrapper cycles:"),
        f"_doublebasemulasmacccache3232":  get(lines, "doublebasemul_asm_acc_cache_32_32_wrapper cycles:"),
        f"_doublebasemulasmacccache3216":  get(lines, "doublebasemul_asm_acc_cache_32_16_wrapper cycles:"),
        f"_doublebasemulasmopt1632":  get(lines, "doublebasemul_asm_opt_16_32_wrapper cycles:"),
        f"_doublebasemulasmaccopt3232":  get(lines, "doublebasemul_asm_acc_opt_32_32_wrapper cycles:"),
        f"_doublebasemulasmaccopt3216":  get(lines, "doublebasemul_asm_acc_opt_32_16_wrapper cycles:"),
        f"polybasemulopt1632":  get(lines, "poly_basemul_opt_16_32 cycles:"),
        f"polybasemulaccopt3232":  get(lines, "poly_basemul_acc_opt_32_32 cycles:"),
        f"polybasemulaccopt3216":  get(lines, "poly_basemul_acc_opt_32_16 cycles:"),
        f"polybasemul":  get(lines, "poly_basemul cycles:"),
        f"polybasemulacc":  get(lines, "poly_basemul_acc cycles:"),
        f"Nothing":  get(lines, "Nothing cycles:"),
    })

def average(results):
    avgs = dict()
    for key in results[0].keys():
        avgs[key] = int(np.array([results[i][key] for i in range(len(results))]).mean())
    return avgs


def bench(scheme_path, scheme_name, scheme_type, iterations, outfile, nohash, ignoreErrors=False):
    logs    = run_bench(scheme_path, scheme_name, scheme_type, iterations, nohash)
    results = []
    for log in logs:
        try:
            result = parseLogSpeed(log, ignoreErrors)
        except:
            breakpoint()
            print("parsing log failed -> retry")
            return bench(scheme_path, scheme_name, scheme_type, iterations, outfile, nohash)
        results.append(result)

    avgResults = average(results)
    print(f"%M4 results for {scheme_name} (scheme_type={scheme_type})", file=outfile)
    scheme_nameStripped = scheme_name.replace("-", "") 
    for key, value in avgResults.items():
        macro = toMacro(f"{scheme_nameStripped}{key}", value)
        print(macro.strip())
        print(macro, end='', file=outfile)
    print('', file=outfile, flush=True)


with open(f"f_benchmarks.txt", "a") as outfile:
    iterations = 100  # defines the number of measurements to perform
    nohash = False  # defines if hashing should be disabled
    now = datetime.datetime.now(datetime.timezone.utc)
    print(f"% Benchmarking measurements written on {now}; iterations={iterations}, nohash={nohash}\n", file=outfile)

    subprocess.check_call(f"make clean", shell=True)
    
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
    # "crypto_sign/dilithium2/new",
    # "crypto_sign/dilithium3/new",
    # "crypto_sign/dilithium5/new"
    ]:
        scheme_name = scheme_path.replace("/", "_")
        scheme_type = re.search('crypto_(.*?)_', scheme_name).group(1)
        bench(scheme_path, scheme_name, scheme_type, iterations, outfile, nohash)



