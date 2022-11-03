def twos_comp(val, bits):
    """compute the 2's complement of int value val"""
    if (val & (1 << (bits - 1))) != 0: # if sign bit is set e.g., 8bit: 128-255
        val = val - (1 << bits)        # compute negative value
    return val                         # return positive value as is


def least_significant_byte(hexa):
    """Returns the least significant byte of an hexadecimal value"""
    # e.g. the last byte
    return hex(int(hexa, 16) & 0b11111111)


def calc_checksum(record):
    """Returns the checksum of a record"""
    sum = 0 # Sum of bytes in the record
    for i in range(1, len(record) - 2, 2):
        sum += int(record[i:i+2], base=16)
        # print(f"record[i:i+2] {record[i:i+2]}, value {int(record[i:i+2], base=16)}, sum {sum}")
    # print(f"sum {sum} hex {hex(sum)}")
    lsb = least_significant_byte(hex(sum))
    return hex(twos_comp(int(lsb, base=16), 8))


def replace_cheksum(record):
    """Replace the checksum with the correct one"""
    return record[:-2] + calc_checksum(record)[2:] # [2:] to remove the "oX" in the hex value


def decode(record):
    """Displays each field of a record"""
    if record[0] != ":":
        raise Exception("Incorrect start code")
    byte_count = record[1:3]
    adress = record[3:7]
    record_type = record[7:9]
    data = ""
    if record_type == "00": # if record == "01" then the data field is empty 
        data = record[9:9 + 2 * int(byte_count, base=16)]
    checksum = record[-2:] # last byte

    print(f"byte_count {byte_count}, adress {adress}, record_type {record_type}, data {data}, checksum {checksum}")


def main():
    record1 = ":10012000194E79234623965778239EDA3F01B2CAA7"
    decode(record1)
    print(calc_checksum(record1))

if __name__ == "__main__":
    # execute only if run as a script
    main()