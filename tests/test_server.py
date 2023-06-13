import time

import pytest
from pxblat import ClientOption
from pxblat import UsageStats
from pxblat.server import check_port_open
from pxblat.server import Client
from pxblat.server import find_free_port
from pxblat.server import query_server
from pxblat.server import Server
from pxblat.server import start_server_mt_nb
from pxblat.server import status_server
from pxblat.server import stop_server
from pxblat.server import wait_server_ready
from rich import print


def test_server_start_free_func(server_option, port):
    port = find_free_port("localhost", port)

    stat = UsageStats()
    start_server_mt_nb(
        "localhost",
        port,
        "tests/data/test_ref.2bit",
        server_option,
        stat,
        try_new_port=False,
    )

    wait_server_ready("localhost", port)

    status = status_server("localhost", port, server_option)
    assert status


def test_server_start_class(server_option, port, two_bit):
    port += 11
    server = Server("localhost", port, two_bit, server_option)
    server.start()

    assert not server.is_ready()
    server.wait_ready()
    assert server.is_ready()

    status = server.status()
    assert status
    server.stop()


@pytest.mark.parametrize(
    "seqname",
    ["seqname1", None],
)
@pytest.mark.parametrize(
    "parse",
    [True, False],
)
def test_client_for_mem_fa(start_server, fa_seq1, seqname, parse):
    client_option = (
        ClientOption()
        .withMinScore(20)
        .withMinIdentity(90)
        .withHost("localhost")
        .withPort(str(start_server.port))
        .withSeqDir("tests/data/")
        .withInSeq(fa_seq1)
        .build()
    )
    print(client_option)

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
def test_thread_client_for_mem_fa(start_server, fa_seq1, seqname, parse):
    client_option = (
        ClientOption()
        .withMinScore(20)
        .withMinIdentity(90)
        .withHost("localhost")
        .withPort(str(start_server.port))
        .withSeqDir("tests/data/")
        .withInSeq(fa_seq1)
        .build()
    )
    print(client_option)
    client = Client(client_option, seqname=seqname, parse=parse)
    client.start()
    ret = client.get()
    print(ret)


@pytest.mark.parametrize(
    "seqname",
    ["seqname1", None],
)
@pytest.mark.parametrize(
    "parse",
    [True, False],
)
def test_client_for_mem_fa_excep(start_server, fa_seq2, seqname, parse):
    client_option = (
        ClientOption()
        .withMinScore(20)
        .withMinIdentity(90)
        .withHost("localhost")
        .withPort(str(start_server.port))
        .withSeqDir("tests/data/")
        .withInSeq(fa_seq2)
        .build()
    )

    if not parse:
        ret = query_server(client_option, seqname=seqname, parse=parse)
        assert ret
    else:
        with pytest.raises(ValueError):
            ret = query_server(client_option, seqname=seqname, parse=parse)


@pytest.mark.parametrize(
    "seqname",
    ["seqname1", None],
)
@pytest.mark.parametrize(
    "parse",
    [True, False],
)
def test_client_for_fa_file(start_server, fa_file1, seqname, parse):
    client_option = (
        ClientOption()
        .withMinScore(20)
        .withMinIdentity(90)
        .withHost("localhost")
        .withPort(str(start_server.port))
        .withSeqDir("tests/data/")
        .withInName(fa_file1.as_posix())
        .build()
    )

    ret = query_server(client_option, seqname=seqname, parse=parse)
    assert ret


@pytest.mark.skip()
def test_server_stop(server_option, port):
    stat = UsageStats()
    start_server_mt_nb(
        "localhost", port, "tests/data/test_ref.2bit", server_option, stat
    )

    wait_server_ready("localhost", port)
    assert check_port_open("localhost", port)

    stop_server("localhost", port)
    time.sleep(1)
    assert not check_port_open("localhost", port)


@pytest.mark.smoke()
def test_sever_with_context(server_option, port, two_bit, expected_status_instance):
    expected_status_instance.port = port + 1
    with Server("localhost", port + 1, two_bit, server_option) as server:
        server.wait_ready()
        assert server.is_ready()
        status = server.status(True)
        assert status == expected_status_instance
