import os
import shlex
import typing
from contextlib import contextmanager
from functools import wraps
from pathlib import Path

from pybind11.setup_helpers import build_ext
from pybind11.setup_helpers import Pybind11Extension


def remove_env(key: str):
    """Remove environment variable."""
    env_cflags = os.environ.get("CFLAGS", "")
    env_cppflags = os.environ.get("CPPFLAGS", "")
    flags = shlex.split(env_cflags) + shlex.split(env_cppflags)

    for flag in flags:
        if flag.startswith(key):
            raise RuntimeError(f"Please remove {key} from CFLAGS and CPPFLAGS.")


def check_conda_env() -> None:
    """Check if conda env is activated."""
    if "CONDA_PREFIX" not in os.environ:
        raise RuntimeError("Please activate conda env first.")


def check_hts_path(hts_lib_path: Path, hts_include_path: Path) -> None:
    """Check if htslib path is valid."""
    header_path = hts_include_path / "htslib"
    if not header_path.exists():
        raise RuntimeError("Please install htslib first.")

    lib_path_linux = hts_lib_path / "libhts.so"
    lib_path_macos = hts_lib_path / "libhts.dylib"
    lib_path_static = hts_lib_path / "libhts.a"

    if (
        not lib_path_linux.exists()
        and not lib_path_static.exists()
        and not lib_path_macos.exists()
    ):
        raise RuntimeError("Please install htslib first.")


def get_hts_lib_path() -> tuple[Path, Path]:
    """Get htslib path."""
    remove_env("-g")
    check_conda_env()
    conda_path = Path(os.environ["CONDA_PREFIX"])

    htslib_library_dir = conda_path / "lib"
    htslib_include_dir = conda_path / "include"

    check_hts_path(htslib_library_dir, htslib_include_dir)

    return htslib_library_dir, htslib_include_dir


# linking against a shared, externally installed htslib version, no
# sources required for htslib
htslib_sources = []
shared_htslib_sources = []
chtslib_sources = []

HTSLIB_LIBRARY_DIR, HTSLIB_INCLUDE_DIR = get_hts_lib_path()

htslib_library_dirs = [HTSLIB_LIBRARY_DIR.as_posix()]
htslib_include_dirs = [HTSLIB_INCLUDE_DIR.as_posix()]
external_htslib_libraries = ["z", "hts", "ssl", "crypto", "m"]


@contextmanager
def change_dir(path: str):
    """Change directory."""
    save_dir = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(save_dir)


def change_env(key: str, value: str):
    """Change environment variable."""

    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            old_env = os.environ.get(key, None)
            os.environ[key] = old_env + " " + value if old_env else value
            func(*args, **kwargs)
            os.environ[key] = old_env if old_env else " "

        return wrapper

    return decorator


def get_files(
    path: typing.Union[Path, str], suffix: typing.List[str]
) -> typing.Iterator[str]:
    """Get bindings."""
    if isinstance(path, str):
        path = Path(path)

    for file in path.iterdir():
        if file.is_dir():
            yield from get_files(file, suffix)
        if file.suffix in suffix:
            yield file.as_posix()


def filter_files(files, exclude=None):
    if exclude is None:
        exclude = []

    for file in files:
        file_name = Path(file).name
        if file_name not in exclude:
            yield file


def get_extra_options():
    return [
        "-D_FILE_OFFSET_BITS=64",
        "-D_LARGEFILE_SOURCE",
        "-D_GNU_SOURCE",
        "-DMACHTYPE_$(MACHTYPE)",
        # "-DCPPBINDING",
    ]


SOURCES = (
    [
        "src/pyblat/extc/bindings/faToTwoBit.cpp",
        "src/pyblat/extc/bindings/gfServer.cpp",
        "src/pyblat/extc/bindings/pygfServer.cpp",
        "src/pyblat/extc/bindings/gfClient.cpp",
    ]
    + list(filter_files(get_files("src/pyblat/extc/bindings/binder", [".cpp"])))
    + list(filter_files(get_files("src/pyblat/extc/src/core", [".c"])))
    + list(
        filter_files(get_files("src/pyblat/extc/src/aux", [".c"]), exclude=["net.c"])
    )
    + list(filter_files(get_files("src/pyblat/extc/src/net", [".c"])))
)

print(SOURCES)


def build(setup_kwargs):
    """Build cpp extension."""
    ext_modules = [
        Pybind11Extension(
            "pyblat._extc",
            language="c++",
            sources=SOURCES,
            include_dirs=htslib_include_dirs
            + [
                "src/pyblat/extc/include/core",
                "src/pyblat/extc/include/aux",
                "src/pyblat/extc/include/net",
                "src/pyblat/extc/bindings",
            ],
            library_dirs=htslib_library_dirs,
            libraries=external_htslib_libraries,
            extra_compile_args=get_extra_options(),
        )
    ]
    setup_kwargs.update(
        {
            "ext_modules": ext_modules,
            "cmdclass": {"build_ext": build_ext},
            "zip_safe": False,
        }
    )
