import time

import pytest
from pxblat import extc
from pxblat.server import check_port_open
from pxblat.server import query_server
from pxblat.server import Server
from pxblat.server import start_server_mt_nb
from pxblat.server import status_server
from pxblat.server import stop_server
from pxblat.server import wait_server_ready
from rich import print


def test_server_start_free_func(server_option, port):
    stat = extc.UsageStats()
    start_server_mt_nb(
        "localhost", port, "tests/data/test_ref.2bit", server_option, stat
    )

    wait_server_ready("localhost", port)

    status = status_server("localhost", port, server_option)

    print(status)
    assert status


def test_server_start_class(server_option, port, two_bit):
    server = Server("localhost", port, two_bit, server_option)
    server.start()

    assert not server.is_ready()
    server.wait_ready()
    assert server.is_ready()

    status = server.status()

    print(status)


@pytest.mark.parametrize(
    "seqname",
    ["seqname1", None],
)
@pytest.mark.parametrize(
    "parse",
    [True, False],
)
def test_client_for_mem_fa(start_server, port, fa_seq1, seqname, parse):
    client_option = (
        extc.gfClientOption()
        .withMinScore(20)
        .withMinIdentity(90)
        .withHost("localhost")
        .withPort(str(port))
        .withSeqDir("tests/data/")
        .withInSeq(fa_seq1)
        .build()
    )

    ret = query_server(client_option, seqname=seqname, parse=parse)
    assert ret


@pytest.mark.parametrize(
    "seqname",
    ["seqname1", None],
)
@pytest.mark.parametrize(
    "parse",
    [True, False],
)
def test_client_for_fa_file(start_server, port, fa_file1, seqname, parse):
    client_option = (
        extc.gfClientOption()
        .withMinScore(20)
        .withMinIdentity(90)
        .withHost("localhost")
        .withPort(str(port))
        .withSeqDir("tests/data/")
        .withInName(fa_file1)
        .build()
    )

    ret = query_server(client_option, seqname=seqname, parse=parse)
    assert ret


def test_server_stop(server_option, port):
    stat = extc.UsageStats()
    start_server_mt_nb(
        "localhost", port, "tests/data/test_ref.2bit", server_option, stat
    )

    wait_server_ready("localhost", port)
    assert check_port_open("localhost", port)

    stop_server("localhost", port)
    time.sleep(1)
    assert not check_port_open("localhost", port)
