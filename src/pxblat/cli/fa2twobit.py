from pathlib import Path

import typer

from pxblat.server import fa_to_two_bit
from .log import log

# faToTwoBit - Convert DNA from fasta to 2bit format
# usage:
#    faToTwoBit in.fa [in2.fa in3.fa ...] out.2bit
# options:
#    -long          use 64-bit offsets for index.   Allow for twoBit to contain more than 4Gb of sequence.
#                   NOT COMPATIBLE WITH OLDER CODE.
#    -noMask        Ignore lower-case masking in fa file.
#    -stripVersion  Strip off version number after '.' for GenBank accessions.
#    -ignoreDups    Convert first sequence only if there are duplicate sequence
#                   names.  Use 'twoBitDup' to find duplicate sequences.
#


def faToTwoBit(
    infa: list[Path] = typer.Argument(
        ...,
        exists=True,
        dir_okay=False,
        readable=True,
        metavar="in.fa [inf2.fa in3.fa ...]",
        show_default=False,
        help="The fasta files",
    ),
    out2bit: Path = typer.Argument(
        ...,
        dir_okay=False,
        writable=True,
        show_default=False,
        metavar="out.2bit",
        help="The output file",
    ),
    long: bool = typer.Option(
        False,
        "--long",
        help="Use 64-bit offsets for index. Allow for twoBit to contain more than 4Gb of sequence.",
    ),
    noMask: bool = typer.Option(
        False, "--nomask", help="Ignore lower-case masking in fa file."
    ),
    stripVersion: bool = typer.Option(
        False,
        "--stripVersion",
        help="Strip off version number after '.' for GenBank accessions.",
    ),
    ignoreDups: bool = typer.Option(
        False,
        "--ignoreDups",
        help="Convert first sequence only if there are duplicate sequence names. Use 'twoBitDup' to find duplicate sequences.",
    ),
):
    """Convert DNA from fasta to 2bit format"""
    for file in infa:
        if not file.exists():
            log.error(f"{file} not exists")
            raise typer.Abort()
        elif not file.is_file():
            log.error(f"{file} is not a file")
            raise typer.Abort()

    if out2bit.exists():
        log.warning(f"{out2bit} exist will be overide")

    fa_to_two_bit(
        [f.as_posix() for f in infa],
        out2bit.as_posix(),
        noMask,
        stripVersion,
        ignoreDups,
        long,
    )
