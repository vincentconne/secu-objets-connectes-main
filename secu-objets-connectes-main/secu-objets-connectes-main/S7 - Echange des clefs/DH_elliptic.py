from cryptography.hazmat.primitives.kdf.hkdf import HKDF
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey
from cryptography.hazmat.primitives import serialization


def Alice_private():
    Alice_private_key = X25519PrivateKey.generate() # generate Alice's private key
    return Alice_private_key

def Alice_public(Alice_private_key):    
    Alice_public_key  = Alice_private_key.public_key()          # generate Alice's public key
    return Alice_public_key

"""
A_public_key  = Alice_public_key.public_bytes(encoding=serialization.Encoding.PEM,format=serialization.PublicFormat.SubjectPublicKeyInfo)
A_private_key = Alice_private_key.private_bytes(encoding=serialization.Encoding.PEM,format=serialization.PrivateFormat.PKCS8,encryption_algorithm=serialization.NoEncryption())
"""


def Bob_private():
    Bob_private_key = X25519PrivateKey.generate() # generate Bob's private key
    return Bob_private_key

def Bob_public(Bob_private_key):    
    Bob_public_key  = Bob_private_key.public_key()          # generate Bob's public key
    return Bob_public_key

"""
Alice_shared_key = Alice_private_key.exchange(Bob_public_key) # generate Alice's shared key
Bob_shared_key = Bob_private_key.exchange(Alice_public_key) # generate Bob's shared key

# Display Shared Key
print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
print("Shared Secret between Alice and Bob : ")
print("------------------------------------------------")
print("\t - Shared Key compute by Alice : ", Alice_shared_key.hex())
print("\t - Shared Key compute by Bob   : ", Bob_shared_key.hex())
"""
