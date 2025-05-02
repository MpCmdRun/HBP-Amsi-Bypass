# AMSI Bypass via HBP

A proof-of-concept AMSI bypass technique using hardware breakpoints (HBP). This project demonstrates how to alter the behavior of `AmsiScanBuffer` at runtime without modifying AMSI DLLs on disk, by leveraging CPU-level debugging features.

## 🔬 Description

AMSI (Antimalware Scan Interface) is a Microsoft Windows feature that allows applications and services to integrate with antimalware products. This project explores a technique to bypass AMSI by setting a hardware breakpoint on `AmsiScanBuffer` and modifying its behavior in memory.

This bypass:
- Does **not** patch AMSI.dll on disk.
- Uses **debug registers** to monitor execution.
- Hooks into the function during runtime with minimal footprint.

> ⚠️ For educational and research purposes only.

## ⚙️ Features

- AMSI bypass using hardware breakpoints
- Stealthy memory modification
- Works on modern Windows versions (depending on security context)

## 🚀 Usage

```bash
git clone https://github.com/MpCmdRun/HBP-Amsi-Bypass.git
cd AmsiBypassViaHBP
```
