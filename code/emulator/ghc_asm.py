#!/usr/bin/python
'''
Created on 2011/12/11

@author: nodchip
'''
import sys
import re

VARIABLE_PATTERN = re.compile('\\[[a-z0-9_]+?\\]')
INCLUDED_FILE_NAMES = set()

def expand_includes(lines):
    output=[]
    for line in lines:
        split = line.split()
        if len(split) == 0:
            continue
        if split[0] == '#include':
            file_name = split[1]
            if file_name in INCLUDED_FILE_NAMES:
                continue
            INCLUDED_FILE_NAMES.add(file_name)
            output.extend(expand_includes(open(split[1]).readlines()))
        else:
            output.append(line)
    return output


def parse_macros(lines):
    macros = dict()
    for line in lines:
        split = line.split()
        if len(split) == 0:
            continue
        if split[0] == '#define':
            macros[split[1]] = split[2]
    
    for key1 in macros.keys():
        for key2 in macros.keys():
            if key1 == key2:
                continue
            if key1.find(key2) != -1:
                raise Exception('macro %s contains %s' % (key1, key2))
            
    return macros

    
def remove_comments_macros(lines):
    output = []
    for line in lines:
        if line.strip().startswith('#'):
            continue
        index = line.find(';')
        if index != -1:
            line = line[0:index]
        line = line.strip()
        if len(line) == 0:
            continue
        output.append(line)
    return output


def expand_call_ret(lines):
    output = []
    for line in lines:
        split = line.split()
        if split[0] == 'call':
            output.append('mov [return_address],pc')
            output.append('add [return_address],3')
            output.append('mov pc,' + split[1])
        elif split[0] == 'ret':
            output.append('mov pc,[return_address]')
        else:
            output.append(line)
    return output


def allocate_variable_addresses(lines):
    address = 0
    variables = dict()
    for line in lines:
        for match_object in re.finditer(VARIABLE_PATTERN, line):
            variable = match_object.group(0)
            if variable in variables:
                continue
            variables[variable] = address
            address += 1
    return variables


def decide_label_addresses(lines):
    address = 0
    addresses = dict()
    for line in lines:
        if line.endswith(':'):
            line = line[0:-1]
            addresses[line] = address
        else:
            address += 1
    return addresses


def main():
    lines = []
    for line in sys.stdin:
        lines.append(line)
    lines = expand_includes(lines)
    macros = parse_macros(lines)
    lines = remove_comments_macros(lines)
    lines = expand_call_ret(lines)
    variable_addresses = allocate_variable_addresses(lines)
    label_addresses = decide_label_addresses(lines)
    
    for line in lines:
        if line.endswith(':'):
            continue
        for src, dst in macros.items():
            line = line.replace(src, dst)
        for variable, address in variable_addresses.items():
            line = line.replace(variable, '[%d]' % address)
        for label, address in label_addresses.items():
            line = line.replace(label, '%d' % address)
        print line
    

if __name__ == '__main__':
    main()
