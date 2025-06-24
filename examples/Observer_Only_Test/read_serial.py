#!/usr/bin/env python3
import serial
import time
import sys

try:
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    ser.reset_input_buffer()
    
    # Send reset
    ser.setDTR(False)
    time.sleep(0.2)
    ser.setDTR(True)
    
    print("=== Reading serial output (press Ctrl+C to exit) ===")
    
    start_time = time.time()
    while time.time() - start_time < 30:  # Run for 30 seconds
        if ser.in_waiting:
            data = ser.read(ser.in_waiting).decode('utf-8', errors='replace')
            print(data, end='', flush=True)
        time.sleep(0.01)
        
except KeyboardInterrupt:
    print("\nExiting...")
except Exception as e:
    print(f"Error: {e}")
finally:
    if 'ser' in locals():
        ser.close()