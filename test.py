import os

key = 4
# TRUE Worst Case Construction for Naive
# Text: 9.5 million "A"s followed by ONE "B"
# Pattern to use: "AAAAAB" (or "AAAAAAAAAB" for more effect)
# This ensures no match is found until the very last byte, forcing O(N*M).

# 9.5MB (safe for 10MB limit)
count = 9500000 
content = ("s" * count) + "v"

# Encrypt
encrypted_bytes = bytearray()
for char in content:
    val = ord(char)
    enc_val = (val + key) % 256
    encrypted_bytes.append(enc_val)

path = r'c:\Users\adm\DAAMINIPROJECT\SecureFileMatcher\test_data\test1.txt'
with open(path, 'wb') as f:
    f.write(encrypted_bytes)

print(f"Generated {path} with size {len(encrypted_bytes)} bytes.")
