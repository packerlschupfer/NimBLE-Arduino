#!/usr/bin/env python3
import serial
import time

print("Opening serial port...")
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
time.sleep(2)  # Wait for device to reset

print("Monitoring serial output...")
print("-" * 60)

try:
    while True:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                print(f"{time.strftime('%H:%M:%S')} | {line}")
except KeyboardInterrupt:
    print("\nMonitoring stopped.")
finally:
    ser.close()