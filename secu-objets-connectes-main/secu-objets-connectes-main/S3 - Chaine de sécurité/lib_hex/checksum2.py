# -*- coding: utf-8 -*-
"""
Created on Sun Sep 19 13:23:38 2021

@author: darkt
"""
def calc_checksum(record):
    """Returns the checksum of a record"""
    record = [int(record[x:x+2],16) for x in range(1,len(record)-2,2)]
    print(sum(record))
    chksum = ((sum(record) ^ 0xff)+1) & 0xff
    return chksum


def replace_cheksum(record):
    """Replace the checksum with the correct one"""
    return record[:-2] + str(hex(calc_checksum(record))[2:]).upper()# [2:] to remove the "oX" in the hex value


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
    record1 = ":10455000068183207F0FF08FC0210501301C40357B"
    print(replace_cheksum(record1))

if __name__ == "__main__":
    # execute only if run as a script
    main()