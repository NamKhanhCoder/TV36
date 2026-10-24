"""Nguyen Hong Anh: integration checks run only on temporary copies of Data.
Run: python tests/test_schedule_vod.py --exe PATH --output PATH
The repository build scripts never see another C++ main (unit source uses stdin).
"""
import argparse
import ctypes
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
UNIT_SOURCE = r"""
#include "Entities/Schedule.h"
#include <iostream>
#include <functional>
#include <climits>
int main() {
    int checks = 0;
    auto check = [&](bool ok) { ++checks; if (!ok) throw std::runtime_error("unit check " + std::to_string(checks)); };
    auto rejects = [&](std::function<void()> action) {
        try { action(); } catch (const std::invalid_argument&) { check(true); return; }
        check(false);
    };
    check(Schedule::endAfterMinutes("2026-10-24 19:00", 150) == "2026-10-24 21:30");
    check(Schedule::endAfterMinutes("2026-10-24 23:00", 160) == "2026-10-25 01:40");
    check(Schedule::endAfterMinutes("2026-01-31 23:30", 60) == "2026-02-01 00:30");
    check(Schedule::endAfterMinutes("2026-12-31 23:30", 60) == "2027-01-01 00:30");
    check(Schedule::endAfterMinutes("2024-02-28 23:30", 60) == "2024-02-29 00:30");
    check(Schedule::endAfterMinutes("2024-02-29 23:30", 60) == "2024-03-01 00:30");
    check(Schedule::endAfterMinutes("1900-02-28 23:30", 60) == "1900-03-01 00:30");
    check(Schedule::endAfterMinutes("2000-02-28 23:30", 60) == "2000-02-29 00:30");
    check(Schedule::endAfterMinutes("2100-12-31 23:58", 1) == "2100-12-31 23:59");
    check(Schedule::endAfterMinutes("2026-10-24 00:00", 3000) == "2026-10-26 02:00");
    rejects([&]{ Schedule::endAfterMinutes("2100-12-31 23:59", 1); });
    rejects([&]{ Schedule::endAfterMinutes("2026-10-24 00:00", INT_MAX); });
    rejects([&]{ Schedule::endAfterMinutes("2026-02-30 00:00", 60); });
    rejects([&]{ Schedule::endAfterMinutes("2026-10-24 24:00", 60); });
    rejects([&]{ Schedule::endAfterMinutes("2026-10-24 00:00", 0); });
    rejects([&]{ Schedule::endAfterMinutes("2026-10-24 00:00", -1); });
    Schedule s;
    s.parseFromString("LPS001|KENH001|TV|2026-10-24 19:00|2026-10-24 20:00|");
    check(s.getVodID().empty() && s.getDescription().empty());
    const auto saved = s.exportToString();
    check(saved.back() == '|' && s.exportToVector().size() == 7);
    s.parseFromString(saved);
    check(s.exportToString() == saved);
    s.parseFromString("LPS002|KENH001|VOD|2026-10-24 19:00|2026-10-24 20:00||VOD014");
    check(s.getVodID() == "VOD014" && s.getDescription().empty());
    const auto linked = s.exportToString();
    rejects([&]{ s.parseFromString("LPS003|KENH001|Bad|2026-10-24 19:00|2026-10-24 18:00||VOD014"); });
    check(s.exportToString() == linked);
    rejects([&]{ s.parseFromString(linked + "|extra"); });
    rejects([&]{ s.setVodID("VODbad"); });
    check(s.getVodID() == "VOD014");
    s.setVodID(""); check(s.getVodID().empty());
    std::cout << checks << " entity checks passed\n";
}
"""


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output')
    args = parser.parse_args()
    exe = Path(args.exe).resolve()
    out = Path(args.output).resolve() if args.output else Path(tempfile.mkdtemp(prefix='tv36-schedule-tests-'))
    out.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    if os.name == 'nt':
        env['PATH'] = 'C:/msys64/ucrt64/bin;' + env['PATH']
    results = []
    original = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'Data').iterdir() if p.is_file()}

    def record(name, ok, detail=''):
        results.append({'case': name, 'pass': bool(ok), 'detail': detail})
        (out/'results.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')

    def fixture(name, changes=None, backup=False):
        wd = out/name
        shutil.copytree(ROOT/('Data_backup' if backup else 'Data'), wd/'Data')
        for filename, content in (changes or {}).items():
            (wd/'Data'/filename).write_text(content, encoding='utf-8')
        return wd

    def run(name, entries, changes=None, wd=None, readonly=None, backup=False):
        wd = wd or fixture(name, changes, backup)
        text = '\n'.join(entries) + '\n'
        (out/(name+'.input.txt')).write_text(text, encoding='utf-8')
        lock = wd/'Data'/readonly if readonly else None
        if lock:
            if os.name != 'nt': raise RuntimeError('Read-only replacement test requires Windows')
            if not ctypes.windll.kernel32.SetFileAttributesW(str(lock), 1): raise RuntimeError('Cannot set read-only')
        try:
            with (out/(name+'.log')).open('w',encoding='utf-8') as log:
                p = subprocess.run([str(exe)], input=text, cwd=wd, env=env,
                    text=True, encoding='utf-8', stdout=log, stderr=subprocess.STDOUT, timeout=8)
        finally:
            if lock: ctypes.windll.kernel32.SetFileAttributesW(str(lock), 128)
        output = (out/(name+'.log')).read_text(encoding='utf-8', errors='replace')
        if p.returncode != 0: raise RuntimeError(f'{name}: exit {p.returncode}')
        return wd, output

    def rows(wd, filename='schedules.txt'):
        return [r.split('|') for r in (wd/'Data'/filename).read_text(encoding='utf-8').splitlines() if r]

    def get(wd, key, filename='schedules.txt'):
        return next(r for r in rows(wd,filename) if r[0] == key)

    # Compile tests through stdin: no duplicate main in project source tree.
    compiler = shutil.which('g++', path=env['PATH'])
    command = [compiler, '-std=c++17', '-I', str(ROOT), '-x', 'c++', '-',
        str(ROOT/'Entities/Schedule.cpp'), str(ROOT/'Utils/Date.cpp'), str(ROOT/'Utils/Time.cpp'),
        '-o', str(out/'entity-tests.exe')]
    unit = subprocess.run(command, input=UNIT_SOURCE, capture_output=True, text=True, env=env, timeout=60)
    (out/'entity-build.log').write_text(unit.stdout+unit.stderr,encoding='utf-8')
    if unit.returncode: raise RuntimeError(unit.stderr)
    unit = subprocess.run([str(out/'entity-tests.exe')],capture_output=True,text=True,env=env,timeout=10)
    record('entity-date-parser',unit.returncode == 0,unit.stdout+unit.stderr)

    # Create both types, check exact duration and reload from disk.
    wd,log = run('create-vod',['5','1','2','KENH001','VOD014','2026-11-01 23:00','New VOD','','0','0'])
    created = next(r for r in rows(wd) if r[5]=='New VOD')
    record('create-vod', created[3:5]==['2026-11-01 23:00','2026-11-02 01:30'] and created[6]=='VOD014')
    new_id = created[0]
    _,log=run('reopen-vod',['5','2','2',new_id,'','0','0'],wd=wd)
    record('reopen-vod', 'VOD ID: VOD014' in log and 'End: 2026-11-02 01:30' in log)
    wd,log=run('create-tv',['5','1','1','KENH001','Manual TV','2026-11-01 18:00','2026-11-01 19:00','','','0','0'])
    record('create-tv',any(r[2]=='Manual TV' and r[6]=='' for r in rows(wd)))
    wd,log=run('cancel-create',['5','1','2','0','','0','0'])
    record('cancel-create', 'Creation cancelled.' in log and len(rows(wd))==31)
    wd,log=run('invalid-selection',['5','1','2','KENH011','KENH001','VOD999','VOD001','VOD007','VOD014','2026-11-02 20:00','','','0','0'])
    record('selection-validation','broadcasting channel' in log and 'VOD not found.' in log and log.count('VOD must be ACTIVE and HOURLY.')==2 and 'created successfully!' in log)

    # Editing and type conversion.
    wd,log=run('keep-schedule',['5','3','LPS029']+['']*6+['','0','0'])
    record('keep-schedule',get(wd,'LPS029')==next(r.split('|') for r in (ROOT/'Data/schedules.txt').read_text().splitlines() if r.startswith('LPS029|')))
    wd,log=run('convert-tv-to-vod',['5','3','LPS001','2','','VOD014','2026-11-03 18:00','','','0','0'])
    record('convert-tv-to-vod',get(wd,'LPS001')[4]=='2026-11-03 20:30' and get(wd,'LPS001')[6]=='VOD014')
    wd,log=run('vod-to-tv',['5','3','LPS029','1','','Manual converted','','2026-10-24 21:00','','','0','0'])
    record('vod-to-tv',get(wd,'LPS029')[2]=='Manual converted' and get(wd,'LPS029')[6]=='' and get(wd,'LPS029')[4]=='2026-10-24 21:00')
    wd,log=run('change-vod',['5','3','LPS029','','','VOD019','','','','0','0'])
    record('change-vod',get(wd,'LPS029')[2]=='Live Concert 1589' and get(wd,'LPS029')[4]=='2026-10-24 22:40' and get(wd,'LPS029')[6]=='VOD019')
    wd,log=run('change-start',['5','3','LPS029','','','','2026-11-04 23:00','','','0','0'])
    record('change-start',get(wd,'LPS029')[4]=='2026-11-05 01:30')
    wd,log=run('cancel-update',['5','3','LPS029','','','','0','','0','0'])
    record('cancel-update','Update cancelled.' in log and get(wd,'LPS029')[3]=='2026-10-24 20:00')

    # Query results including historical inactive VOD.
    for key,fragment in [('VOD014','Total: 2 record(s)'),('VOD001','NON_HOURLY (on demand)'),('VOD999','VOD not found:'),('VOD003','VOD has not been scheduled yet:')]:
        _,log=run('query-'+key,['5','2','5',key,'','0','0'])
        record('query-'+key,fragment in log)
    wd,log=run('query-chronological',['5','2','5','VOD014','','0','0'])
    record('query-chronological', log.index('| LPS029') < log.index('| LPS030'))
    _,log=run('filter-date',['5','2','4','2026-10-26','','0','0'])
    record('filter-date-overnight','LPS031' in log)
    _,log=run('filter-channel',['5','2','3','KENH001','','0','0'])
    record('filter-channel','LPS029' in log and 'LPS031' in log)
    _,log=run('sort',['5','5','','0','0'])
    record('sort','LPS025' in log and 'LPS029' in log)

    # Shared-channel overlap applies across TV and VOD; adjacent intervals allowed.
    for name,start,end in [('partial','2026-10-24 19:30','2026-10-24 20:30'),('equal','2026-10-24 20:00','2026-10-24 22:30'),('inside','2026-10-24 20:30','2026-10-24 21:00'),('contains','2026-10-24 19:00','2026-10-24 23:00'),('overnight','2026-10-26 00:00','2026-10-26 02:00')]:
        wd,log=run('overlap-'+name,['5','1','1','KENH001','Conflict',start,end,'0','','0','0'])
        record('overlap-'+name,'Schedule overlaps' in log and 'Creation cancelled.' in log and len(rows(wd))==31)
    for name,channel,start,end in [('adjacent','KENH001','2026-10-24 22:30','2026-10-24 23:00'),('other-channel','KENH002','2026-10-24 20:00','2026-10-24 21:00')]:
        wd,log=run(name,['5','1','1',channel,'Allowed',start,end,'','','0','0'])
        record(name,'Schedule created successfully!' in log and len(rows(wd))==32)
    wd,log=run('vod-overlaps-tv',['5','1','2','KENH001','VOD014','2026-10-22 18:00','0','','0','0'])
    record('vod-overlaps-tv','Schedule overlaps LPS001' in log and len(rows(wd))==31)

    # Reject bad end/range then cancel; no mutation.
    for name,start,end in [('equal-time','2026-11-01 10:00','2026-11-01 10:00'),('reverse','2026-11-01 10:00','2026-11-01 09:00')]:
        wd,log=run(name,['5','1','1','KENH001','Bad',start,end,'0','','0','0'])
        record(name,'End time must be after start time.' in log and len(rows(wd))==31)
    wd,log=run('overflow',['5','1','2','KENH001','VOD014','2100-12-31 23:00','0','','0','0'])
    record('overflow','exceeds year 2100' in log and len(rows(wd))==31)

    # VOD guards and snapshot titles.
    update=lambda key,fields:['9','5',key]+fields+['','0','0']
    wd,log=run('block-duration',update('VOD014',['','','151','']+['']*5))
    record('block-duration','Cannot change duration; referenced by schedules: LPS029, LPS030' in log and get(wd,'VOD014','noidungvod.txt')[3]=='150')
    wd,log=run('keep-duration',update('VOD014',['','','150']+['']*5))
    record('keep-duration','Cannot change duration' not in log and get(wd,'VOD014','noidungvod.txt')[3]=='150')
    wd,log=run('block-access',update('VOD014',['']*7+['2','']))
    record('block-access','Cannot change to NON_HOURLY; referenced by schedules:' in log and get(wd,'VOD014','noidungvod.txt')[8]=='HOURLY')
    wd,log=run('block-delete',['9','6','VOD014','','0','0'])
    record('block-delete','Cannot delete VOD; referenced by schedules:' in log and len(rows(wd,'noidungvod.txt'))==20)
    wd,log=run('rename-vod',update('VOD014',['New title']+['']*7))
    record('snapshot-name',get(wd,'VOD014','noidungvod.txt')[1]=='New title' and get(wd,'LPS029')[2]=='Rap Viet Chung Ket Mua 3')
    _,log=run('channel-delete-guard',['4','4','KENH001','y','','0','0'])
    record('channel-delete-guard','referenced by Schedule LPS029' in log)
    wd,log=run('package-delete-guard',['9','6','VOD001','','0','0'])
    record('package-delete-guard','VOD content belongs to package:' in log and len(rows(wd,'noidungvod.txt'))==20)

    # Once all references are gone, duration/type/delete are available again.
    no_refs='\n'.join(r for r in (ROOT/'Data/schedules.txt').read_text().splitlines() if not r.endswith('|VOD014'))+'\n'
    no_packages='\n'.join('|'.join(r.split('|')[:5]+[','.join(v for v in r.split('|')[5].split(',') if v!='VOD014')]) for r in (ROOT/'Data/packages.txt').read_text().splitlines())+'\n'
    wd,log=run('unreferenced-edit',update('VOD014',['','','151']+['']*4+['2']),changes={'schedules.txt':no_refs})
    record('unreferenced-edit',get(wd,'VOD014','noidungvod.txt')[3]=='151' and get(wd,'VOD014','noidungvod.txt')[8]=='NON_HOURLY')
    wd,log=run('unreferenced-delete',['9','6','VOD014','y','','0','0'],changes={'schedules.txt':no_refs,'packages.txt':no_packages})
    record('unreferenced-delete',all(r[0]!='VOD014' for r in rows(wd,'noidungvod.txt')))

    # Historical inactive records may be viewed, kept or deleted, but not rescheduled.
    changed_vods='\n'.join('|'.join(r[:7]+['INACTIVE']+r[8:]) if r[0]=='VOD014' else '|'.join(r) for r in [r.split('|') for r in (ROOT/'Data/noidungvod.txt').read_text().splitlines()])+'\n'
    wd,log=run('inactive-keep',['5','3','LPS029']+['']*6+['','0','0'],changes={'noidungvod.txt':changed_vods})
    record('inactive-keep','Schedule updated successfully!' in log)
    wd,log=run('inactive-reschedule',['5','3','LPS029','','','','2026-11-06 20:00','0','','0','0'],changes={'noidungvod.txt':changed_vods})
    record('inactive-reschedule','VOD must be ACTIVE and HOURLY to place/reschedule.' in log and get(wd,'LPS029')[3]=='2026-10-24 20:00')
    wd,log=run('inactive-delete',['5','4','LPS029','y','','0','0'],changes={'noidungvod.txt':changed_vods})
    record('inactive-delete',all(r[0]!='LPS029' for r in rows(wd)))
    stopped=(ROOT/'Data/channels.txt').read_text().replace('KENH001|TV36 Thoi Su|TL004|1|HD|https://example.com/tv36/live/kenh001.m3u8|DANG_PHAT','KENH001|TV36 Thoi Su|TL004|1|HD|https://example.com/tv36/live/kenh001.m3u8|NGUNG_PHAT')
    wd,log=run('stopped-keep',['5','3','LPS029']+['']*6+['','0','0'],changes={'channels.txt':stopped})
    record('stopped-keep','Schedule updated successfully!' in log)
    wd,log=run('stopped-reschedule',['5','3','LPS029','','','','2026-11-06 20:00','0','','0','0'],changes={'channels.txt':stopped})
    record('stopped-reschedule','Cannot place/reschedule on a stopped channel.' in log)

    # Delete cancel / confirm, legacy migration, backup and counter recovery.
    wd,log=run('delete-cancel',['5','4','LPS029','n','','0','0'])
    record('delete-cancel',len(rows(wd))==31 and 'Deletion cancelled.' in log)
    wd,log=run('delete-confirm',['5','4','LPS029','y','','0','0'])
    record('delete-confirm',len(rows(wd))==30)
    legacy='LPS001|KENH001|Legacy|2026-11-01 18:00|2026-11-01 19:00|\n'
    wd,log=run('legacy-save',['5','3','LPS001']+['']*7+['','0','0'],changes={'schedules.txt':legacy})
    record('legacy-save',rows(wd)==[['LPS001','KENH001','Legacy','2026-11-01 18:00','2026-11-01 19:00','','']])
    wd,log=run('backup-load',['5','5','','0','9','2','','0','0'],backup=True)
    record('backup-load','Data corrupted' not in log and 'Total: 31 record(s)' in log and 'Total: 20 record(s)' in log)
    wd,log=run('counter-recovery',['5','1','1','KENH001','Unique','2026-11-10 18:00','2026-11-10 19:00','','','0','0'],changes={'schedules_counter.txt':'28\n'})
    record('counter-recovery',len(rows(wd))==32 and len({r[0] for r in rows(wd)})==32)

    # Fail all three schedule writes and a VOD update; originals and RAM survive.
    if os.name=='nt':
        for name,entries in [
            ('readonly-create',['5','1','1','KENH001','Not saved','2026-11-11 18:00','2026-11-11 19:00','','','2','1','','0','0']),
            ('readonly-update',['5','3','LPS029']+['']*4+['Not saved','','2','2','LPS029','','0','0']),
            ('readonly-delete',['5','4','LPS029','y','','2','2','LPS029','','0','0'])]:
            wd,log=run(name,entries,readonly='schedules.txt')
            record(name,(wd/'Data/schedules.txt').read_bytes()==(ROOT/'Data/schedules.txt').read_bytes() and '[Error] Cannot replace data file' in log and not list((wd/'Data').glob('*.pending-*')))
        wd,log=run('readonly-vod-update',update('VOD014',['Not saved']+['']*7),readonly='noidungvod.txt')
        record('readonly-vod-update',(wd/'Data/noidungvod.txt').read_bytes()==(ROOT/'Data/noidungvod.txt').read_bytes() and 'Memory rolled back' in log)

    record('source-data-unchanged',original=={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'Data').iterdir() if p.is_file()})
    (out/'results.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
    failed=[r for r in results if not r['pass']]
    print(json.dumps({'output':str(out),'passed':len(results)-len(failed),'total':len(results),'failed':failed},ensure_ascii=False,indent=2))
    return bool(failed)

if __name__=='__main__':
    raise SystemExit(main())
