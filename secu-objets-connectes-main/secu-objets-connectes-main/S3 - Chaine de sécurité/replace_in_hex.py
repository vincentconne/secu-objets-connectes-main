import lib_hex.checksum2 as check
import lib_hex.decode_hex as decode


def replace_in_hex(old, new, file_in, file_out):
    """Replace old to new in the file_name hex file"""
    old_hex = ascii_to_hex(old).upper()
    new_hex = ascii_to_hex(new).upper()
    
    line_nb = 0

    with open(file_in, "r", encoding="utf-8") as f:
        with open(file_out, "w", encoding="utf-8") as out:
            for line in f:
                if old_hex in decode.data(line):
                    new_line = replace_line(line, old_hex, new_hex)
                    # replace checksum
                    out.write(new_line)
                    print(f"{line_nb} : {line} a été remplacé par {new_line}")
                else:
                    out.write(line)
            line_nb += 1


def replace_line(line, old, new):
    """Replace old by new in line"""
    n = len(line)
    checksum = decode.checksum(line)
    
    old_data = decode.data(line)
    new_data = old_data.replace(old, new)
    new_line = line.replace(old_data, new_data)
    new_line_wo_ch = new_line[:-2]
    
    # bourrage
    while len(new_line_wo_ch) < n - 2:
        new_line_wo_ch += "0"
    
    # 0 en trop à la fin
    while (len(new_line_wo_ch) > n - 2) and (new_line_wo_ch[-1] == "0"):
        new_line_wo_ch = new_line_wo_ch[:-1]
    
    new_line = new_line_wo_ch + checksum
    return check.replace_cheksum(new_line)


def ascii_to_hex(str):
    """Returns hex code from ascii string"""
    return str.encode("ascii").hex()


def main():
    replace_in_hex("EDUROAM", "ZZ_SH", "./S3 - Chaine de sécurité/firmware_tp3_v2.33.hex", "./S3 - Chaine de sécurité/out.hex")
    decode.decode_file("./S3 - Chaine de sécurité/out.hex", out="./S3 - Chaine de sécurité/decode.txt")


if __name__ == "__main__":
    main()