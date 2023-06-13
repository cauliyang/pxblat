import pxblat
from pathlib import Path
import sys

import subprocess
import time
import pytest
from rich import print
from multiprocessing import Process


@pytest.fixture
def fas():
    return Path("tests/data/fas")


def is_linux():
    return sys.platform.startswith("linux")


def is_mac():
    return sys.platform.startswith("darwin")


def find_base_path():
    base_path = Path("./tests") / "blat"

    if is_linux():
        blat_path = base_path / "linux"
    elif is_mac():
        blat_path = base_path / "darwin"
    else:
        raise RuntimeError("Unsupported platform.")

    return blat_path


def find_gfserver():
    path = find_base_path() / "gfserver"
    path.chmod(0o755)
    return path


def find_gfclient():
    path = find_base_path() / "gfclient"
    path.chmod(0o755)
    return path


def run_cmd(cmd):
    subprocess.run(cmd, shell=True)


def run_cblat(result_dir: Path, port: int, fa_data: Path):
    cport = port + 20
    two_bit = Path("benchmark/data/chr20.2bit")

    gfserver = find_gfserver()
    gfclient = find_gfclient()

    print("open c server")

    p = Process(
        target=run_cmd,
        args=(f"{gfserver} start localhost {cport} {two_bit} -canStop -stepSize=5",),
    )
    p.start()
    time.sleep(5)

    for _, fa in enumerate(fa_data.glob("*.fa")):
        print(f"run cc {fa}")
        cc_res = result_dir / f"{fa.stem}_cc.psl"
        run_cmd(
            f"{gfclient} -minScore=20 -minIdentity=90 localhost {cport} {two_bit.parent.as_posix()} {fa} {cc_res}"
        )

    print("stop c server")
    run_cmd(f"{gfserver} stop localhost {cport}")


def run_pxblat(result_dir: Path, port: int, fa_data: Path):
    cport = port + 20
    two_bit = Path("benchmark/data/chr20.2bit")
    server_option = pxblat.gfServerOption().withCanStop(True).withStepSize(5).build()

    print("open python server")
    server = pxblat.Server("localhost", cport, two_bit, server_option)
    server.start()
    server.wait_ready()
    results = {}

    for _, fa in enumerate(fa_data.glob("*.fa")):
        client_option = (
            pxblat.gfClientOption()
            .withMinScore(20)
            .withMinIdentity(90)
            .withHost("localhost")
            .withPort(str(cport))
            .withSeqDir(two_bit.parent.as_posix())
            .withInName(fa.as_posix())
            .build()
        )

        client = pxblat.Client(client_option)
        client.start()
        ret = client.get()
        results[fa.stem] = ret

    server.stop()
    return results


def get_key_hsp(hsp):
    key = ""

    for i in sorted(hsp.query_start_all):
        key += str(i)

    for i in sorted(hsp.query_end_all):
        key += str(i)

    for i in sorted(hsp.hit_start_all):
        key += str(i)

    for i in sorted(hsp.hit_end_all):
        key += str(i)

    return key


def compare_hsp(hsp1, hsp2):
    return (
        hsp1.query_start_all == hsp2.query_start_all
        and hsp1.query_end_all == hsp2.query_end_all
        and hsp1.hit_start_all == hsp2.hit_start_all
        and hsp1.hit_end_all == hsp2.hit_end_all
    )


def get_overlap(hsps1, hsps2):
    set1 = {get_key_hsp(i) for i in hsps1}
    set2 = {get_key_hsp(i) for i in hsps2}
    return set1 - set2, set2 - set1, set1 & set2


def _cpsl(file1, file2, isprint=True):
    cc_psl = file1
    cp_psl = file2

    from Bio import SearchIO

    cc_res = SearchIO.read(cc_psl, "blat-psl")

    if isinstance(cp_psl, str) or isinstance(cp_psl, Path):
        cp_res = SearchIO.read(cp_psl, "blat-psl")
    else:
        cp_res = cp_psl

    cc_hsps = cc_res.hsps
    cp_hsps = cp_res.hsps

    cc_hsps.sort(key=lambda x: x.score, reverse=True)
    cp_hsps.sort(key=lambda x: x.score, reverse=True)

    if isprint:
        for i in range(5):
            print(f"id {i} CC:")
            print(cc_hsps[i])
            print(f"id {i} CP:")
            print(cp_hsps[i])
            print(f"compare same:  {compare_hsp(cc_hsps[i], cp_hsps[i])}")
            print("\n")

    return get_overlap(cc_hsps, cp_hsps)


def create_result(result_dir, port, fas):
    run_cblat(result_dir, port, fas)
    results = run_pxblat(result_dir, port, fas)
    return results


def test_result(tmpdir, port, fas):
    pxblat_results = create_result(tmpdir, port, fas)

    for fa in fas.glob("*fa"):
        cc_res = tmpdir / f"{fa.stem}_cc.psl"
        pp_res = pxblat_results[fa.stem]
        a, b, _ = _cpsl(cc_res, pp_res, False)
        assert len(a) == 0
        assert len(b) == 0
