#!/bin/bash

INFILE="$1"
OUTFILE="$2"

function usage
{
    echo "Usage:"
    echo "    ${0##*/}: <input file> <output file>"
    echo
    echo "The input file should be a binary bitstream.  The output file is a"
    echo "raw binary sutable for SPI flash programming starting at offset 0."
}

if [ -z "${INFILE}" -o -z "${OUTFILE}" -o ! -e "${INFILE}" ]; then
    usage
    exit 1
fi

if [ -e "${OUTFILE}" ]; then
    echo -n "Warning: replace file ${OUTFILE} (y/n)? "
    read ANSWER
    if [ "${ANSWER}" != "yes" -a "${ANSWER}" != "y" ]; then
        exit 0
    fi
fi

export LC_ALL=C

TMP1=`mktemp`
TMP2=`mktemp`
trap "RET=$? && rm -f ${TMP1} ${TMP2} && exit ${RET}" EXIT

OFFSET=$(grep -obUaP "\xff\xff\xbd\xb3" "${INFILE}" | cut -d: -f1)

if [ -z "${OFFSET}" ]; then
    echo "Unable to locate start of preamble"
    exit 2
fi
if [ ${OFFSET} -lt 0 -o ${OFFSET} -gt 512 ]; then
    echo "Unexpected preamble offset"
    exit 3
fi

if ! dd if="${INFILE}" of=${TMP2} bs=1 skip=${OFFSET} 2>/dev/null; then
    echo "Failed to create padded offset file"
    exit 100
fi
if ! dd if=/dev/zero ibs=1 count=$((0x10002)) 2>/dev/null | tr "\000" "\377" > ${TMP1}; then
    echo "Failed to append binary bitstream file"
    exit 101
fi

cat ${TMP1} ${TMP2} > "${OUTFILE}"

echo "Output file ${OUTFILE} written"
