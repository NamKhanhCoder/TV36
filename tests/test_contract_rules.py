"""Contract regression scenarios, using only disposable copies of Data.

Run: python tests/test_contract_rules.py --exe PATH --output NEW_DIRECTORY
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output')
    parser.add_argument('--group', choices=['all', 'allocation', 'months'], default='all')
    args = parser.parse_args()
    exe = Path(args.exe).resolve()
    out = Path(args.output).resolve() if args.output else Path(tempfile.mkdtemp(prefix='tv36-contract-'))
    out.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    if os.name == 'nt':
        env['PATH'] = 'C:/msys64/ucrt64/bin;' + env['PATH']
    hashes = lambda: {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'Data').iterdir() if p.is_file()}
    original = hashes()
    results = []

    def rows(wd, name='contracts'):
        return [line.split('|') for line in (wd/'Data'/(name+'.txt')).read_text(encoding='utf8').splitlines() if line]

    def get(data, key):
        return next(row for row in data if row[0] == key)

    def record(name, passed, detail=''):
        results.append({'case': name, 'pass': bool(passed), 'detail': detail})
        (out/'results.json').write_text(json.dumps(results, indent=2), encoding='utf8')
        print(('PASS ' if passed else 'FAIL ') + name, flush=True)

    def run(name, operation, entries, check, changes=()):
        wd = out/name
        shutil.copytree(ROOT/'Data', wd/'Data')
        for file, key, column, value in changes:
            data = rows(wd, file)
            get(data, key)[column] = value
            (wd/'Data'/(file+'.txt')).write_text('\n'.join('|'.join(r) for r in data)+'\n', encoding='utf8')
        before = rows(wd)
        # Extra exit choices also finish the old implementation when rejected
        # input used to enter a different prompt path.
        text = '\n'.join(['7', str(operation), *entries, *(['0']*8)])+'\n'
        (out/(name+'.input.txt')).write_text(text, encoding='utf8')
        try:
            with (out/(name+'.log')).open('w', encoding='utf8') as log:
                p = subprocess.run([str(exe)], input=text, text=True, encoding='utf8', env=env,
                                   cwd=wd, stdout=log, stderr=subprocess.STDOUT, timeout=8)
            output = (out/(name+'.log')).read_text(encoding='utf8', errors='replace')
            after = rows(wd)
            record(name, p.returncode == 0 and check(before, after, output), f'exit={p.returncode}')
        except subprocess.TimeoutExpired:
            record(name, False, 'Input flow did not finish in 8 seconds')

    update = lambda key, package='', stb='', months='', status='': [key, package, stb, months, '', status, '']
    suspended = [('settopboxes', 'STB007', 5, 'TOT')]
    if args.group in ['all', 'allocation']:
        run('reject-suspended-stb-swap', 3, ['HD010', '', 'STB007', '', '', '', '', ''],
            lambda b,a,s: get(a,'HD010')[3] == get(b,'HD010')[3], suspended)
        run('reject-active-stb-swap', 3, ['HD010', '', 'STB002', '', '', '', '', ''],
            lambda b,a,s: get(a,'HD010')[3] == get(b,'HD010')[3])
        run('allow-free-stb-swap', 3, update('HD010', stb='STB011'),
            lambda b,a,s: get(a,'HD010')[3] == 'STB011')
        run('keep-own-stb', 3, update('HD010', stb='STB010', status='1'), lambda b,a,s: a == b)
        for status in ['1','2']:
            for owner in ['ACTIVE','SUSPENDED']:
                run('reject-reactivate-'+status+'-'+owner, 3, update('HD008', status=status),
                    lambda b,a,s: get(a,'HD008')[9] == 'EXPIRED',
                    suspended+[('contracts','HD008',3,'STB007'),('contracts','HD007',9,owner)])
        run('allow-free-reactivation', 3, update('HD008', status='2'),
            lambda b,a,s: get(a,'HD008')[9] == 'SUSPENDED')
        run('reject-extension-reactivation', 3, ['HD008','','','1','y','','0',''],
            lambda b,a,s: get(a,'HD008')[9] == 'EXPIRED' and get(a,'HD008')[6] != get(b,'HD008')[6],
            suspended+[('contracts','HD008',3,'STB007')])
        run('reject-closed-package-create', 1, ['KH011','GOI008','0',''],
            lambda b,a,s: a == b and 'not available for new contracts' in s)
        run('reject-closed-package-update', 3, ['HD001','GOI008','','','','','',''],
            lambda b,a,s: get(a,'HD001')[2] == get(b,'HD001')[2])
        run('keep-historical-package', 3, update('HD008', package='GOI008'), lambda b,a,s: a == b)
        run('allow-open-package-update', 3, ['HD001','GOI002','','','','','',''],
            lambda b,a,s: get(a,'HD001')[2] == 'GOI002')
        create = ['KH011','GOI001','STB011','25/10/2026','25/10/2026','12','Ha Noi','','1','']
        run('allow-contract-create', 1, create, lambda b,a,s: len(a)==len(b)+1 and a[-1][2:4]==['GOI001','STB011'])
        bad_create = ['KH011','GOI001','STB007','0','25/10/2026','25/10/2026','12','Ha Noi','','1','','0']
        run('reject-suspended-stb-create', 1, bad_create, lambda b,a,s: a == b, suspended)
    if args.group in ['all', 'months']:
        for value in ['1abc', '1.5', '-1', '2147483647', '999999999999999999999']:
            run('reject-months-'+value.replace('.','dot'), 3, ['HD001','','',value,'','','',''], lambda b,a,s: a == b)
        for value in ['', '0']:
            run('keep-months-'+(value or 'enter'), 3, update('HD001', months=value), lambda b,a,s: a == b)
        run('extend-one-month', 3, update('HD001', months='1'),
            lambda b,a,s: get(a,'HD001')[6]=='01/07/2027')
        run('reject-end-after-2100', 3, ['HD001','','','1','','','',''], lambda b,a,s: a == b,
            [('contracts','HD001',6,'31/12/2100')])
        unit_source = r'''
#include "Utils/Date.h"
#include <climits>
#include <iostream>
#include <stdexcept>
int main() {
    int failed=0;
    auto check=[&](bool ok,const char* name){ if(!ok){++failed;std::cout<<name<<"\n";} };
    Date leap(31,1,2024); leap.addMonths(1); check(leap.toString()=="29/02/2024","leap");
    Date common(31,1,2026); common.addMonths(1); check(common.toString()=="28/02/2026","month-end");
    Date last(30,11,2100); last.addMonths(1); check(last.toString()=="30/12/2100","last-valid-month");
    for(int amount: {1, INT_MAX}) {
        Date d(31,12,2100); const auto before=d.toString(); bool rejected=false;
        try {d.addMonths(amount);} catch(const std::exception&) {rejected=true;}
        check(rejected && d.toString()==before,"overflow must reject without changing date");
    }
    Date empty; bool rejected=false; try{empty.addMonths(1);}catch(const std::exception&){rejected=true;}
    check(rejected && empty.isEmpty(),"empty date must reject");
    std::cout<<"failed="<<failed<<"\n"; return failed ? 1 : 0;
}
'''
        compiler = shutil.which('g++', path=env['PATH'])
        p = subprocess.run([compiler,'-std=c++17','-I',str(ROOT),'-x','c++','-',str(ROOT/'Utils/Date.cpp'),'-o',str(out/'date-tests.exe')],
                           input=unit_source,text=True,capture_output=True,env=env,timeout=60)
        if p.returncode:
            raise RuntimeError(p.stderr)
        p = subprocess.run([str(out/'date-tests.exe')],text=True,capture_output=True,env=env,timeout=8)
        record('date-boundaries',p.returncode==0,p.stdout+p.stderr)
    record('source-data-unchanged', hashes() == original)
    print(json.dumps({'passed':sum(r['pass'] for r in results),'total':len(results)}))
    return int(any(not r['pass'] for r in results))


if __name__ == '__main__':
    raise SystemExit(main())
