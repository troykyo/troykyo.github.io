#!/usr/bin/env python3
"""
build-day-gpx.py — one-shot generator.

Merges the 10 stage tracks (14–23) into 8 per-day GPX files for Strava Route
upload. Days 7 and 8 each combine two stages; the rest are one-to-one.

Mirrors output into both pilgrimage/ and pilgramage/ — the two folders that
must stay byte-identical.

Usage:
    python3 scripts/build-day-gpx.py              # build files
    python3 scripts/build-day-gpx.py --report     # print per-day km only
"""
import math
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CANON_TRACKS = ROOT / "pilgrimage" / "tracks"
CANON_DAYS = ROOT / "pilgrimage" / "days"
MIRROR_TRACKS = ROOT / "pilgramage" / "tracks"
MIRROR_DAYS = ROOT / "pilgramage" / "days"

DAYS = [
    (1, "19 Jul", "Lucca → San Miniato", "lucca-san-miniato", ["14-lucca-san-miniato"]),
    (2, "20 Jul", "San Miniato → San Gimignano", "san-miniato-san-gimignano", ["15-san-miniato-san-gimignano"]),
    (3, "21 Jul", "San Gimignano → Siena", "san-gimignano-siena", ["16-san-gimignano-siena"]),
    (4, "22 Jul", "Siena → San Quirico", "siena-san-quirico", ["17-siena-san-quirico"]),
    (5, "23 Jul", "San Quirico → Radicofani", "san-quirico-radicofani", ["18-san-quirico-radicofani"]),
    (6, "24 Jul", "Radicofani → Bolsena", "radicofani-bolsena", ["19-radicofani-bolsena"]),
    (7, "25 Jul", "Bolsena → Sutri", "bolsena-sutri",
     ["20-bolsena-viterbo", "21-viterbo-sutri"]),
    (8, "26 Jul", "Sutri → Rome", "sutri-rome",
     ["22-sutri-formello", "23-formello-roma"]),
]
YEAR = "2026"


def haversine_m(a, b):
    R = 6_371_000
    lat1, lon1 = a
    lat2, lon2 = b
    p1, p2 = math.radians(lat1), math.radians(lat2)
    dp = math.radians(lat2 - lat1)
    dl = math.radians(lon2 - lon1)
    h = math.sin(dp / 2) ** 2 + math.cos(p1) * math.cos(p2) * math.sin(dl / 2) ** 2
    return 2 * R * math.asin(math.sqrt(h))


TRKPT_RE = re.compile(
    r'<trkpt\s+lat="([-0-9.]+)"\s+lon="([-0-9.]+)">\s*'
    r'(?:<ele>([-0-9.]+)</ele>\s*)?'
    r'</trkpt>',
    re.S,
)


def read_trkpts(path):
    text = path.read_text()
    pts = []
    for m in TRKPT_RE.finditer(text):
        lat, lon, ele = float(m.group(1)), float(m.group(2)), m.group(3)
        pts.append((lat, lon, float(ele) if ele is not None else None))
    return pts


def track_length_km(pts):
    d = 0.0
    for i in range(1, len(pts)):
        d += haversine_m(pts[i - 1][:2], pts[i][:2])
    return d / 1000.0


def write_gpx(path, name, desc, pts):
    body = []
    body.append('<?xml version="1.0" encoding="UTF-8"?>')
    body.append(
        '<gpx xmlns="http://www.topografix.com/GPX/1/1" '
        'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" '
        'version="1.1" creator="build-day-gpx.py" '
        'xsi:schemaLocation="http://www.topografix.com/GPX/1/1 '
        'http://www.topografix.com/GPX/1/1/gpx.xsd">'
    )
    if pts:
        lats = [p[0] for p in pts]
        lons = [p[1] for p in pts]
        bounds = f' <bounds minlat="{min(lats):.5f}" minlon="{min(lons):.5f}" maxlat="{max(lats):.5f}" maxlon="{max(lons):.5f}"/>'
    else:
        bounds = ""
    body.append(f"  <metadata><name><![CDATA[{name}]]></name><desc><![CDATA[{desc}]]></desc>{bounds}</metadata>")
    body.append(f"  <trk><name><![CDATA[{name}]]></name><trkseg>")
    for lat, lon, ele in pts:
        if ele is None:
            body.append(f'      <trkpt lat="{lat}" lon="{lon}"></trkpt>')
        else:
            body.append(f'      <trkpt lat="{lat}" lon="{lon}"><ele>{ele}</ele></trkpt>')
    body.append("    </trkseg></trk></gpx>")
    path.write_text("\n".join(body) + "\n")


def merge_day(day_num, date, label, slug, sources):
    """Concatenate source tracks, dedupe seam points <5 m apart."""
    merged = []
    for src in sources:
        pts = read_trkpts(CANON_TRACKS / f"{src}.gpx")
        if merged and pts:
            # seam dedupe: drop first pt of next track if it's basically the last pt of previous
            if haversine_m(merged[-1][:2], pts[0][:2]) < 5:
                pts = pts[1:]
        merged.extend(pts)
    out = CANON_DAYS / f"day-{day_num}-{slug}.gpx"
    write_gpx(out, name=f"Day {day_num} · {label}", desc=f"Via Francigena · {date} {YEAR}", pts=merged)
    return track_length_km(merged), len(merged)


def mirror():
    """Copy days/*.gpx into pilgramage/days/."""
    MIRROR_DAYS.mkdir(parents=True, exist_ok=True)
    for f in CANON_DAYS.glob("*.gpx"):
        (MIRROR_DAYS / f.name).write_bytes(f.read_bytes())


def main(argv):
    report_only = "--report" in argv
    if report_only:
        # Lazy: also splits, since reporting Day-1 km requires 14a
        pass

    print("Per-day distances:")
    total = 0.0
    for day_num, date, label, slug, sources in DAYS:
        km, n = merge_day(day_num, date, label, slug, sources)
        total += km
        print(f"  Day {day_num} ({date}) {label:<35s} {km:6.1f} km  ({n} pts)")
    print(f"  {'TOTAL':<42s} {total:6.1f} km")

    mirror()
    print("\nMirrored to pilgramage/.")


if __name__ == "__main__":
    main(sys.argv[1:])
