import asyncio
from pathlib import Path

import pyblat
import simdjson
from invoke import task
from pyblat import extc
from rich import print

PORT = 65000


def option_stat():
    server_option = extc.gfServerOption().withCanStop(True).withStepSize(5).build()
    client_option = (
        extc.gfClientOption()
        .withMinScore(20)
        .withMinIdentity(90)
        .withHost("localhost")
        .withPort(str(PORT))
        .withTSeqDir("tests/data/")
        .withInName("tests/data/test_case1.fa")
        .build()
    )

    stat = extc.UsageStats()

    return server_option, client_option, stat


def test_start_server():
    two_bit_file = Path("tests/data/test_ref.2bit")
    if not two_bit_file.exists():
        extc.faToTwoBit(["tests/data/test_ref.fa"], two_bit_file.as_posix())

    server_option, client_option, stat = option_stat()

    ret = pyblat.server.start_server(
        "localhost", PORT, two_bit_file.as_posix(), server_option, stat
    )
    return ret


def test_server_status():
    server_option, client_option, stat = option_stat()
    extc.statusServer("localhost", str(PORT), server_option)


def test_query_server():
    extc.gfServerOption().withCanStop(True).withStepSize(5).build()
    extc.queryServer(
        "query", "localhost", str(PORT), "tests/data/test_query.fa", False, False
    )


def wait_for_ready(options):
    while extc.statusServer("localhost", str(PORT), options) < 0:
        pass


@task
def server_query(c):
    two_bit_file = Path("tests/data/test_ref.2bit")
    server_option, client_option, stat = option_stat()

    pyblat.server.start_server(
        "localhost", PORT, two_bit_file.as_posix(), server_option, stat
    )

    while True:
        res2 = pyblat.server.status_server("localhost", PORT, server_option)
        if res2.returncode >= 0:
            break

    # print("server is ready")

    # pyblat.server.stop_server("localhost", PORT)
    # print("stopping server")
    # res1.result().join()

    # print(f"{res1.stdout=}")
    # print(f"{res1.stderr=}")

    # extc.stopServer("localhost", "PORT")


@task
def cstatus_server(c, docs=False):
    c.run(f"gfServer status localhost {PORT}")


@task
def cstart_server(c):
    c.run(
        f"./bin/gfServer start localhost {PORT} tests/data/test_ref.2bit -canStop -stepSize=5 -debugLog"
    )


@task
def cstop_server(c):
    c.run(f"./bin/gfServer stop localhost {PORT}")


@task
def cquery_server(c):
    c.run(f"./bin/gfServer query localhost {PORT}  tests/data/test_case1.fa")


@task
def cclient(c):
    c.run(
        f"./bin/gfClient -minScore=20 -minIdentity=90 localhost {PORT} tests/data/ tests/data/test_case1.fa testc.psl"
    )

    # f"{self.gfclient} -minScore=20 -minIdentity={mini_identity} localhost {self.port} . " f"{in_fasta} {out_psl}"


@task
def pclient(c):
    server_option, client_option, stat = option_stat()
    ret = extc.pygfClient(client_option)
    print(client_option)
    print(ret)


@task
def pstart_server(c):
    res = test_start_server()
    print(f"here {res=}")


@task
def pstatus_server(c):
    server_option, client_option, stat = option_stat()
    ret = pyblat.server.status_server("localhost", PORT, server_option)
    print(f"{ret}")


@task
def pstop_server(c):
    res = pyblat.server.stop_server("localhost", PORT)
    print(f"{res=}")


@task
def pquery_server(c):
    ret = extc.pyqueryServer(
        "query", "localhost", str(PORT), "tests/data/test_case1.fa", False, False
    )

    print(f"{ret=}")


@task
def server(c):
    option = extc.gfServerOption().withCanStop(True).withStepSize(5).build()
    two_bit_file = Path("tests/data/test_ref.2bit")

    server = pyblat.server.Server("localhost", PORT, two_bit_file, option)
    server.start()
    server.wait_ready()

    ret = server.status()
    print(f"{ret=}")

    server.stop()
    print("python: server stopped")


@task
def add(c):
    a = 1
    print(f"before {a=}")
    extc.test_add(a)
    print(f"after {a=}")


def gfSignature():
    return b"0ddf270562684f29"


async def aread_stream(reader):
    data = b""
    while True:
        data += await reader.read(100)
        if b"end" in data:
            break
    return data


async def _asc():
    reader, writer = await asyncio.open_connection("localhost", PORT)
    writer.write(gfSignature() + b"status")
    await writer.drain()
    data = await aread_stream(reader)
    writer.close()
    await writer.wait_closed()
    return data


@task
def asc(c):
    data = asyncio.run(_asc())
    print(data)


@task
def sc(c):
    options = (
        extc.gfServerOption()
        .withCanStop(True)
        .withDebugLog(True)
        .withSyslog(True)
        .withStepSize(5)
        .build()
    )
    data = pyblat.server.status_server("localhost", PORT, options)
    print(simdjson.dumps(data, indent=4))


@task
def st(c):
    pyblat.server.stop_server("localhost", PORT)


@task
def ls(c):
    ret = pyblat.server.files("localhost", PORT)
    print(f"{ret=}")


# c server
# hostName: localhost
# portName: 65000
# fileCount: 1
# seqFiles[0]: tests/data/test_ref.2bit
# maxNtSize: 40000
# maxAaSize: 8000
# minMatch: 2
# tileSize: 11
# stepSize: 5
# doTrans: 0
# allowOneMismatch: 0
# noSimpRepMask: 0
# repMatch: 2252 WARN:
# maxDnaHits: 100
# maxTransHits: 200
# maxGap: 2
# seqLog: 0
# ipLog: 0
# doMask: 0
# canStop: 1
# indexFile: (null)
# genome: (null)
# genomeDataDir: (null)
# Counting tiles in tests/data/test_ref.2bit

# p server
# [..indings/gfServer.cpp:1236 (startServer)] hostName = "localhost" (std::string&)
# [..indings/gfServer.cpp:1236 (startServer)] portName = "65000" (std::string&)
# [..indings/gfServer.cpp:1236 (startServer)] fileCount = 1 (int32_t)
# [..indings/gfServer.cpp:1236 (startServer)] cseqFiles = {tests/data/test_ref.2bit} (std::vector<char*>)
# [..indings/gfServer.cpp:1236 (startServer)] options = gfServerOption(canStop: true, log: , logFacility: , mask: false,
# maxAaSize: 8000, maxDnaHits: 100, maxGap: 2,
# maxNtSize: 40000, maxTransHits: 200, minMatch: 2,
# repMatch: 0, seqLog: false, ipLog: false,
# debugLog: false, tileSize: 11, stepSize: 5, trans: false,
# syslog: false, perSeqMax: , noSimpRepMask: false, indexFile: ,
# timeout: 90, genome: , genomeDataDir: , allowOneMismatch: false) (cppbinding::gfServerOption&)

# [..indings/gfServer.cpp:1236 (startServer)] stats = UsageStats(baseCount: 0, blatCount: 0, aaCount: 0,
# pcrCount: 0, warnCount: 0, noSigCount: 0, missCount: 0, trimCount: 0) (cppbinding::UsageStats&)
# Counting tiles in tests/data/test_ref.2bit
