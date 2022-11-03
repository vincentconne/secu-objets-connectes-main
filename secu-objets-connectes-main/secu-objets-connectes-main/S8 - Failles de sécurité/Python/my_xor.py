def charXor(a, b):
    """Returns character a xor b"""
    return chr(ord(a) ^ ord(b))

def Xor(inp,key):
    """Returns xor encrypted inp by key"""
    s = ""
    i = 0
    for c in inp:
        s += charXor(c, key[i])
        i = (i + 1) % len(key)
    return s

def toCdecl(ch,prefix="char tab"):
    outp=list(ch)
    return prefix+"[{}]".format(len(outp)+1)+r"""="\x"""+(r"\x".join(["{:02x}".format(ord(_)) for _ in outp]))+r"""";"""

if __name__ == '__main__':
    # print (toCdecl(Xor("TRAVAUX PRATIQUES ISIMA","F")))
    # print (toCdecl(Xor("TRAVAUX PRATIQUES ISIMA","5")))
    # print (toCdecl(Xor("TRAVAUX PRATIQUES ISIMA","F5")))
    # print (toCdecl(Xor("TRAVAUX PRATIQUES ISIMA","ZZ2_F5")))
    # print (toCdecl(Xor("ZZ2_F5","F5")))
    
    # print(Xor(Xor("TRAVAUX PRATIQUES ISIMA","F"), "F"))
    # print(Xor(Xor("TRAVAUX PRATIQUES ISIMA","5"), "5"))
    # print(Xor(Xor("TRAVAUX PRATIQUES ISIMA","F5"), "F5"))
    # print(Xor(Xor("TRAVAUX PRATIQUES ISIMA","ZZ2_F5"), "ZZ2_F5"))
    # print(Xor(Xor("ZZ2_F5","F5"), "F5")
    print(toCdecl(Xor("AAAAA", "LOTRO")))