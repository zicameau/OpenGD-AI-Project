set(ZLIB_URL "https://github.com/madler/zlib/archive/refs/tags/v1.2.13.tar.gz")
set(ZLIB_HASH "SHA256=1525952a0a567581792613a9723333d7f8cc20b87a81f920fb8bc7e3f2251428")

_1kfetch_dist(zlib
    URL ${ZLIB_URL}
    URL_HASH ${ZLIB_HASH}
) 